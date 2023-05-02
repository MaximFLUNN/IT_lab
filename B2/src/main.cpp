// #include <Arduino.h>
// #include "L3G.h"
// #include <Wire.h>
// int loop_time;
// const char* key;
// L3G gyro;

// void setup() {
//   pinMode(LED_BUILTIN, OUTPUT);
//   Serial.begin(115200);
//   digitalWrite(LED_BUILTIN, LOW);
//   loop_time = 500;
//   key = "ESP32Devkit1";
//   Wire.begin();

//   if (!gyro.init()) {
//   Serial.println("Failed to autodetect gyro type!");
//   while (1);
//   }
  
//   gyro.enableDefault();
// }

// void loop() {
//   gyro.read();
//   Serial.print("G ");
//   Serial.print("X: ");
//   Serial.print((int)gyro.g.x);
//   Serial.print(" Y: ");
//   Serial.print((int)gyro.g.y);
//   Serial.print(" Z: ");
//   Serial.println((int)gyro.g.z);
  
//   delay(100);
// }

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "L3G.h"
#include <Wire.h>
L3G gyro;

// Cгенерировать UUID можно здесь:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
int iter = 1;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    BLEDevice::init("ESP32Devkit1");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_NOTIFY
                                        );

    pCharacteristic->setValue("Hello World says Maxim");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising(); 
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");

    Wire.begin();
    if (!gyro.init()) {
        Serial.println("Failed to autodetect gyro type!");
        while (1);
    }

    gyro.enableDefault();
}

void loop() {
    gyro.read();
    std::string word = "G X: " + std::to_string((int)gyro.g.x) + std::string(" Y: ") + std::to_string((int)gyro.g.y) + std::string(" Z: ") + std::to_string((int)gyro.g.z);
    // word += " X: ";
    // word += (int)gyro.g.x;
    // word += " Y: ";
    // word += (int)gyro.g.y;
    // word += " Z: ";
    // word += (int)gyro.g.z;
    pCharacteristic->setValue(word);
    pCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    //   Serial.print("G ");
    //   Serial.print("X: ");
    //   Serial.print((int)gyro.g.x);
    //   Serial.print(" Y: ");
    //   Serial.print((int)gyro.g.y);
    //   Serial.print(" Z: ");
    //   Serial.println((int)gyro.g.z);

    // здесь пишем основной код
    // delay(1000);
    // std::string word = "Kolya loh count: " + std::to_string(iter);
    // pCharacteristic->setValue(word);
    // iter++;
}