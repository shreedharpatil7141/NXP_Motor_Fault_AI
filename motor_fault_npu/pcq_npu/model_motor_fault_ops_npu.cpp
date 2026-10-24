/*
 * Motor Fault AI NPU operator registration
 *
 * Generated model:
 * 4class_fault_classifier_npu
 */

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/neutron/neutron.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver()
{
    static tflite::MicroMutableOpResolver<2> s_microOpResolver;

    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddCustom(
        tflite::GetString_NEUTRON_GRAPH(),
        tflite::Register_NEUTRON_GRAPH());

    return s_microOpResolver;
}
