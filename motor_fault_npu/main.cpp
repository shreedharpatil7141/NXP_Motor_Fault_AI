#include "board_init.h"
#include "fsl_debug_console.h"
#include "model.h"
#include "preprocess.h"
#include "raw_vectors.h"
#include "test_vectors.h"
#include "timer.h"

#include <cstdint>
#include <cstring>

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

    PRINTF("Input type: %d\r\n", inputType);
    PRINTF("Input shape: ");

    for (uint32_t i = 0; i < inputDims.size; i++)
        PRINTF("%u ", static_cast<unsigned int>(inputDims.data[i]));

    PRINTF("\r\n");

    PRINTF("Output type: %d\r\n", outputType);
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
        int8_t features[RAW_FEATURE_SIZE];
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
               expectedPredicted, predicted, static_cast<int>(output[0]),
               static_cast<int>(output[1]), static_cast<int>(output[2]),
               static_cast<int>(output[3]), static_cast<int>(raw_expected_out[vectorIndex][0]),
               static_cast<int>(raw_expected_out[vectorIndex][1]),
               static_cast<int>(raw_expected_out[vectorIndex][2]),
               static_cast<int>(raw_expected_out[vectorIndex][3]), vectorMaxAbsDiff,
               static_cast<unsigned int>(vectorDiffOverOne),
               static_cast<unsigned int>(preprocessLatency),
               static_cast<unsigned int>(inferenceLatency));
    }

    PRINTF("Summary: expected_argmax=%u/%u label_accuracy=%u/%u "
           "feature_max_abs_diff=%d feature_diff_gt1=%u\r\n",
           static_cast<unsigned int>(featureArgmaxMatches), RAW_NUM_WINDOWS,
           static_cast<unsigned int>(labelMatches), RAW_NUM_WINDOWS,
           featureMaxAbsDiff, static_cast<unsigned int>(featureDiffOverOne));
    PRINTF("Latency_us[preprocess min/mean/max]=%u/%u/%u "
           "inference min/mean/max=%u/%u/%u\r\n",
           static_cast<unsigned int>(preprocessMin),
           static_cast<unsigned int>(preprocessTotal / RAW_NUM_WINDOWS),
           static_cast<unsigned int>(preprocessMax),
           static_cast<unsigned int>(inferenceMin),
           static_cast<unsigned int>(inferenceTotal / RAW_NUM_WINDOWS),
           static_cast<unsigned int>(inferenceMax));

    while (1)
    {
    }
}
