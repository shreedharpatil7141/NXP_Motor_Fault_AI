#include "alert.h"

#include "board.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

namespace
{
alert_state_t s_state = ALERT_STATE_NORMAL;
unsigned int s_onCount = 0U;
unsigned int s_offCount = 0U;
int s_latchedFaultClass = 0;

void led_off()
{
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();
}

void set_led(alert_state_t state)
{
    led_off();
    if (state == ALERT_STATE_FAULT_ALERT)
    {
        LED_RED_ON();
    }
    else if (state == ALERT_STATE_LOW_CONFIDENCE)
    {
        LED_BLUE_ON();
    }
    else
    {
        LED_GREEN_ON();
    }
}
}

void alert_init(void)
{
    CLOCK_EnableClock(kCLOCK_Port0);
    CLOCK_EnableClock(kCLOCK_Port1);
    PORT_SetPinMux(PORT0, BOARD_LED_RED_GPIO_PIN, kPORT_MuxAlt0);
    PORT_SetPinMux(PORT0, BOARD_LED_GREEN_GPIO_PIN, kPORT_MuxAlt0);
    PORT_SetPinMux(PORT1, BOARD_LED_BLUE_GPIO_PIN, kPORT_MuxAlt0);

    gpio_pin_config_t config = {kGPIO_DigitalOutput, LOGIC_LED_OFF};
    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &config);
    GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, &config);
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &config);

    s_state = ALERT_STATE_NORMAL;
    s_onCount = 0U;
    s_offCount = 0U;
    s_latchedFaultClass = 0;
    set_led(s_state);
}

alert_state_t alert_update(int predictedClass, float confidence)
{
    const bool isFault = predictedClass != 0;
    const bool isConfidentFault = isFault && confidence >= ALERT_CONF_THRESHOLD;

    if (s_state == ALERT_STATE_FAULT_ALERT)
    {
        if (!isFault)
        {
            s_onCount = 0U;
            if (++s_offCount >= ALERT_OFF_COUNT)
            {
                s_state = ALERT_STATE_NORMAL;
                s_offCount = 0U;
            }
        }
        else
        {
            s_offCount = 0U;
        }
    }
    else if (isConfidentFault)
    {
        s_offCount = 0U;
        if (++s_onCount >= ALERT_ON_COUNT)
        {
            s_state = ALERT_STATE_FAULT_ALERT;
            s_latchedFaultClass = predictedClass;
            s_onCount = 0U;
        }
        else
        {
            s_state = ALERT_STATE_LOW_CONFIDENCE;
        }
    }
    else if (isFault)
    {
        s_onCount = 0U;
        s_offCount = 0U;
        s_state = ALERT_STATE_LOW_CONFIDENCE;
    }
    else
    {
        s_onCount = 0U;
        s_offCount = 0U;
        s_state = ALERT_STATE_NORMAL;
    }

    (void)s_latchedFaultClass;
    set_led(s_state);
    return s_state;
}
