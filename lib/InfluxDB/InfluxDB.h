#ifndef INFLUXDB_H
#define INFLUXDB_H

#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

class InfluxDB
{
private:
    WiFiClient wClient;
    HttpClient hClient;
    String DatabaseName;
public:
    InfluxDB(char host[], int port, char db[], int timeout);
    int Push(String data);
};

#endif