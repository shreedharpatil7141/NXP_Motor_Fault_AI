#ifndef MOTOR_FAULT_PREPROCESS_H
#define MOTOR_FAULT_PREPROCESS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void preprocess_window(const float *raw1024, int8_t *out513);
void preprocess_debug_once(const float *vector0);

#ifdef __cplusplus
}
#endif

#endif
