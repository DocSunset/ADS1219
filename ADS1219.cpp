#include "ADS1219.h"

namespace ADS1219
{
    bool ADC::command(COMMAND cmd) 
    {
        _bus.beginTransmission(_address);
        _bus.write(static_cast<uint8_t>(cmd));
        error = _bus.endTransmission();
        return error != 0;
    }

    bool ADC::read_register(RREG reg, uint8_t& out)
    {
        if (!command(static_cast<COMMAND>(static_cast<uint8_t>(reg))))
        {
            return false;
        }

        if (!_bus.requestFrom(_address, 1, true))
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

    bool ADC::read_raw(int& out)
    {
        if (!command(COMMAND::RDATA)) return false;
        {
            return false;
        }

        if (!_bus.requestFrom(_address, 3, true))
        {
            error = 5;
            return false;
        }

        out = 0;
        int r;
        for (int i = 3; i > 0; --i)
        {
            r = _bus.read();
            if (r == -1) 
            {
                error = 6;
                return false;
            }
            out |= r << (8 * i);
        }
        out = out >> 8;
        return true;
    }

    bool ADC::write_config(Config newconf)
    {
        _bus.beginTransmission(_address);
        _bus.write(static_cast<uint8_t>(COMMAND::WREG));
        _bus.write(newconf._byte);
        error = _bus.endTransmission();
        if (error != 0) return false;
        _config = newconf;
        return true;
    }
}
