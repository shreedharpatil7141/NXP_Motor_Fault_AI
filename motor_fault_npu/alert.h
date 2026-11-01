#ifndef MOTOR_FAULT_ALERT_H
#define MOTOR_FAULT_ALERT_H

#define ALERT_CONF_THRESHOLD 0.70f
#define ALERT_ON_COUNT 3U
#define ALERT_OFF_COUNT 3U

enum alert_state_t
{
    ALERT_STATE_NORMAL = 0,
    ALERT_STATE_FAULT_ALERT,
    ALERT_STATE_LOW_CONFIDENCE
};

void alert_init(void);
alert_state_t alert_update(int predictedClass, float confidence);

#endif
