#include "board_init.h"
#include "fsl_clock.h"
#include "fsl_ctimer.h"
#include "fsl_debug_console.h"
#include "model.h"
#include "preprocess.h"
#include "raw_vectors.h"
#include "test_vectors.h"
#include "timer.h"

#include <cstdint>
#include <cstring>

extern "C" void hard_fault_report(uint32_t *stacked);

extern "C" __attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile(
        "tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b hard_fault_report\n");
}

extern "C" void hard_fault_report(uint32_t *stacked)
{
    PRINTF("HardFault CFSR=0x%08x HFSR=0x%08x PC=0x%08x LR=0x%08x\r\n",
           (unsigned int)SCB->CFSR, (unsigned int)SCB->HFSR,
           (unsigned int)stacked[6], (unsigned int)stacked[5]);
    while (1) {}
}

namespace
{
constexpr uint32_t kSampleRateHz = 12000U;
constexpr uint32_t kStreamWindowSize = 1024U;
constexpr uint32_t kStreamHopSize = 512U;
constexpr uint32_t kStreamWindowCount = 200U;
constexpr uint32_t kStreamRingSize = 4096U;
constexpr uint32_t kStreamTimerIndex = 4U;
CTIMER_Type *const kStreamTimer = CTIMER4;

float s_streamRing[kStreamRingSize] __ALIGNED(16);
volatile uint32_t s_streamProduced = 0;
volatile uint32_t s_streamOldest = 0;
volatile uint32_t s_streamDropped = 0;

void stream_timer_callback(uint32_t flags)
{
    (void)flags;
    uint32_t produced = s_streamProduced;
    uint32_t sourceIndex = produced % (RAW_NUM_WINDOWS * RAW_WINDOW_SIZE);
    uint32_t sourceWindow = sourceIndex / RAW_WINDOW_SIZE;
    uint32_t sourceSample = sourceIndex % RAW_WINDOW_SIZE;
    s_streamRing[produced % kStreamRingSize] =
        raw_windows[sourceWindow][sourceSample];
    s_streamProduced = produced + 1U;
    if ((produced + 1U) - s_streamOldest > kStreamRingSize)
    {
        s_streamOldest++;
        s_streamDropped++;
    }
}

ctimer_callback_t s_streamCallbackTable[] = {
    stream_timer_callback, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void stream_timer_init()
{
    CLOCK_SetClkDiv(kCLOCK_DivCtimer4Clk, 1U);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER4);

    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(kStreamTimer, &config);
    CTIMER_RegisterCallBack(kStreamTimer, &s_streamCallbackTable[0],
                            kCTIMER_MultipleCallback);

    ctimer_match_config_t matchConfig;
    matchConfig.enableCounterReset = true;
    matchConfig.enableCounterStop = false;
    matchConfig.matchValue =
        CLOCK_GetCTimerClkFreq(kStreamTimerIndex) / kSampleRateHz;
    matchConfig.outControl = kCTIMER_Output_NoAction;
    matchConfig.outPinInitState = false;
    matchConfig.enableInterrupt = true;
    PRINTF("checkpoint CTIMER4 interrupt enable before %d\r\n", (int)1);
    CTIMER_SetupMatch(kStreamTimer, kCTIMER_Match_0, &matchConfig);
    PRINTF("checkpoint CTIMER4 interrupt enable after %d\r\n", (int)1);
}

bool stream_copy_latest_window(float *window)
{
    uint32_t produced = s_streamProduced;
    uint32_t oldest = s_streamOldest;
    if (produced < kStreamWindowSize ||
        produced - kStreamWindowSize < oldest)
    {
        return false;
    }

    uint32_t first = produced - kStreamWindowSize;
    for (uint32_t index = 0; index < kStreamWindowSize; ++index)
    {
        window[index] = s_streamRing[(first + index) % kStreamRingSize];
    }
    return true;
}
}

int main(void)
{
    BOARD_Init();
    TIMER_Init();

    PRINTF("\r\n=== Motor Fault AI - MCXN947 NPU ===\r\n");

    if (MODEL_Init() != kStatus_Success)
    {
        PRINTF("ERROR: MODEL_Init failed\r\n");
        while (1) {}
    }

    tensor_dims_t inputDims;
    tensor_type_t inputType;

    uint8_t *inputData =
        MODEL_GetInputTensorData(&inputDims, &inputType);

    tensor_dims_t outputDims;
    tensor_type_t outputType;

    uint8_t *outputData =
        MODEL_GetOutputTensorData(&outputDims, &outputType);

    PRINTF("Input type: %d\r\n", (int)inputType);
    PRINTF("Input shape: ");

    for (uint32_t i = 0; i < inputDims.size; i++)
        PRINTF("%u ", static_cast<unsigned int>(inputDims.data[i]));

    PRINTF("\r\n");

    PRINTF("Output type: %d\r\n", (int)outputType);
    PRINTF("Output shape: ");

    for (uint32_t i = 0; i < outputDims.size; i++)
        PRINTF("%u ", static_cast<unsigned int>(outputDims.data[i]));

    PRINTF("\r\n");

    int8_t *input = reinterpret_cast<int8_t *>(inputData);
    int8_t *output = reinterpret_cast<int8_t *>(outputData);
    uint32_t preprocessMin = 0xffffffffu;
    uint32_t preprocessMax = 0;
    uint64_t preprocessTotal = 0;
    uint32_t inferenceMin = 0xffffffffu;
    uint32_t inferenceMax = 0;
    uint64_t inferenceTotal = 0;
    uint32_t featureArgmaxMatches = 0;
    uint32_t labelMatches = 0;
    int featureMaxAbsDiff = 0;
    uint32_t featureDiffOverOne = 0;

    PRINTF("Classes: Normal=0 IR=1 OR=2 B=3\r\n");
    preprocess_debug_once(raw_windows[0]);
    for (uint32_t vectorIndex = 0; vectorIndex < RAW_NUM_WINDOWS; vectorIndex++)
    {
        static int8_t features[RAW_FEATURE_SIZE];
        uint32_t preprocessStart = TIMER_GetTimeInUS();
        preprocess_window(raw_windows[vectorIndex], features);
        uint32_t preprocessLatency = TIMER_GetTimeInUS() - preprocessStart;
        preprocessMin = preprocessLatency < preprocessMin ? preprocessLatency : preprocessMin;
        preprocessMax = preprocessLatency > preprocessMax ? preprocessLatency : preprocessMax;
        preprocessTotal += preprocessLatency;

        int vectorMaxAbsDiff = 0;
        uint32_t vectorDiffOverOne = 0;
        for (uint32_t featureIndex = 0; featureIndex < RAW_FEATURE_SIZE; featureIndex++)
        {
            int difference = static_cast<int>(features[featureIndex]) -
                             static_cast<int>(raw_expected_feat[vectorIndex][featureIndex]);
            int absoluteDifference = difference < 0 ? -difference : difference;
            vectorMaxAbsDiff = absoluteDifference > vectorMaxAbsDiff
                                   ? absoluteDifference
                                   : vectorMaxAbsDiff;
            vectorDiffOverOne += absoluteDifference > 1 ? 1u : 0u;
            input[featureIndex] = features[featureIndex];
        }
        featureMaxAbsDiff = vectorMaxAbsDiff > featureMaxAbsDiff
                                ? vectorMaxAbsDiff
                                : featureMaxAbsDiff;
        featureDiffOverOne += vectorDiffOverOne;

        if (vectorIndex == 0U)
        {
            PRINTF("Feature probe: fw=[%d,%d,%d,%d,%d,%d,%d,%d] "
                   "ref=[%d,%d,%d,%d,%d,%d,%d,%d]\r\n",
                   static_cast<int>(features[0]), static_cast<int>(features[1]),
                   static_cast<int>(features[2]), static_cast<int>(features[3]),
                   static_cast<int>(features[4]), static_cast<int>(features[5]),
                   static_cast<int>(features[6]), static_cast<int>(features[7]),
                   static_cast<int>(raw_expected_feat[0][0]),
                   static_cast<int>(raw_expected_feat[0][1]),
                   static_cast<int>(raw_expected_feat[0][2]),
                   static_cast<int>(raw_expected_feat[0][3]),
                   static_cast<int>(raw_expected_feat[0][4]),
                   static_cast<int>(raw_expected_feat[0][5]),
                   static_cast<int>(raw_expected_feat[0][6]),
                   static_cast<int>(raw_expected_feat[0][7]));
        }

        uint32_t inferenceStart = TIMER_GetTimeInUS();
        if (MODEL_RunInference() != kStatus_Success)
        {
            PRINTF("ERROR: Inference failed for vector %u\r\n",
                   static_cast<unsigned int>(vectorIndex));
            while (1) {}
        }
        uint32_t inferenceLatency = TIMER_GetTimeInUS() - inferenceStart;
        inferenceMin = inferenceLatency < inferenceMin ? inferenceLatency : inferenceMin;
        inferenceMax = inferenceLatency > inferenceMax ? inferenceLatency : inferenceMax;
        inferenceTotal += inferenceLatency;

        int predicted = 0;
        int expectedPredicted = 0;
        for (int outputIndex = 1; outputIndex < 4; outputIndex++)
        {
            if (output[outputIndex] > output[predicted])
                predicted = outputIndex;
            if (raw_expected_out[vectorIndex][outputIndex] >
                raw_expected_out[vectorIndex][expectedPredicted])
                expectedPredicted = outputIndex;
        }
        bool argmaxMatch = predicted == expectedPredicted;
        bool labelMatch = predicted == raw_labels[vectorIndex];
        featureArgmaxMatches += argmaxMatch ? 1u : 0u;
        labelMatches += labelMatch ? 1u : 0u;

        PRINTF("Vector %u: label=%u expected_argmax=%d actual_argmax=%d "
               "out=[%d,%d,%d,%d] expected=[%d,%d,%d,%d] "
               "feat_diff_max=%d feat_diff_gt1=%u preprocess=%u us inference=%u us\r\n",
               static_cast<unsigned int>(vectorIndex), raw_labels[vectorIndex],
               (int)expectedPredicted, (int)predicted, static_cast<int>(output[0]),
               static_cast<int>(output[1]), static_cast<int>(output[2]),
               static_cast<int>(output[3]), static_cast<int>(raw_expected_out[vectorIndex][0]),
               static_cast<int>(raw_expected_out[vectorIndex][1]),
               static_cast<int>(raw_expected_out[vectorIndex][2]),
               static_cast<int>(raw_expected_out[vectorIndex][3]),
               (int)vectorMaxAbsDiff, static_cast<unsigned int>(vectorDiffOverOne),
               static_cast<unsigned int>(preprocessLatency),
               static_cast<unsigned int>(inferenceLatency));
    }

    PRINTF("Summary: expected_argmax=%u/%u label_accuracy=%u/%u "
           "feature_max_abs_diff=%d feature_diff_gt1=%u\r\n",
           static_cast<unsigned int>(featureArgmaxMatches), RAW_NUM_WINDOWS,
           static_cast<unsigned int>(labelMatches), RAW_NUM_WINDOWS,
           (int)featureMaxAbsDiff, static_cast<unsigned int>(featureDiffOverOne));
    PRINTF("Latency_us[preprocess min/mean/max]=%u/%u/%u "
           "inference min/mean/max=%u/%u/%u\r\n",
           static_cast<unsigned int>(preprocessMin),
           static_cast<unsigned int>(preprocessTotal / RAW_NUM_WINDOWS),
           static_cast<unsigned int>(preprocessMax),
           static_cast<unsigned int>(inferenceMin),
           static_cast<unsigned int>(inferenceTotal / RAW_NUM_WINDOWS),
           static_cast<unsigned int>(inferenceMax));

    PRINTF("\r\n=== Streaming simulation: 12 kHz, 50%% overlap ===\r\n");
    PRINTF("Window period_us=42667 samples_per_window=%d hop=%d\r\n",
           (int)kStreamWindowSize, (int)kStreamHopSize);
    static float streamWindow[kStreamWindowSize] __ALIGNED(16);
    static int8_t streamFeatures[RAW_FEATURE_SIZE];
    uint32_t streamHistogram[4] = {0, 0, 0, 0};
    uint64_t streamPreprocessTotal = 0;
    uint64_t streamInferenceTotal = 0;
    uint64_t streamTotalTotal = 0;
    uint32_t streamPreprocessMin = 0xffffffffU;
    uint32_t streamPreprocessMax = 0;
    uint32_t streamInferenceMin = 0xffffffffU;
    uint32_t streamInferenceMax = 0;
    uint32_t streamTotalMin = 0xffffffffU;
    uint32_t streamTotalMax = 0;
    uint32_t nextWindowEnd = kStreamWindowSize;
    uint32_t processedWindows = 0;
    uint32_t streamOverruns = 0;

    PRINTF("checkpoint CTIMER4 init before %d\r\n", (int)1);
    stream_timer_init();
    PRINTF("checkpoint CTIMER4 init after %d\r\n", (int)1);
    PRINTF("checkpoint CTIMER4 start before %d\r\n", (int)1);
    CTIMER_StartTimer(kStreamTimer);
    PRINTF("checkpoint CTIMER4 start after %d\r\n", (int)1);
    PRINTF("checkpoint streaming loop entry %d\r\n", (int)1);
    while (processedWindows < kStreamWindowCount)
    {
        if (s_streamProduced < nextWindowEnd)
        {
            continue;
        }
        uint32_t totalStart = TIMER_GetTimeInUS();
        if (!stream_copy_latest_window(streamWindow))
        {
            s_streamDropped++;
            nextWindowEnd += kStreamHopSize;
            continue;
        }
        uint32_t preprocessStart = TIMER_GetTimeInUS();
        preprocess_window(streamWindow, streamFeatures);
        uint32_t preprocessLatency = TIMER_GetTimeInUS() - preprocessStart;
        for (uint32_t featureIndex = 0; featureIndex < RAW_FEATURE_SIZE;
             ++featureIndex)
        {
            input[featureIndex] = streamFeatures[featureIndex];
        }

        uint32_t inferenceStart = TIMER_GetTimeInUS();
        if (MODEL_RunInference() != kStatus_Success)
        {
            PRINTF("ERROR: Streaming inference failed\r\n");
            while (1) {}
        }
        uint32_t inferenceLatency = TIMER_GetTimeInUS() - inferenceStart;
        uint32_t totalLatency = TIMER_GetTimeInUS() - totalStart;
        int predicted = 0;
        for (int outputIndex = 1; outputIndex < 4; ++outputIndex)
        {
            if (output[outputIndex] > output[predicted])
            {
                predicted = outputIndex;
            }
        }
        streamHistogram[predicted]++;
        streamPreprocessTotal += preprocessLatency;
        streamInferenceTotal += inferenceLatency;
        streamTotalTotal += totalLatency;
        streamPreprocessMin = preprocessLatency < streamPreprocessMin
                                  ? preprocessLatency
                                  : streamPreprocessMin;
        streamPreprocessMax = preprocessLatency > streamPreprocessMax
                                  ? preprocessLatency
                                  : streamPreprocessMax;
        streamInferenceMin = inferenceLatency < streamInferenceMin
                                 ? inferenceLatency
                                 : streamInferenceMin;
        streamInferenceMax = inferenceLatency > streamInferenceMax
                                 ? inferenceLatency
                                 : streamInferenceMax;
        streamTotalMin =
            totalLatency < streamTotalMin ? totalLatency : streamTotalMin;
        streamTotalMax =
            totalLatency > streamTotalMax ? totalLatency : streamTotalMax;
        if (s_streamProduced >= nextWindowEnd + kStreamHopSize)
        {
            streamOverruns++;
        }
        processedWindows++;
        nextWindowEnd += kStreamHopSize;
    }

    CTIMER_StopTimer(kStreamTimer);
    uint32_t meanTotal = static_cast<uint32_t>(
        streamTotalTotal / kStreamWindowCount);
    uint32_t cpuLoadPermille = (meanTotal * 100000U) / 42667U;
    PRINTF("Streaming class histogram: Normal=%d IR=%d OR=%d B=%d\r\n",
           (int)streamHistogram[0], (int)streamHistogram[1],
           (int)streamHistogram[2], (int)streamHistogram[3]);
    PRINTF("Streaming windows=%d overruns=%d dropped_samples=%d\r\n",
           (int)processedWindows, (int)streamOverruns, (int)s_streamDropped);
    PRINTF("Streaming us[preprocess min/mean/max]=%d/%d/%d "
           "inference=%d/%d/%d total=%d/%d/%d\r\n",
           (int)streamPreprocessMin,
           (int)(streamPreprocessTotal / kStreamWindowCount),
           (int)streamPreprocessMax, (int)streamInferenceMin,
           (int)(streamInferenceTotal / kStreamWindowCount),
           (int)streamInferenceMax, (int)streamTotalMin,
           (int)(streamTotalTotal / kStreamWindowCount),
           (int)streamTotalMax);
    PRINTF("Streaming period_us=42667 CPU_load_percent_x1000=%d\r\n",
           (int)cpuLoadPermille);

    while (1)
    {
    }
}
