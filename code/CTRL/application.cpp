
#include "main.h"               // for I2C_HandleTypeDef and HAL functions
#include "util/data_types.hpp"
#include "PWM/PWM.hpp"
#include "PWM/PCA9635.hpp"
#include "application.hpp"


// FORWARD DECLARATIONS =====================================================================================
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim21;
extern I2C_HandleTypeDef hi2c1;   // defined in main.c

namespace CTRL {

    // CONSTANTS ============================================================================================

    // MACROS ===============================================================================================

    // TYPES ================================================================================================

    // STATIC VARIABLES =====================================================================================

    static volatile unsigned int test = 0;
    PCA9635 pwmDriver(0x07, &hi2c1);

    // FUNCTION IMPLEMENTATION ==============================================================================

    void setup() {

        for (size_t x = 0; x < 255; x++)
        {
            const auto result = HAL_I2C_IsDeviceReady(&hi2c1, x, 3, 100);
            if (result == HAL_OK)
                test += 1;
        }

        pwmDriver.begin();
        pwmDriver.set_output_enable_pin(GPIOA, GPIO_PIN_7);
        pwmDriver.set_output_enable(true);                             // enable outputs

        pwmDriver.all_off();
        // pwmDriver.set_led_driver_mode(0, PCA963X_LEDPWM);   // Set channel 0 to individual PWM mode
        // pwmDriver.write1(0, 128);                          // Set PWM duty cycle for channel 0 to 128 (50%)
    }

    // CLASS IMPLEMENTATION =================================================================================

    // CLASS PUBLIC =========================================================================================

    void application_main(void) {

        // 7‑bit address of PCA9635 when all A0..A6 are GND
        // const u16 PCA9635_ADDR = (0x00);
        
        setup();
        // PWM::init();

        // Configure PA3 as output push-pull, no pull-up/down, low speed
        GPIO_InitTypeDef pin_3_init = {
            .Pin = GPIO_PIN_3,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
        };
        HAL_GPIO_Init(GPIOA, &pin_3_init);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

        // __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

        // GPIO_InitTypeDef pin_5_init = {
        //     .Pin = GPIO_PIN_5,
        //     .Mode = GPIO_MODE_OUTPUT_PP,
        //     .Pull = GPIO_NOPULL,
        //     .Speed = GPIO_SPEED_FREQ_LOW,
        // };
        // HAL_GPIO_Init(GPIOA, &pin_5_init);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    
        while (1) {
            
            test += 1;
        }
    }

    // CLASS PROTECTED ======================================================================================

    // CLASS PRIVATE ========================================================================================

}
