
#include "main.h"               // for I2C_HandleTypeDef and HAL functions
#include "PWM.hpp"
#include "util/data_types.hpp"
#include "util/math.hpp"
#include "PWM.hpp"


// FORWARD DECLARATIONS =====================================================================================

extern I2C_HandleTypeDef hi2c1;         // Declare the I2C handle that is defined in main.c

namespace CTRL::PWM {

    // CONSTANTS ============================================================================================

    constexpr u16 PCA9635_ADDR  = 0x00;           // 7‑bit address, replace with actual address from hardware pins
    constexpr u8  REG_MODE1     = 0x00;
    constexpr u8  REG_MODE2     = 0x01;
    constexpr u8  REG_PWM0      = 0x02;           // First PWM register (LED0)
    constexpr u8  REG_LEDOUT0   = 0x14;           // Output state for LEDs 0‑3
    constexpr u8  REG_LEDOUT1   = 0x15;           // LEDs 4‑7
    constexpr u8  REG_LEDOUT2   = 0x16;           // LEDs 8‑11

    // MACROS ===============================================================================================

    // TYPES ================================================================================================

    // enable the PWM-IC via the PA7(13) pin
    void enable_pwm_ic(const bool enable = true);

    bool write_reg(u8 reg, u8 value);

    u8 angle_to_pwm(i16 angle);

    void set_servo_target(u8 servo_index, i16 angle);
    
    bool set_output_mode(const u8 channel, u8 mode);

    bool set_pwm_duty(const u8 channel, u8 duty);

    // STATIC VARIABLES =====================================================================================
    
    static volatile u32 debug_counter = 0;   // add at file scope

    // FUNCTION IMPLEMENTATION ==============================================================================

    bool init() {

        if (!HAL_I2C_IsDeviceReady(&hi2c1, PCA9635_ADDR, 3, 100) == HAL_OK)     // enure PWM-IC is there
            return false;
        
        enable_pwm_ic();

        // Wake up oscillator: clear SLEEP bit in MODE1 (address 0x00) Default MODE1 = 0x11 (SLEEP=1, ALLCALL=1). Write 0x01 to clear SLEEP.
        if (!write_reg(0x00, 0x01))
            return false;

        // Set MODE2 to totem-pole, no inversion, outputs change on STOP Default MODE2 = 0x04 (OUTDRV=1, OUTNE=01). Write 0x00 for simpler behaviour.
        if (!write_reg(0x01, 0x00))
            return false;

        // volatile u8 servo_pwm = angle_to_pwm(0);

        for (u8 i = 0; i < 15; i++) {

            set_output_mode(10, 2);
            set_pwm_duty(10, 2);
        }
                
        return true;
    }


    // Map angle (-180..+180) to an 8‑bit PWM value.
    FORCE_INLINE u8 angle_to_pwm(i16 angle) {

        if (angle > 180)    angle = 180;
        if (angle < -180)   angle = -180;
        return static_cast<u8>( (angle + 180) * 0.71111111111f );
    }


    // Set the PWM output for a specific servo (0‑9) based on angle
    void set_servo_target(u8 servo_index, i16 angle) {

        if (servo_index >= 10)      // only first 10 outputs used for servos
            return;

        u8 pwm_val = angle_to_pwm(angle);
        u8 reg = REG_PWM0 + servo_index;         // PWM0 at 0x02, PWM1 at 0x03, …
        write_reg(reg, pwm_val);
    }

    // PRIVATE FUNCTION IMPLEMENTATION ======================================================================

    void enable_pwm_ic(const bool enable) {

        // Enable GPIOA clock (if not already enabled in main.c)
        __HAL_RCC_GPIOA_CLK_ENABLE();

        // Configure PA7 as output push-pull, no pull-up/pull-down, low speed
        GPIO_InitTypeDef gpio_init = {
            .Pin = GPIO_PIN_7,
            .Mode = GPIO_MODE_OUTPUT_PP,      // Push-pull output
            .Pull = GPIO_NOPULL,              // No internal resistor
            .Speed = GPIO_SPEED_FREQ_LOW,     // Low speed is enough
        };
        HAL_GPIO_Init(GPIOA, &gpio_init);

        // OE active low -> true: GPIO_PIN_RESET
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (enable ? GPIO_PinState::GPIO_PIN_RESET : GPIO_PinState::GPIO_PIN_SET));
    }
    

    bool read_reg(u8 reg, u8* value) {
        
        return HAL_I2C_Mem_Read(&hi2c1, PCA9635_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100) == HAL_OK;
    }


    // Helper function to write a single byte to a register
    bool write_reg(u8 reg, u8 value) {

        return HAL_I2C_Mem_Write(&hi2c1, PCA9635_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100) == HAL_OK;
    }
    
    // CLASS IMPLEMENTATION =================================================================================

    // CLASS PUBLIC =========================================================================================

    // CLASS PROTECTED ======================================================================================

    // CLASS PRIVATE ========================================================================================

    // Set output mode for a single channel
    // mode: 0 = off, 1 = on, 2 = individual PWM, 3 = individual + group PWM
    bool set_output_mode(const u8 channel, u8 mode) {

        if (channel >= 16) 
            return false;

        if (mode > 3) 
            return false;

        const u8 reg_addr = REG_LEDOUT0 + (channel / 4);   // LEDOUT0..LEDOUT3
        const u8 shift = (channel % 4) * 2;
        const u8 mask = 0x03 << shift;

        // Read current LEDOUT register
        u8 reg_val{};
        if (!read_reg(reg_addr, &reg_val)) 
            return false;

        // Modify the two bits for this channel
        reg_val = (reg_val & ~mask) | (mode << shift);

        return write_reg(reg_addr, reg_val);
    }


    // Set PWM duty cycle (0..255)
    bool set_pwm_duty(const u8 channel, u8 duty) {

        if (channel >= 16) 
            return false;

        u8 reg = REG_PWM0 + channel;          // PWM0 at 0x02, PWM1 at 0x03, ...
        return write_reg(reg, duty);
    }

}
