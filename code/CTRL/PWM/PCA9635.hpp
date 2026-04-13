#pragma once


#include "stm32l0xx_hal.h"
#include "util/data_types.hpp"
#include "util/macros.hpp"



#define PCA9635_LIB_VERSION         ("0.6.2-stm32")

//  mode codes
#define PCA963X_MODE1               0x00
#define PCA963X_MODE2               0x01

//  0x80 bit ==> Auto-Increment for all registers.
//               used in writeN()
#define PCA963X_PWM(x)              (0x82+(x))
#define PCA963X_GRPPWM              0x12
#define PCA963X_GRPFREQ             0x13

//  check datasheet for details
#define PCA963X_LEDOUT_BASE         0x14  //  0x14..0x17
#define PCA963X_LEDOFF              0x00  //  default @ startup
#define PCA963X_LEDON               0x01
#define PCA963X_LEDPWM              0x02
#define PCA963X_LEDGRPPWM           0x03

//  Error codes
#define PCA963X_OK                  0x00
#define PCA963X_ERROR               0xFF
#define PCA963X_ERR_WRITE           0xFE
#define PCA963X_ERR_CHAN            0xFD
#define PCA963X_ERR_MODE            0xFC
#define PCA963X_ERR_REG             0xFB
#define PCA963X_ERR_I2C             0xFA

//  Configuration bits MODE1 register
#define PCA963X_MODE1_AUTOINCR2     0x80  //  ReadOnly,  0 = disable  1 = enable
#define PCA963X_MODE1_AUTOINCR1     0x40  //  ReadOnly,  bit1
#define PCA963X_MODE1_AUTOINCR0     0x20  //  ReadOnly,  bit0
#define PCA963X_MODE1_SLEEP         0x10  //  0 = normal       1 = sleep
#define PCA963X_MODE1_SUB1          0x08  //  0 = disable      1 = enable
#define PCA963X_MODE1_SUB2          0x04  //  0 = disable      1 = enable
#define PCA963X_MODE1_SUB3          0x02  //  0 = disable      1 = enable
#define PCA963X_MODE1_ALLCALL       0x01  //  0 = disable      1 = enable
#define PCA963X_MODE1_NONE          0x00

//  Configuration bits MODE2 register
#define PCA963X_MODE2_BLINK         0x20  //  0 = dim          1 = blink
#define PCA963X_MODE2_INVERT        0x10  //  0 = normal       1 = inverted
#define PCA963X_MODE2_ACK           0x08  //  0 = on STOP      1 = on ACK
#define PCA963X_MODE2_TOTEMPOLE     0x04  //  0 = open drain   1 = totem-pole
#define PCA963X_MODE2_NONE          0x00

//  Registers in which the ALLCALL and sub-addresses are stored
#define PCA963X_SUBADR(x)           (0x17 +(x))  //  x = 1..3
#define PCA963X_ALLCALLADR          0x1B

//  Standard ALLCALL and sub-addresses
#define PCA963X_ALLCALL             0x70
#define PCA963X_SUB1                0x71
#define PCA963X_SUB2                0x72
#define PCA963X_SUB3                0x74

namespace CTRL {

    class PCA9635
    {
    public:
    
      // Constructor now takes an I2C_HandleTypeDef pointer and device address
      explicit PCA9635(const u8 deviceAddress, I2C_HandleTypeDef *hi2c);
    
      bool begin(u8 mode1_mask = PCA963X_MODE1_ALLCALL, u8 mode2_mask = PCA963X_MODE2_NONE);
    
      // CONFIGURATION
      u8  configure(u8 mode1_mask, u8 mode2_mask);

      DEFAULT_GETTER(u8, channel_count)
      DEFAULT_GETTER(u8, address)
    
      // LED DRIVER MODE
      u8 set_led_driver_mode_all(u8 mode);
      u8 set_led_driver_mode(u8 channel, u8 mode);
      // u8 get_led_driver_mode(u8 channel);
    
      // WRITE MODE REGISTERS
      u8 set_mode1(u8 value);
      u8 set_mode2(u8 value);
      u8 get_mode1();
      u8 get_mode2();
    
      // GROUP REGISTERS
      u8 set_group_pwm(u8 value);
      u8 get_group_pwm();
      u8 set_group_freq(u8 value);
      u8 get_group_freq();
    
      // WRITE PWM
      u8 write1(u8 channel, u8 value);
      u8 write3(u8 channel, u8 R, u8 G, u8 B);
      u8 write_n(u8 channel, u8 * arr, u8 count);
      u8 write_all(u8 * arr);
      u8 all_off();
    
      // (no-stop variants – for HAL they behave the same as normal writes)
      u8 write_n_no_stop(u8 channel, u8 * arr, u8 count);
      u8 write_stop();
    
      // SUB CALL
      bool enable_sub_call(u8 nr);
      bool disable_sub_call(u8 nr);
      bool is_enabled_sub_call(u8 nr);
      bool set_sub_call_address(u8 nr, u8 address);
      u8 get_sub_call_address(u8 nr);
    
      // ALL CALL
      bool enable_all_call();
      bool disable_all_call();
      bool is_enabled_all_call();
      bool set_all_call_address(u8 address);
      u8 get_all_call_address();
    
      // OE - Output Enable control (STM32 version: specify GPIO port and pin)
      bool set_output_enable_pin(GPIO_TypeDef *port, u16 pin);
      bool set_output_enable(bool on);
      u8 get_output_enable();
    
      // EXPERIMENTAL
      int i2_c_software_reset(u8 method);
      u8 write_led_out(u8 reg, u8 mask);
      u8 read_led_out(u8 reg);
    
      // ERROR
      int last_error();
    
    private:
    
      u8 write_register(u8 reg, u8 value);
      u8 read_register(u8 reg);
    
      u8                    m_address;
      I2C_HandleTypeDef*    m_i2c_handle;
      int                   m_error = PCA963X_OK;
      u8                    m_channel_count = 16;
      GPIO_TypeDef*         m_oe_port;     // GPIO port for OE pin
      u16                   m_oe_pin;       // GPIO pin for OE
      bool                  m_oe_valid;     // true if OE pin configured
    };

}
