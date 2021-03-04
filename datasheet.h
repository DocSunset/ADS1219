#pragma once
#include <inttypes.h>

namespace ADS1219 {
    // Table 10. I2C Address Truth Table
    enum class ADDR1 : uint8_t
    {
        DGND = 0b0000,
        DVDD = 0b0100,
        SDA  = 0b1000,
        SCL  = 0b0100
    };
    enum class ADDR0 : uint8_t
    {
        DGND = 0b0000,
        DVDD = 0b0001,
        SDA  = 0b0010,
        SCL  = 0b0001
    };

    // Helper class for setting address
    struct Address
    {
        static constexpr uint8_t init(ADDR1 a1, ADDR0 a0)
        {
            return static_cast<uint8_t> 
                  (      0b1001111 & 
                       ( 0b1000000 
                       | static_cast<uint8_t>(a1)
                       | static_cast<uint8_t>(a0)
                       )
                  );
        }
        constexpr Address(ADDR0 a0, ADDR1 a1) : _byte {init(a1, a0)} {}
        constexpr Address(ADDR1 a1, ADDR0 a0) : _byte {init(a1, a0)} {}
        const uint8_t _byte;
    };

    // Table 7. Command Definitions
    enum COMMAND : uint8_t
    {
        RESET       = 0b00000110,
        STARTSYNC   = 0b00001000,
        POWERDOWN   = 0b00000010,
        RDATA       = 0b00010000,
        WREG        = 0b01000000
    };
    enum RREG : uint8_t
    {
        CONFIG      = 0b00100000,
        STATUS      = 0b00100100,
    };

    // Table 10. Configuration Register Field Descriptions
    // On boot or reset, the config register is set to all zeros
    enum class MUX : uint8_t
    {
        AIN0_AIN1   = 0,
        AIN2_AIN3   = 0b00100000,
        AIN1_AIN2   = 0b01000000,
        AIN0        = 0b01100000,
        AIN1        = 0b10000000,
        AIN2        = 0b10100000,
        AIN3        = 0b11000000,
        HALF_AVDD   = 0b11100000,
        MASK        = 0b00011111
    };
    enum class GAIN : uint8_t
    {
        ONE         = 0,
        FOUR        = 0b00010000,
        MASK        = 0b11101111
    };
    enum class RATE : uint8_t
    {
        SPS20       = 0,
        SPS90       = 0b00000100,
        SPS330      = 0b00001000,
        SPS1000     = 0b00001100,
        MASK        = 0b11110011
    };
    enum class MODE : uint8_t
    {
        SINGLE_SHOT = 0,
        CONTINUOUS  = 0b00000010,
        MASK        = 0b11111101
    };
    enum class VREF : uint8_t
    {
        INTERN      = 0,
        EXTERN      = 0b00000001,
        MASK        = 0b11111110
    };

    // Helper class for setting config
    struct Config 
    { 
        // It is recommended to use configuration() to construct a
        // Config rather than directly giving it a byte.
        uint8_t _byte; 
    };

    // Base case for apply when all fields run out
    constexpr Config apply(Config c) {return c;}

    // Safely apply any number of fields to the given Config.  Note that if
    // multiple values are given for one field, only the first will be applied
    template<typename Field, typename... Fields>
    constexpr Config apply(Config c, Field f, Fields... fields)
    {
        return Config{ static_cast<uint8_t>(
                  (static_cast<uint8_t>(f) & ~static_cast<uint8_t>(Field::MASK)) 
                | (static_cast<uint8_t>(Field::MASK) & apply(c, fields...)._byte))};
    }

    template<typename... Fields> 
    constexpr Config configuration(Fields... fields)
    {
        return apply(Config{}, fields...);
    };

    constexpr int MAX_CODE = 0x7fffff;

}
