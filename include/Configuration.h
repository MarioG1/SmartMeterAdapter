#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration
{
private:
public:
    //Wifi Settings
    const char* WIFI_SSID = "WIFI_SSID";
    const char* WIFI_PASSWORD = "WIFI_PASSWORD";

    //InfluxDB Settings
    const char* INFLUXDB_HOST = "127.0.0.1";
    const int INFLUXDB_PORT = 8086;
    const int INFLUXDB_TIMEOUT = 1000;
    const char* INFLUXDB_DBNAME = "smartmeter";

    //Other Sutff
    const int HDLC_HEADER_SIZE = 13;

    //Keys to decrypt HDLC frames send out by the SmartMeter. This are usually provided by your electricity supplier
    //Decryption Key
    uint8_t AUTH_KEY[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //Zähler 1
    uint8_t ENC_KEY_0[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //Zähler 2
    uint8_t ENC_KEY_1[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
};

#endif