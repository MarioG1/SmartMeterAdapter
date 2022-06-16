#ifndef E450Dataset_H
#define E450Dataset_H

#include "Configuration.h"

class E450Dataset
{
private:
    Configuration config;
    uint8_t encKey[16];
    uint8_t rawData[120];
    char* decryptionKey[16];
    uint32_t ei_last;
    uint32_t ee_last;
    void getIv(uint8_t iv[12]);
    void getAad(uint8_t aad[17]);
    void getTag(uint8_t tag[12]);
    //Debug
    void sendBytes(uint8_t* bytes, unsigned int count);
public:
    E450Dataset(uint8_t encKey[16]);
    bool decrypt(uint8_t rawData[], uint16_t length);
    uint8_t decryptedData[120];
    uint8_t dataLength;

    void getSerial(char serial[8]);
    String getDataString();
};

#endif