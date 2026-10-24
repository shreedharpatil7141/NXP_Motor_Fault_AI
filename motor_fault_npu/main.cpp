#include "board_init.h"
#include "fsl_debug_console.h"
#include "model.h"
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
        PRINTF("%lu ", (unsigned long)inputDims.data[i]);

    PRINTF("\r\n");

    PRINTF("Output type: %d\r\n", outputType);
    PRINTF("Output shape: ");

    for (uint32_t i = 0; i < outputDims.size; i++)
        PRINTF("%lu ", (unsigned long)outputDims.data[i]);

    PRINTF("\r\n");

    int8_t *input = reinterpret_cast<int8_t *>(inputData);
    int8_t *output = reinterpret_cast<int8_t *>(outputData);
    uint32_t minLatency = 0xffffffffu;
    uint32_t maxLatency = 0;
    uint64_t totalLatency = 0;
    uint32_t argmaxMatches = 0;
    uint32_t labelMatches = 0;
    int maxAbsDiff = 0;

    PRINTF("Classes: Normal=0 IR=1 OR=2 B=3\r\n");
    for (uint32_t vectorIndex = 0; vectorIndex < NUM_TEST_VECTORS; vectorIndex++)
    {
        memcpy(input, test_inputs[vectorIndex], NUM_BINS * sizeof(int8_t));
        uint32_t startTime = TIMER_GetTimeInUS();
        if (MODEL_RunInference() != kStatus_Success)
        {
            PRINTF("ERROR: Inference failed for vector %lu\r\n",
                   (unsigned long)vectorIndex);
            while (1) {}
        }
        uint32_t latency = TIMER_GetTimeInUS() - startTime;
        minLatency = latency < minLatency ? latency : minLatency;
        maxLatency = latency > maxLatency ? latency : maxLatency;
        totalLatency += latency;

        int predicted = 0;
        int expectedPredicted = 0;
        int vectorMaxAbsDiff = 0;
        for (int outputIndex = 1; outputIndex < 4; outputIndex++)
        {
            if (output[outputIndex] > output[predicted])
                predicted = outputIndex;
            if (test_expected_out[vectorIndex][outputIndex] >
                test_expected_out[vectorIndex][expectedPredicted])
                expectedPredicted = outputIndex;
        }
        for (int outputIndex = 0; outputIndex < 4; outputIndex++)
        {
            int difference = static_cast<int>(output[outputIndex]) -
                             static_cast<int>(test_expected_out[vectorIndex][outputIndex]);
            int absoluteDifference = difference < 0 ? -difference : difference;
            vectorMaxAbsDiff = absoluteDifference > vectorMaxAbsDiff
                                   ? absoluteDifference
                                   : vectorMaxAbsDiff;
        }
        maxAbsDiff = vectorMaxAbsDiff > maxAbsDiff ? vectorMaxAbsDiff : maxAbsDiff;
        bool argmaxMatch = predicted == expectedPredicted;
        bool labelMatch = predicted == test_labels[vectorIndex];
        argmaxMatches += argmaxMatch ? 1u : 0u;
        labelMatches += labelMatch ? 1u : 0u;

        PRINTF("Vector %lu: label=%u expected_argmax=%d actual_argmax=%d "
               "out=[%d,%d,%d,%d] expected=[%d,%d,%d,%d] diff_max=%d latency=%lu us\r\n",
               (unsigned long)vectorIndex, test_labels[vectorIndex], expectedPredicted,
               predicted, output[0], output[1], output[2], output[3],
               test_expected_out[vectorIndex][0], test_expected_out[vectorIndex][1],
               test_expected_out[vectorIndex][2], test_expected_out[vectorIndex][3],
               vectorMaxAbsDiff, (unsigned long)latency);
    }

    PRINTF("Summary: argmax=%lu/%u label_accuracy=%lu/%u max_abs_diff=%d "
           "latency_us[min/mean/max]=%lu/%lu/%lu\r\n",
           (unsigned long)argmaxMatches, NUM_TEST_VECTORS,
           (unsigned long)labelMatches, NUM_TEST_VECTORS, maxAbsDiff,
           (unsigned long)minLatency,
           (unsigned long)(totalLatency / NUM_TEST_VECTORS),
           (unsigned long)maxLatency);

    while (1)
    {
    }
}
