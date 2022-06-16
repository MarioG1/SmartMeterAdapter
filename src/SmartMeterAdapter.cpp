//#define LOGGING;

#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include <SoftwareSerial.h>
#include <..\lib\SoftwareSerial\SoftwareSerial.h>

#include "DeviceStatus.h"
#include "Configuration.h"
#include "InfluxDB.h"
#include "Arduhdlc.h"
#include "E450Dataset.h"

#define MAX_HDLC_FRAME_LENGTH 120

DeviceStatus dstatus;
Configuration config;
InfluxDB db(config.INFLUXDB_HOST, config.INFLUXDB_PORT, config.INFLUXDB_DBNAME, config.INFLUXDB_TIMEOUT);

unsigned long lastCycle = 0;

//SofwareSerial op Pins 10 and 11
SoftwareSerial sofwareSerial1(10, 11);

//HDLC => I noticed later that it is not really HDLC, just looks like it in the first place
void send_character(uint8_t data);
void hdlc_frame_handler_0(uint8_t *data, uint16_t length);
void hdlc_frame_handler_1(uint8_t *data, uint16_t length);
Arduhdlc hdlc0(&send_character, &hdlc_frame_handler_0, MAX_HDLC_FRAME_LENGTH);
Arduhdlc hdlc1(&send_character, &hdlc_frame_handler_1, MAX_HDLC_FRAME_LENGTH);

//E450 Datasets (Used to decrypt HDLC Frames)
E450Dataset dataset0(config.ENC_KEY_0);
E450Dataset dataset1(config.ENC_KEY_1);

/**
 * Connect to the configured WiFI network.
 * The function will not return until a connection to the WiFi network is established.
 **/ 
void wifiConnect() {
  #ifdef LOGGING
    Serial.print("WiFi Firmware Version: ");
    Serial.println(WiFi.firmwareVersion());
  #endif

  dstatus.setStatus(DeviceStatus::WIFI_CONNECT);

  // Wait until the WiFi module is booted and responses to commands
  while(WiFi.status() == WL_NO_MODULE) {
    #ifdef LOGGING
      Serial.println("No connection to WiFI module");
    #endif
    dstatus.setStatus(DeviceStatus::WIFI_ERROR);

    WiFi.disconnect();
    WiFi.end();

    delay(500);
  }

  // Set WiFi Connection Timeout to 15s
  WiFi.setTimeout(10*1000);
  WiFi.noLowPowerMode();

  // If WiFi is not connected try to connect...
  while (WiFi.status() != WL_CONNECTED) {
    dstatus.setStatus(DeviceStatus::WIFI_CONNECT);

    #ifdef LOGGING
        Serial.println("Disconnecting WiFi");
    #endif
    WiFi.disconnect();

    #ifdef LOGGING
        Serial.println("Connecting WiFi");
    #endif

    WiFi.begin(config.WIFI_SSID, config.WIFI_PASSWORD);
    delay(250);

    if(WiFi.status() != WL_CONNECTED) {
      dstatus.setStatus(DeviceStatus::WIFI_ERROR);
      #ifdef LOGGING
        Serial.println("Error Connecting to WiFI");
        Serial.print("Wifi Status: ");
        Serial.println(WiFi.status());
        Serial.print("Reason Code: ");
        Serial.println(WiFi.reasonCode());
      #endif
    }
  }
}

/**
 * Checks if the WiFi is connected and initializes and connect if not.
 **/ 
void checkWifiStatus() {
  if(WiFi.status() != WL_CONNECTED) {
    #ifdef LOGGING
        Serial.println("WiFi Connection Lost. Reconnecting...");
        Serial.print("Wifi Status: ");
        Serial.println(WiFi.status());
        Serial.print("Reason Code: ");
        Serial.println(WiFi.reasonCode());
    #endif
    wifiConnect();
  }
}

/**
 * Sends binary data formated as HEX out of Serail 0 (USB)
 * @param bytes Data to send
 * @param count Length of data to send
 **/ 
void sendBytes(uint8_t* bytes, unsigned int count){
    for(unsigned int i = 0; i < count; i++) {
      Serial.print(bytes[i], HEX);
      Serial.print(" ");
    }
}

/** 
 * Function to send out one 8bit character on Serial 0 (USB)
 **/
void send_character(uint8_t data) {
    Serial.print((char)data);
}

/**
 * HDLC frame handler for serial 1 (SmartMeter 0). Called when one HDLC frame was received
 * @param data data of the received HDLC frame
 * @param length length of the HDLC frame
 **/ 
void hdlc_frame_handler_0(uint8_t *data, uint16_t length) {
  if(!dataset0.decrypt(data, length)) {
      Serial.println("Landis: DecryptAndExtractValues: Encryption Tag Check error \r\n");
  } else {
    //check if WiFi is Still connected and recconect if not
    checkWifiStatus();
    dstatus.setStatus(DeviceStatus::BUSY);
    int result = db.Push(dataset0.getDataString());
    if(result == 200 || result  == 204) {
      dstatus.setStatus(DeviceStatus::READY);
    } else {
      dstatus.setStatus(DeviceStatus::ERROR);
    }
  }
}

/**
 * HDLC frame handler for serial 2 (SmartMeter 1). Called when one HDLC frame was received
 * @param data data of the received HDLC frame
 * @param length length of the HDLC frame
 **/ 
void hdlc_frame_handler_1(uint8_t *data, uint16_t length) {
  if(!dataset1.decrypt(data, length)) {
      Serial.println("Landis: DecryptAndExtractValues: Encryption Tag Check error \r\n");
  } else {
    //check if WiFi is Still connected and recconect if not
    checkWifiStatus();
    dstatus.setStatus(DeviceStatus::BUSY);
    int result = db.Push(dataset1.getDataString());
    if(result == 200 || result  == 204) {
      dstatus.setStatus(DeviceStatus::READY);
    } else {
      dstatus.setStatus(DeviceStatus::ERROR);
    }
  }
}

//---------------------------------------------------------------------------------------------------------
// Arduino Setup
//---------------------------------------------------------------------------------------------------------

void setup() {
  dstatus.Init();

  //Init Serial 0 for debug output to the PC
  Serial.begin(19200, SERIAL_8E1);

  //Init Serial 1 for Smart Meter communication
  Serial1.begin(2400, SERIAL_8E1);

  #ifdef LOGGING
    Serial.println("Startup...");
  #endif

  //Init Serial 2 (Sofware Serial) for Smart Meter communication
  sofwareSerial1.begin(2400);
  sofwareSerial1.listen();
 
  //Device Status LED
  dstatus.setStatus(DeviceStatus::WIFI_CONNECT);

  //Connect to WiFi network
  wifiConnect();

  dstatus.setStatus(DeviceStatus::READY);
}


//---------------------------------------------------------------------------------------------------------
// Arduino Main Loop
//---------------------------------------------------------------------------------------------------------
void loop() {
  dstatus.blinkCycle(500);

  while (Serial1.available() > 0) {
    // get the new byte:
    char inChar = Serial1.read();
    // Pass all incoming data to hdlc char receiver
    hdlc1.charReceiver(inChar);
  }

  while (sofwareSerial1.available() > 0) {
    char inChar = sofwareSerial1.read();
    // Pass all incoming data to hdlc char receiver
    hdlc0.charReceiver(inChar);
  }
}