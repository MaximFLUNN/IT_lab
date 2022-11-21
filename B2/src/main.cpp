#include <Arduino.h>
#include "tinyexpr.h"
//#include <BluetoothSerial.h>
//#include <iocontrol.h>
//BluetoothSerial SerialBT;
int loop_time;
const int buff_size = 48;
char _exp[buff_size] = {0};
int iterator;
double ans = NAN;
std::string answer;
int size = 0;
bool work = false;
bool tr = false;

//int inc;
//bool zero = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(256000);
  digitalWrite(LED_BUILTIN, LOW);
  //SerialBT.begin("ESP32 Control dev v1");
  //Serial.println("Start\n");
  
  loop_time = 500;
}

void loop() {
  //delay(loop_time * 0.5);
  iterator = 0;
  ans = NAN;
  while (Serial.available()) {
  if (work) {
     digitalWrite(LED_BUILTIN, HIGH);
  }
    work = true;
    delay(0);
    if (Serial.available() >0 ) {
      char c = Serial.read();
      _exp[iterator] = c;
      iterator++;
      tr = true;
    }
  }

// ans = te_interp(_exp, 0);
//   if (ans != -999) {
//     answer = std::to_string(ans);
//     if (answer != "nan") {
//       size = answer.length();
//       for (int i = 0; i < size; i++) {
//         Serial.write(answer[i]);
//       }
//       Serial.write('\t');
//       for (int i = 0; i < 256; i++) {
//         _exp[i] = 0;
//       }
//     }
  if (tr) {
    ans = te_interp(_exp, 0);
    answer = std::to_string(ans);
    size = answer.length();
    for (int i = 0; i < size; i++) {
      Serial.write(answer[i]);
    }
    //Serial.print(ans);
    Serial.print("\t");
    for (int i = 0; i < buff_size; i++) {
      _exp[i] = 0;
    }
  }
  tr = false;
  work = false;
  // if (ans != -999) {
  //   answer = std::to_string(ans);
  //   if (answer != "nan") {
  //     size = answer.length();
  //     for (int i = 0; i < size; i++) {
  //       Serial.write(answer[i]);
  //     }
  //     Serial.write('\t');
  //     for (int i = 0; i < 256; i++) {
  //       _exp[i] = 0;
  //     }
  //   }
  // }
  //delay(loop_time * 0.5);
  if (!work) {
  digitalWrite(LED_BUILTIN, LOW);
  }

}