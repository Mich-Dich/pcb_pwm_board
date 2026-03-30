
#include "main.h"               // for I2C_HandleTypeDef and HAL functions
#include "util/data_types.hpp"
#include "PWM/PWM.hpp"
#include "application.hpp"


// FORWARD DECLARATIONS =====================================================================================
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim21;

namespace CTRL {

    // CONSTANTS ============================================================================================

    // MACROS ===============================================================================================

    // TYPES ================================================================================================

    // STATIC VARIABLES =====================================================================================

    static volatile unsigned int test = 0;

    // FUNCTION IMPLEMENTATION ==============================================================================

    // CLASS IMPLEMENTATION =================================================================================

    // CLASS PUBLIC =========================================================================================

    void application_main(void) {

        // 7‑bit address of PCA9635 when all A0..A6 are GND
        const u16 PCA9635_ADDR = (0x00);
        
        PWM::init();

        // // Configure PA3 as output push-pull, no pull-up/down, low speed
        // GPIO_InitTypeDef pin_3_init = {
        //     .Pin = GPIO_PIN_3,
        //     .Mode = GPIO_MODE_OUTPUT_PP,
        //     .Pull = GPIO_NOPULL,
        //     .Speed = GPIO_SPEED_FREQ_LOW,
        // };
        // HAL_GPIO_Init(GPIOA, &pin_3_init);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

        // GPIO_InitTypeDef pin_5_init = {
        //     .Pin = GPIO_PIN_5,
        //     .Mode = GPIO_MODE_OUTPUT_PP,
        //     .Pull = GPIO_NOPULL,
        //     .Speed = GPIO_SPEED_FREQ_LOW,
        // };
        // HAL_GPIO_Init(GPIOA, &pin_5_init);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    
        while (1) {
            
            test++;           // Increment the volatile counter
        }
    }

    // CLASS PROTECTED ======================================================================================

    // CLASS PRIVATE ========================================================================================

}
