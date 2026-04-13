
#include "PCA9635.hpp"

namespace CTRL {

    // Constructor
    PCA9635::PCA9635(const u8 deviceAddress, I2C_HandleTypeDef *hi2c) {

        m_address       = deviceAddress;
        m_i2c_handle    = hi2c;
        m_channel_count = 16;
        m_error         = PCA963X_OK;
        m_oe_port       = nullptr;
        m_oe_pin        = 0;
        m_oe_valid      = false;
    }


    bool PCA9635::begin(u8 mode1_mask, u8 mode2_mask) {

        if (HAL_I2C_IsDeviceReady(m_i2c_handle, m_address, 3, 10) != HAL_OK)    // Device reachable?
            return false;
            
        configure(mode1_mask, mode2_mask);
        return true;
    }


    u8 PCA9635::configure(u8 mode1_mask, u8 mode2_mask) {
        m_error = PCA963X_OK;
        u8 r1 = set_mode1(mode1_mask);
        u8 r2 = set_mode2(mode2_mask);
        if ((r1 != PCA963X_OK) || (r2 != PCA963X_OK))
            return PCA963X_ERROR;
        return m_error;
    }

    
    // LED DRIVER MODE
    u8 PCA9635::set_led_driver_mode_all(u8 mode) {

        if (mode > 3) return PCA963X_ERR_MODE;
        u8 mask = 0;
        switch(mode)
        {
            case PCA963X_LEDGRPPWM: mask = 0b11111111; break;
            case PCA963X_LEDPWM:    mask = 0b10101010; break;
            case PCA963X_LEDON:     mask = 0b01010101; break;
            default:                mask = 0b00000000; break;
        }
        for (int reg = 0; reg < 4; reg++)
            write_led_out(reg, mask);
        return PCA963X_OK;
    }

    u8 PCA9635::set_led_driver_mode(u8 channel, u8 mode) {

        if (channel >= m_channel_count)
        {
            m_error = PCA963X_ERR_CHAN;
            return m_error;
        }
        if (mode > 3)
        {
            m_error = PCA963X_ERR_MODE;
            return m_error;
        }

        u8 reg = PCA963X_LEDOUT_BASE + (channel >> 2);
        u8 shift = (channel & 0x03) * 2;
        u8 setmask = mode << shift;
        u8 clrmask = ~(0x03 << shift);
        u8 value = (read_register(reg) & clrmask) | setmask;
        write_register(reg, value);
        m_error = PCA963X_OK;
        return m_error;
    }

    // u8 PCA9635::get_led_driver_mode(u8 channel) {

    //     if (channel >= m_channel_count)
    //     {
    //         m_error = PCA963X_ERR_CHAN;
    //         return m_error;
    //     }
    //     u8 reg = PCA963X_LEDOUT_BASE + (channel >> 2);
    //     u8 shift = (channel & 0x03) * 2;
    //     u8 value = (read_register(reg) >> shift) & 0x03;
    //     m_error = PCA963X_OK;
    //     return value;
    // }

    
    // MODE REGISTERS
    u8 PCA9635::set_mode1(u8 value) { return write_register(PCA963X_MODE1, value); }


    u8 PCA9635::set_mode2(u8 value) { return write_register(PCA963X_MODE2, value); }


    u8 PCA9635::get_mode1() { return read_register(PCA963X_MODE1); }


    u8 PCA9635::get_mode2() { return read_register(PCA963X_MODE2); }

    
    // GROUP REGISTERS
    u8 PCA9635::set_group_pwm(u8 value) { return write_register(PCA963X_GRPPWM, value); }


    u8 PCA9635::get_group_pwm() { return read_register(PCA963X_GRPPWM); }


    u8 PCA9635::set_group_freq(u8 value) { return write_register(PCA963X_GRPFREQ, value); }


    u8 PCA9635::get_group_freq() { return read_register(PCA963X_GRPFREQ); }

    // WRITE PWM
    u8 PCA9635::write1(u8 channel, u8 value) { return write_n(channel, &value, 1); }


    u8 PCA9635::write3(u8 channel, u8 R, u8 G, u8 B) {

        u8 arr[3] = { R, G, B };
        return write_n(channel, arr, 3);
    }

    
    u8 PCA9635::write_n(u8 channel, u8* arr, u8 count) {

        if (channel + count > m_channel_count)
        {
            m_error = PCA963X_ERR_CHAN;
            return PCA963X_ERROR;
        }
        u8 base = PCA963X_PWM(channel); // includes auto-increment bit (0x80)
        // Write multiple bytes starting at 'base' register
        HAL_StatusTypeDef status = HAL_I2C_Mem_Write(m_i2c_handle, m_address, base,
                                                    I2C_MEMADD_SIZE_8BIT, arr, count, 100);
        if (status != HAL_OK)
        {
            m_error = PCA963X_ERR_I2C;
            return PCA963X_ERROR;
        }
        m_error = PCA963X_OK;
        return m_error;
    }


    u8 PCA9635::write_all(u8* arr) { return write_n(0, arr, 16); }


    u8 PCA9635::all_off() {

        u8 arr[16] = {0};
        return write_n(0, arr, 16);
    }


    // In HAL we cannot easily suppress the STOP, so just do a normal write.
    // The STOP will be generated, but that does not harm most use cases.
    u8 PCA9635::write_n_no_stop(u8 channel, u8* arr, u8 count) { return write_n(channel, arr, count); }


    // No action needed – each HAL transaction already sends STOP.
    u8 PCA9635::write_stop() { return PCA963X_OK; }


    // SUB CALL
    bool PCA9635::enable_sub_call(u8 nr) {
            
        if (nr == 0 || nr > 3) return false;
        u8 prev = get_mode1();
        u8 mask = prev;
        if (nr == 1)      mask |= PCA963X_MODE1_SUB1;
        else if (nr == 2) mask |= PCA963X_MODE1_SUB2;
        else              mask |= PCA963X_MODE1_SUB3;
        if (mask != prev)
        {
            set_mode1(mask);
        }
        return true;
    }


    bool PCA9635::disable_sub_call(u8 nr) {

        if (nr == 0 || nr > 3) return false;
        u8 prev = get_mode1();
        u8 mask = prev;
        if (nr == 1)      mask &= ~PCA963X_MODE1_SUB1;
        else if (nr == 2) mask &= ~PCA963X_MODE1_SUB2;
        else              mask &= ~PCA963X_MODE1_SUB3;
        if (mask != prev)
        {
            set_mode1(mask);
        }
        return true;
    }

    
    bool PCA9635::is_enabled_sub_call(u8 nr) {
            
        if (nr == 0 || nr > 3) return false;
        u8 mask = get_mode1();
        if (nr == 1) return (mask & PCA963X_MODE1_SUB1) != 0;
        if (nr == 2) return (mask & PCA963X_MODE1_SUB2) != 0;
        return (mask & PCA963X_MODE1_SUB3) != 0;
    }


    bool PCA9635::set_sub_call_address(u8 nr, u8 address) {
            
        if (nr == 0 || nr > 3) return false;
        write_register(PCA963X_SUBADR(nr), address);
        return true;
    }

    
    u8 PCA9635::get_sub_call_address(u8 nr) {
            
        if (nr == 0 || nr > 3) return 0;
        return read_register(PCA963X_SUBADR(nr));
    }

    
    // ALL CALL
    bool PCA9635::enable_all_call() {
            
        u8 prev = get_mode1();
        u8 mask = prev | PCA963X_MODE1_ALLCALL;
        if (mask != prev)
            set_mode1(mask);
        return true;
    }


    bool PCA9635::disable_all_call() {
            
        u8 prev = get_mode1();
        u8 mask = prev & ~PCA963X_MODE1_ALLCALL;
        if (mask != prev)
            set_mode1(mask);
        return true;
    }

    
    bool PCA9635::is_enabled_all_call() {
            
        u8 mask = get_mode1();
        return (mask & PCA963X_MODE1_ALLCALL) != 0;
    }


    bool PCA9635::set_all_call_address(u8 address) {
            
        write_register(PCA963X_ALLCALLADR, address);
        return true;
    }


    u8 PCA9635::get_all_call_address() { return read_register(PCA963X_ALLCALLADR); }


    // OE control (STM32)
    bool PCA9635::set_output_enable_pin(GPIO_TypeDef* port, uint16_t pin) {

        m_oe_port = port;
        m_oe_pin = pin;
        m_oe_valid = true;
        // Configure as output push-pull, low speed, no pull
        GPIO_InitTypeDef gpio_init = {
            .Pin = pin,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
        };
        HAL_GPIO_Init(port, &gpio_init);
        // OE active low: default disable (HIGH)
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        return true;
    }


    bool PCA9635::set_output_enable(bool on) {

        if (!m_oe_valid) return false;
        // OE active low: true -> LOW (output enabled)
        HAL_GPIO_WritePin(m_oe_port, m_oe_pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
        return true;
    }


    u8 PCA9635::get_output_enable() {
            
        if (!m_oe_valid) return 1; // assume disabled
        GPIO_PinState state = HAL_GPIO_ReadPin(m_oe_port, m_oe_pin);
        // active low: LOW means enabled (return 1), HIGH means disabled (return 0)
        return (state == GPIO_PIN_RESET) ? 1 : 0;
    }


    // EXPERIMENTAL
    int PCA9635::i2_c_software_reset(u8 method) {
            
        if (method > 1) return -999;
        if (method == 1)
        {
            const u8 SW_RESET = 0x03;
            u8 cmd[2] = {0xA5, 0x5A};
            return HAL_I2C_Master_Transmit(m_i2c_handle, SW_RESET, cmd, 2, 100);
        }
        // default method 0
        u8 cmd = 0x06;
        return HAL_I2C_Master_Transmit(m_i2c_handle, 0x00, &cmd, 1, 100);
    }


    u8 PCA9635::write_led_out(u8 reg, u8 mask) {
            
        if (reg > 3) return PCA963X_ERROR;
        write_register(PCA963X_LEDOUT_BASE + reg, mask);
        return PCA963X_OK;
    }


    u8 PCA9635::read_led_out(u8 reg) {
            
        if (reg > 3) return 0x00;
        return read_register(PCA963X_LEDOUT_BASE + reg);
    }

    
    // ERROR
    int PCA9635::last_error() {
            
        int e = m_error;
        m_error = PCA963X_OK;
        return e;
    }

    
    // OBSOLETE
    // u8 PCA9635::set_led_driver_mode(u8 mode) { return set_led_driver_mode_all(mode); }


    // PRIVATE METHODS
    u8 PCA9635::write_register(u8 reg, u8 value) {

        HAL_StatusTypeDef status = HAL_I2C_Mem_Write(m_i2c_handle, m_address, reg,
                                                    I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
        if (status != HAL_OK)
        {
            m_error = PCA963X_ERR_I2C;
            return PCA963X_ERROR;
        }
        m_error = PCA963X_OK;
        return m_error;
    }


    u8 PCA9635::read_register(u8 reg) {

        u8 value = 0;
        HAL_StatusTypeDef status = HAL_I2C_Mem_Read(m_i2c_handle, m_address, reg,
                                                    I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
        if (status != HAL_OK)
        {
            m_error = PCA963X_ERR_I2C;
            return 0;
        }
        m_error = PCA963X_OK;
        return value;
    }

}
