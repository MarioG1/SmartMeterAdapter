//#define LOGGING;

#include "InfluxDB.h"

InfluxDB::InfluxDB(char host[], int port, char db[], int timeout) : hClient(wClient, host, port) {
    DatabaseName = db;
    hClient.setHttpResponseTimeout(timeout);
}

int InfluxDB::Push(String postData) {

    String contentType = "text/plain";

    hClient.post("/write?db="+DatabaseName, contentType, postData);

    int statusCode = hClient.responseStatusCode();
    hClient.responseBody();


    #ifdef LOGGING
        Serial.println(postData);
        Serial.println("InfluxDB Returned Status Code: " + String(statusCode));
    #endif

    return statusCode;
}