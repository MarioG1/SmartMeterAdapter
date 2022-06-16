//#define LOGGING;

#include <stdio.h>
#include <string.h>
#include <AES.h>
#include <GCM.h>
#include "Arduino.h"

#include "E450Dataset.h"
#include "Configuration.h"

/**
 * Class to decrypt and given dataset send out by the SmartMeter
 * @param ENC_KEY Key used to decrypt the data
 **/
E450Dataset::E450Dataset(uint8_t ENC_KEY[16]) {
    Configuration config;
    memcpy(&this->encKey[0], &ENC_KEY[0], 16);

    this->ee_last = 0;
    this->ei_last = 0;
}

/**
 * Decrypts an dataset received from smartmeter
 * @param inputData Data to decrypt
 * @param length Length of input data in bytes. Just for logging/debugging
 * @return true on success
 **/
bool E450Dataset::decrypt(uint8_t inputData[120], uint16_t length) {
    //Remove HDLC Header since its not needed anymore
    memcpy(&this->rawData[0], &inputData[config.HDLC_HEADER_SIZE], length-config.HDLC_HEADER_SIZE);

    uint8_t iv[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t aad[17] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t tag[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    this->getIv(iv);
    this->getAad(aad);
    this->getTag(tag);

    GCM<AESSmall128> gcmDec;
    gcmDec.setKey(this->encKey, sizeof(this->encKey));
    gcmDec.setIV(iv, sizeof(iv));
    gcmDec.addAuthData(aad, sizeof(aad));

    //Length of the Encrypted Data
    this->dataLength = this->rawData[8]-17;

    #ifdef LOGGING
        Serial.println("ImputData:");
        this->sendBytes(inputData,length);

        Serial.println("RawData:");
        this->sendBytes(this->rawData,length-config.HDLC_HEADER_SIZE);

        Serial.println("IV:");
        this->sendBytes(iv, 12);

        Serial.println("AuthKey:");
        this->sendBytes(config.AUTH_KEY, 16);

        Serial.println("AAD:");
        this->sendBytes(aad, 17);

        Serial.println("Tag:");
        this->sendBytes(tag, 12);
    #endif

    // Ciphertext is located after Byte 14
    gcmDec.decrypt(this->decryptedData, &this->rawData[14], this->dataLength);
    return gcmDec.checkTag(tag, sizeof(tag));
}

/**
 * Returns the initialization vector (IV) from the dataset
 * The IV is not returned by the function, instead the IV is written in the iv parameter
 * @param iv The IV will be written into this variable, has to be an array of 12Byte
 **/
void E450Dataset::getIv(uint8_t iv[12]) {
    // First 0-7 Bytes (Systm Title)
    memcpy(&iv[0], &this->rawData[0], 8);
    // Byte 10-13 v(=nonce). INFO: Nonce increased by 1 for each send dataset (=increment counter)
    memcpy(&iv[8], &this->rawData[10], 4);
}

/**
 * Returns the Additional Authenticated Data (AAD) from the dataset
 * The AAD is not returned by the function, instead the AAD is written in the iv parameter
 * @param aad The AAD will be written into this variable, has to be an array of 17Byte
 **/
void E450Dataset::getAad(uint8_t aad[17]) {
    // Byte 9 (Security Byte)
    memcpy(&aad[0], &this->rawData[9], 1);
    // Auth KEY
    memcpy(&aad[1], &config.AUTH_KEY[0], 16);
}


/**
 * Returns the Tag from the dataset
 * The Tag is not returned by the function, instead the Tag is written in the iv parameter
 * @param aad The Tag will be written into this variable, has to be an array of 12Byte
 **/
void E450Dataset::getTag(uint8_t tag[12]) {
    // Last 12 Bytes (Byte 8 contains the amount of Bytes after Byte 12 (Encrypted Date + Tag))
    memcpy(&tag[0], &this->rawData[(this->rawData[8]-3)], 12);
}


/**
 * Returns the serialnumber of the smartmeter which has send the dataset
 * The serial is not returned by the function, instead the serial is written in the serial parameter
 * @param serial The serial will be written into this variable, has to be an char array of 8Byte
 **/
void E450Dataset::getSerial(char serial[8])  {
    memcpy(&serial[0], &this->decryptedData[this->dataLength-8], 8);
}

/**
 * Returns and string which contains all relevant data from the decrypted dataset.
 * The string is already formatted to be send to and InfluxDB backed to be stored. 
 * Its not that universal I now. Feel free to modify the function to your needs.
 **/ 
String E450Dataset::getDataString(){
    char serial[8];

    this->getSerial(serial);

    uint16_t u1 = (uint16_t) this->decryptedData[21] << 8 | this->decryptedData[22];
    uint16_t u2 = (uint16_t) this->decryptedData[24] << 8 | this->decryptedData[25];
    uint16_t u3 = (uint16_t) this->decryptedData[27] << 8 | this->decryptedData[28];

    float i1 = float ((uint16_t) this->decryptedData[30] << 8 | this->decryptedData[31])/100;
    float i2 = float ((uint16_t) this->decryptedData[33] << 8 | this->decryptedData[34])/100;
    float i3 = float ((uint16_t) this->decryptedData[36] << 8 | this->decryptedData[37])/100;

    uint32_t pi = (uint32_t) this->decryptedData[39] << 24 | (uint32_t) this->decryptedData[40] << 16 | (uint16_t) this->decryptedData[41] << 8 | this->decryptedData[42];
    uint32_t pe = (uint32_t) this->decryptedData[44] << 24 | (uint32_t) this->decryptedData[45] << 16 | (uint16_t) this->decryptedData[46] << 8 | this->decryptedData[47];

    uint32_t ei = (uint32_t) this->decryptedData[49] << 24 | (uint32_t) this->decryptedData[50] << 16 | (uint16_t) this->decryptedData[51] << 8 | this->decryptedData[52];
    uint32_t ee = (uint32_t) this->decryptedData[54] << 24 | (uint32_t) this->decryptedData[55] << 16 | (uint16_t) this->decryptedData[56] << 8 | this->decryptedData[57];

    uint32_t ei_diff = 0;
    uint32_t ee_diff = 0;

    if(this->ei_last != 0) {
        ei_diff = ei - this->ei_last;
    }

    if(this->ee_last != 0) {
        ee_diff = ee - this->ee_last;
    }

    this->ei_last = ei;
    this->ee_last = ee;

    return "energy,Serial="+String(serial)
                +" U1="+String(u1)+",U2="+String(u2)+",U3="+String(u3)
                +",I1="+String(i1)+",I2="+String(i2)+",I3="+String(i3)
                +",PowerImport="+String(pi)+",PowerExport="+String(pe)
                +",EnergyImportTotal="+String(ei)+",EnergyExportTotal="+String(ee)
                +",EnergyImportDelta="+String(ei_diff)+",EnergyExportDelta="+String(ee_diff);;
}

/**
 * Debug function to send out the datased on the serial interface.
 * When used on an ArduinoUno it will be send out on the virtual com port.
 **/ 
void E450Dataset::sendBytes(uint8_t* bytes, unsigned int count){
    for(unsigned int i = 0; i < count; i++) {
      Serial.print(bytes[i], HEX);
      Serial.print(" ");
    }
    Serial.print("\n\r");
}
