#ifndef CRC16_X_25_H
#define CRC16_X_25_H

#include <stdint.h>

class crc16_x_25 {
    private:
        uint16_t reflect(uint16_t crc, int bitnum);
    public:
        uint16_t calc_checksum(unsigned char *p, uint16_t len);
};

#endif