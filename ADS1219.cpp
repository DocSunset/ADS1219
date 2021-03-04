#include "ADS1219.h"

namespace ADS1219
{
    bool ADS1219_ADC::command(COMMAND cmd, bool send_stop) 
    {
        _bus.beginTransmission(_address);
        _bus.write(static_cast<uint8_t>(cmd));
        error = _bus.endTransmission(send_stop);
        return error == 0;
    }

    bool ADS1219_ADC::read_register(RREG reg, uint8_t& out)
    {
        if (!command(static_cast<COMMAND>(static_cast<uint8_t>(reg)), false)) 
            return false;

        if (_bus.requestFrom(_address, (uint8_t)1, (uint8_t)1) <= 0)
        {
            error = 5;
            return false;
        }

        int r = _bus.read();
        if (r == -1) 
        {
            error = 6;
            return false;
        }
        out = r;
        return true;
    }

    bool ADS1219_ADC::read_raw(int& out)
    {
        if (!command(COMMAND::RDATA, false)) return false;

        if (!_bus.requestFrom(_address, (uint8_t)3, (uint8_t)1))
        {
            error = 5;
            return false;
        }

        out = 0;
        int r;
        for (int i = 0; i < 3; ++i)
        {
            r = _bus.read();
            if (r == -1) 
            {
                error = 6;
                return false;
            }
            out |= r << (8 * (2-i));
        }
        if (out > MAX_CODE) out |= 0xff000000;
        return true;
    }

    bool ADS1219_ADC::write_config(Config newconf)
    {
        _bus.beginTransmission(_address);
        _bus.write(static_cast<uint8_t>(COMMAND::WREG));
        _bus.write(newconf._byte);
        error = _bus.endTransmission(true); // you should send stop
        if (error != 0) return false;
        _config = newconf;
        return true;
    }
}
