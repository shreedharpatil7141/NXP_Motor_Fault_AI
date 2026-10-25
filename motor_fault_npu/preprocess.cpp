#include "preprocess.h"

#ifndef USE_POWERQUAD
#define USE_POWERQUAD 0
#endif

#include "fsl_debug_console.h"
#if USE_POWERQUAD
#include "fsl_powerquad.h"
#include "fsl_powerquad_data.h"
#endif
#include "fsl_common.h"
#include "raw_vectors.h"
#if !USE_POWERQUAD
#include "arm_math.h"
#endif

#include <cmath>
#include <cstdint>
#include <cstring>

namespace
{
constexpr uint32_t kWindowSize = 1024;
constexpr uint32_t kFeatureSize = 513;
constexpr float kMean = 1.2953433990478516f;
constexpr float kStd = 1.0514010190963745f;
constexpr float kInputScale = 0.02456347830593586f;
constexpr int32_t kInputZeroPoint = -78;

#if USE_POWERQUAD
constexpr float kFftInputScale = 2097152.0f;
constexpr float kFftOutputScale = 1.0f / kFftInputScale;
int32_t s_fftInput[kWindowSize] __ALIGNED(16);
int32_t s_fftResult[kWindowSize * 2U] __ALIGNED(16);
float s_debugInput[kWindowSize] __ALIGNED(16);
float s_debugResult[kWindowSize * 2U] __ALIGNED(16);
pq_config_t s_pqConfig;
bool s_initialized = false;
#else
float s_cpuInput[kWindowSize] __ALIGNED(16);
float s_cpuOutput[kWindowSize] __ALIGNED(16);
float s_cpuMagnitude[kFeatureSize] __ALIGNED(16);
arm_rfft_fast_instance_f32 s_cpuRfft;
bool s_cpuInitialized = false;
#endif

#if USE_POWERQUAD
void configure_powerquad(pq_format_t externalFormat, int8_t inputPrescale,
                         int8_t outputPrescale)
{
    s_pqConfig.inputAFormat = externalFormat;
    s_pqConfig.inputBFormat = externalFormat;
    s_pqConfig.outputFormat = externalFormat;
    s_pqConfig.tmpFormat = kPQ_32Bit;
    s_pqConfig.machineFormat = kPQ_32Bit;
    s_pqConfig.inputAPrescale = inputPrescale;
    s_pqConfig.outputPrescale = outputPrescale;
    s_pqConfig.tmpPrescale = 0;
    PQ_SetConfig(POWERQUAD, &s_pqConfig);
}

void initialize_powerquad()
{
    if (s_initialized)
    {
        return;
    }

    PQ_Init(POWERQUAD);
    PQ_GetDefaultConfig(&s_pqConfig);
    configure_powerquad(kPQ_32Bit, 10, 0);
    s_initialized = true;
}
#endif

int scaled_float(float value)
{
    return static_cast<int>(value * 1000.0f);
}

#if USE_POWERQUAD
void print_debug_powerquad_output(uint32_t bin)
{
    uint32_t realWord = reinterpret_cast<uint32_t *>(s_debugResult)[2U * bin];
    uint32_t imagWord = reinterpret_cast<uint32_t *>(s_debugResult)[2U * bin + 1U];
    float real = s_debugResult[2U * bin];
    float imag = s_debugResult[2U * bin + 1U];
    PRINTF("PQ out bin %u: real=0x%08x (%d) imag=0x%08x (%d)\r\n",
           static_cast<unsigned int>(bin), static_cast<unsigned int>(realWord),
           scaled_float(real), static_cast<unsigned int>(imagWord),
           scaled_float(imag));
}
#endif

int32_t quantize_feature(float magnitude)
{
    float feature = log1pf(magnitude);
    float normalized = (feature - kMean) / kStd;
    int32_t quantized = static_cast<int32_t>(
        roundf(normalized / kInputScale) + kInputZeroPoint);
    if (quantized < -128)
    {
        quantized = -128;
    }
    else if (quantized > 127)
    {
        quantized = 127;
    }
    return quantized;
}

#if !USE_POWERQUAD
void initialize_cpu_fft()
{
    if (s_cpuInitialized)
    {
        return;
    }

    if (arm_rfft_fast_init_f32(&s_cpuRfft, kWindowSize) != ARM_MATH_SUCCESS)
    {
        PRINTF("ERROR: arm_rfft_fast_init_f32 failed\r\n");
        while (1)
        {
        }
    }
    s_cpuInitialized = true;
}

void cpu_fft_magnitudes(const float *raw1024, float *magnitudes)
{
    initialize_cpu_fft();
    std::memcpy(s_cpuInput, raw1024, sizeof(s_cpuInput));
    arm_rfft_fast_f32(&s_cpuRfft, s_cpuInput, s_cpuOutput, 0);
    magnitudes[0] = fabsf(s_cpuOutput[0]);
    magnitudes[kWindowSize / 2U] = fabsf(s_cpuOutput[1]);
    for (uint32_t bin = 1U; bin < kWindowSize / 2U; ++bin)
    {
        float real = s_cpuOutput[2U * bin];
        float imag = s_cpuOutput[2U * bin + 1U];
        magnitudes[bin] = sqrtf((real * real) + (imag * imag));
    }
}
#endif
}

extern "C" void preprocess_window(const float *raw1024, int8_t *out513)
{
#if USE_POWERQUAD
    initialize_powerquad();
    for (uint32_t index = 0; index < kWindowSize; ++index)
    {
        s_fftInput[index] = static_cast<int32_t>(
            roundf(raw1024[index] * kFftInputScale));
    }
    PQ_TransformRFFT(POWERQUAD, kWindowSize, s_fftInput, s_fftResult);
    PQ_WaitDone(POWERQUAD);

    for (uint32_t bin = 0; bin < kFeatureSize; ++bin)
    {
        float real = static_cast<float>(s_fftResult[2U * bin]);
        float imag = (bin == 0U || bin == kWindowSize / 2U)
                         ? 0.0f
                         : static_cast<float>(s_fftResult[2U * bin + 1U]);
        real *= kFftOutputScale;
        imag *= kFftOutputScale;
        out513[bin] = static_cast<int8_t>(
            quantize_feature(sqrtf((real * real) + (imag * imag))));
    }
#else
    cpu_fft_magnitudes(raw1024, s_cpuMagnitude);
    for (uint32_t bin = 0; bin < kFeatureSize; ++bin)
    {
        out513[bin] = static_cast<int8_t>(quantize_feature(s_cpuMagnitude[bin]));
    }
#endif
}

extern "C" void preprocess_debug_once(const float *vector0)
{
    static float sineWindow[kWindowSize];

#if USE_POWERQUAD
    initialize_powerquad();
    PRINTF("\r\n=== preprocess_window debug (one-shot) ===\r\n");
    PRINTF("Vector 0 label: Normal (%u)\r\n", 0u);
    PRINTF("raw_windows[0] first4 x1000=[%d,%d,%d,%d]\r\n",
           (int)scaled_float(vector0[0]), (int)scaled_float(vector0[1]),
           (int)scaled_float(vector0[2]), (int)scaled_float(vector0[3]));

    for (uint32_t index = 0; index < kWindowSize; ++index)
    {
        sineWindow[index] =
            1000.0f * sinf(2.0f * 3.14159265358979323846f * 50.0f *
                               static_cast<float>(index) /
                               static_cast<float>(kWindowSize));
    }
    PRINTF("sine window first4 x1000=[%d,%d,%d,%d]\r\n",
           (int)scaled_float(sineWindow[0]), (int)scaled_float(sineWindow[1]),
           (int)scaled_float(sineWindow[2]), (int)scaled_float(sineWindow[3]));

    std::memcpy(s_debugInput, sineWindow, sizeof(s_debugInput));
    configure_powerquad(kPQ_Float, 0, 0);
    PRINTF("PQ format set: inputA=float, inputB=float, output=float, "
           "tmp=32-bit fixed, machine=32-bit fixed, inputA prescale=0, "
           "output prescale=0\r\n");
    PRINTF("PQ input address=%p output address=%p\r\n",
           static_cast<void *>(s_debugInput), static_cast<void *>(s_debugResult));
    PRINTF("PQ input after copy-in: [0x%08x,0x%08x,0x%08x,0x%08x]\r\n",
           reinterpret_cast<uint32_t *>(s_debugInput)[0],
           reinterpret_cast<uint32_t *>(s_debugInput)[1],
           reinterpret_cast<uint32_t *>(s_debugInput)[2],
           reinterpret_cast<uint32_t *>(s_debugInput)[3]);
    PRINTF("PQ call: PQ_TransformRFFT(POWERQUAD, 1024, s_debugInput, "
           "s_debugResult)\r\n");
    PQ_TransformRFFT(POWERQUAD, kWindowSize, s_debugInput, s_debugResult);
    PQ_WaitDone(POWERQUAD);
    PRINTF("PQ_WaitDone called before reading: yes\r\n");

    PRINTF("PQ output first 8 words after PQ_WaitDone:\r\n");
    for (uint32_t word = 0; word < 8U; ++word)
    {
        uint32_t rawWord = reinterpret_cast<uint32_t *>(s_debugResult)[word];
        float decoded = s_debugResult[word];
        PRINTF("  word[%u]=0x%08x decoded_x1000=%d\r\n",
               static_cast<unsigned int>(word),
               static_cast<unsigned int>(rawWord), (int)scaled_float(decoded));
    }
    PRINTF("PQ sine bins (decoded real/imag x1000):\r\n");
    print_debug_powerquad_output(49U);
    print_debug_powerquad_output(50U);
    print_debug_powerquad_output(51U);
    print_debug_powerquad_output(511U);
    print_debug_powerquad_output(512U);

    PRINTF("Firmware bins 0..7: mag/log1p/norm x1000, quantized:\r\n");
    for (uint32_t bin = 0; bin < 8U; ++bin)
    {
        float real = s_debugResult[2U * bin];
        float imag = (bin == 0U || bin == kWindowSize / 2U)
                         ? 0.0f
                         : s_debugResult[2U * bin + 1U];
        float magnitude = sqrtf((real * real) + (imag * imag));
        float logged = log1pf(magnitude);
        float normalized = (logged - kMean) / kStd;
        int32_t quantized = static_cast<int32_t>(
            roundf(normalized / kInputScale) + kInputZeroPoint);
        if (quantized < -128)
        {
            quantized = -128;
        }
        else if (quantized > 127)
        {
            quantized = 127;
        }
        PRINTF("  bin %u: %d/%d/%d/%d\r\n",
               static_cast<unsigned int>(bin), (int)scaled_float(magnitude),
               (int)scaled_float(logged), (int)scaled_float(normalized),
               static_cast<int>(static_cast<int8_t>(quantized)));
    }

    configure_powerquad(kPQ_32Bit, 10, 0);
#else
    float sineMagnitude[kFeatureSize];
    initialize_cpu_fft();
    PRINTF("\r\n=== preprocess_window CPU FFT debug (one-shot) ===\r\n");
    PRINTF("Vector 0 label: Normal (0)\r\n");
    PRINTF("raw_windows[0] first4 x1000=[%d,%d,%d,%d]\r\n",
           (int)scaled_float(vector0[0]), (int)scaled_float(vector0[1]),
           (int)scaled_float(vector0[2]), (int)scaled_float(vector0[3]));
    PRINTF("Normalization scalars: mean_x1000=%d std_x1000=%d\r\n",
           (int)scaled_float(kMean), (int)scaled_float(kStd));
    for (uint32_t index = 0; index < kWindowSize; ++index)
    {
        sineWindow[index] =
            1000.0f * sinf(2.0f * 3.14159265358979323846f * 50.0f *
                               static_cast<float>(index) /
                               static_cast<float>(kWindowSize));
    }
    cpu_fft_magnitudes(sineWindow, sineMagnitude);
    PRINTF("CPU sine mag[50]=%d (expected 512000 +/-1%%)\r\n",
           (int)scaled_float(sineMagnitude[50]));
    PRINTF("CPU sine mags bins 49/51/0/512 x1000=%d/%d/%d/%d\r\n",
           (int)scaled_float(sineMagnitude[49]), (int)scaled_float(sineMagnitude[51]),
           (int)scaled_float(sineMagnitude[0]), (int)scaled_float(sineMagnitude[512]));
    cpu_fft_magnitudes(vector0, s_cpuMagnitude);
    PRINTF("Firmware/Numpy window 0 magnitude bins 0..7 x1000:\r\n");
    for (uint32_t bin = 0; bin < 8U; ++bin)
    {
        PRINTF("  bin %u: %d/%d\r\n", static_cast<unsigned int>(bin),
               (int)scaled_float(s_cpuMagnitude[bin]),
               (int)scaled_float(raw_debug_numpy_window0_mag[bin]));
    }
    PRINTF("Firmware/Numpy sine magnitude bins 49..51 x1000:\r\n");
    for (uint32_t bin = 49U; bin <= 51U; ++bin)
    {
        PRINTF("  bin %u: %d/%d\r\n", static_cast<unsigned int>(bin),
               (int)scaled_float(sineMagnitude[bin]),
               (int)scaled_float(raw_debug_numpy_sine_mag[bin - 49U]));
    }
    PRINTF("Firmware bins 0..7: mag/log1p/norm x1000, quantized:\r\n");
    for (uint32_t bin = 0; bin < 8U; ++bin)
    {
        float logged = log1pf(s_cpuMagnitude[bin]);
        float normalized = (logged - kMean) / kStd;
        PRINTF("  bin %u: %d/%d/%d/%d\r\n",
               static_cast<unsigned int>(bin), (int)scaled_float(s_cpuMagnitude[bin]),
               (int)scaled_float(logged), (int)scaled_float(normalized),
               static_cast<int>(static_cast<int8_t>(
                   quantize_feature(s_cpuMagnitude[bin]))));
    }
#endif
}
