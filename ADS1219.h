#pragma once
#include <cinttypes>
#include <Wire.h>
#include "datasheet.h"

namespace ADS1219 {
class ADC
{
public:
    // The ADC must be initialized with an address and an I2C bus
    ADC(const Address& address, TwoWire& wire) 
        : _address{address._byte}, _bus{wire}, _config{0} {}

    // Send a command to the device. Normally this is not used directly.
    bool command(COMMAND cmd);

    // Send the reset command to the device; 
    // equivalent to toggling the device's reset pin or power cycling the device.
    bool reset()            { return command(RESET); }

    // Send the STARTSYNC command to the device.
    // This will a single shot conversion or start continuous conversion, 
    // depending on the current conversion mode configuration. 
    // Note that any ongoing conversion will be restarted.
    bool start_conversion() { return command(STARTSYNC); }

    // Send the STARTSYNC command to the device.
    // It is recommended to use this function, equivalent to start_conversion(),
    // when your intention is explicitly to restart any ongoing conversions.
    bool sync_conversion()  { return command(STARTSYNC); }

    // Send the POWERDOWN command to the device.
    // The device shuts down all analog circuitry, reducing power consumption.
    // The device retains its configuration and responds to commands, 
    // but does not perform any conversions.  Send STARTSYNC to switch back to
    // normal operation and begin a conversion.
    bool powerdown()        { return command(POWERDOWN); }

    // Read one of the registers. This command is normally not used directly.
    // Returns true if the read was successful.
    bool read_register(RREG reg, uint8_t& out);

    // Read the config register from the device.
    // Returns true if the read was successful.
    bool read_config(uint8_t& out) { return read_register(CONFIG, out); }

    // Read the status register from the device to check if new data is ready.
    // Returns true if data is ready. This may return false due to an error
    // on the I2C bus even if data is ready.
    bool data_ready() 
    { 
        uint8_t r = 0;
        if (!read_register(STATUS, r)) return false;
        else return r > 0;
    }

    // Interpret a pin reading as the DRDY pin; returns true if new data is
    // ready. The user is responsible for ensuring the pin reading actually
    // corresponds with the DRDY pin for this device.
    bool data_ready(const int& drdy_pin_reading) { return drdy_pin_reading == 0; }

    // Latch and read the most recent conversion data from the device.
    // Returns true if the read was successful.
    bool read_raw(int& out);

    // Normalize a raw reading to the interval [-1 1] floating point
    float normalize(const int& raw) const {return (float)raw / (float)(0x7fffff);}

    // Calls read_raw() and normalizes the result to a float in the interval [-1, 1]
    // Returns true if the read was successful.
    bool read_normalized(float& out) 
    { 
        int r;
        if (!read_raw(r)) return false;
        out = normalize(r);
        return true;
    }

    // Send the current configuration stored in memory to the device.
    // This is normally called automatically when using set_config or modify_config
    bool write_config(Config newconf);

    // Apply fields to the default configuration and send to device.
    // Note that if multiple values are given for one field, 
    // only the first will be applied.
    template<typename... Fields>
    bool set_config(Fields... fields) 
    { 
        return write_config(configuration(fields...)); 
    }

    // Apply fields to the current configuration and send to device.
    // Note that if multiple values are given for one field, 
    // only the first will be applied
    template<typename... Fields>
    void modify_config(Fields... fields) 
    { 
        return write_config(apply(_config, fields...));
    }

    // The I2C address to which communications will be addressed.
    const uint8_t _address;

    // The I2C bus on which communications will take place
    TwoWire& _bus;

    // The configuration most recently written to or read from the device
    Config _config;

    // The most recently received error code. Check in case a command fails.
    uint8_t error;

}; // ADC
} // namespace ADS1219
