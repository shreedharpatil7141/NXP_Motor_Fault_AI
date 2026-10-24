#include "board_init.h"
#include "fsl_debug_console.h"
#include "model.h"
#include "timer.h"

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

    /*
     * TEST INPUT
     *
     * Model expects 513 INT8 samples.
     *
     * For now use the quantized zero-point value.
     * zero_point = -78
     *
     * IMPORTANT:
     * We write directly into the INT8 tensor.
     * Therefore we do NOT call MODEL_ConvertInput().
     */
    int8_t *input = reinterpret_cast<int8_t *>(inputData);

    for (int i = 0; i < 513; i++)
    {
        input[i] = -78;
    }

    uint32_t startTime = TIMER_GetTimeInUS();

    if (MODEL_RunInference() != kStatus_Success)
    {
        PRINTF("ERROR: Inference failed\r\n");
        while (1) {}
    }

    uint32_t endTime = TIMER_GetTimeInUS();

    PRINTF("Inference time: %lu us\r\n",
           (unsigned long)(endTime - startTime));

    int8_t *output = reinterpret_cast<int8_t *>(outputData);

    PRINTF("Raw output: ");

    for (int i = 0; i < 4; i++)
    {
        PRINTF("%d ", output[i]);
    }

    PRINTF("\r\n");

    PRINTF("Output probabilities: ");

    for (int i = 0; i < 4; i++)
    {
        float probability =
            ((float)output[i] + 128.0f) * 0.00390625f;

        PRINTF("%.3f ", probability);
    }

    PRINTF("\r\n");

    int best = 0;

    for (int i = 1; i < 4; i++)
    {
        if (output[i] > output[best])
            best = i;
    }

    PRINTF("Predicted class: %d\r\n", best);

    while (1)
    {
    }
}
