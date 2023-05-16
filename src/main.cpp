#include <Preferences.h>
#include "Definitions.h"

Preferences parameters;

void loadParameterFromEEPROM();

int32_t laser1Time = 0;
int8_t laser1Power = 0;
int32_t laser1Delay = 0;

int32_t laser2Time = 0;
int8_t laser2Power = 0;
int32_t laser2Delay = 0;

bool power_state = false;
int32_t power_timer = 0;
int32_t max_time = 0;


String _receivedString = "";
bool _isStringComplete = false;

bool laser1Active = false;
uint64_t laser1Change = 0;

bool laser2Active = false;
uint64_t laser2Change = 0;

void setup() {
  analogReadResolution(12);
  Serial.begin(115200);
  Serial.println("\r\nESP32-C3");
  Serial.println("Bi-Laser System");
  loadParameterFromEEPROM();

  if(laser1Time > laser2Time)
    max_time = laser1Time;
  else
    max_time = laser2Time;

  pinMode(DOOR_INPUT, INPUT);

  pinMode(POWER_SGNL, OUTPUT);
  digitalWrite(POWER_SGNL, 0);

  ledcSetup(0, 1000, 14);
  ledcAttachPin(PWM1_SGNL, 0);
  ledcWrite(0, 0);
   
  ledcSetup(1, 1000, 14);
  ledcAttachPin(PWM2_SGNL, 1);
  ledcWrite(1, 0);
}

void loop() { 

  if(power_state){
    if(millis() - power_timer >= max_time){
      Serial.println("POWER:OFF");
      digitalWrite(POWER_SGNL, LOW);
      power_state = false;
    }
  }

  if(laser1Active){
    if(millis() - laser1Change >= laser1Time || digitalRead(DOOR_INPUT)){
      laser1Active = false;
      ledcWrite(0, 0);
      Serial.println("TRIGG:L1:END");
    }
  }

  if(laser2Active) { 
    if(millis() - laser2Change >= laser2Time || digitalRead(DOOR_INPUT)){
      laser2Active = false;
      ledcWrite(1, 0);
      Serial.println("TRIGG:L2:END");
    }
  }

  while(Serial.available()>0){
    char inChar = (char) Serial.read();
    if(inChar == '#'){
      _isStringComplete = true;
      break;
    }
    if(inChar != '\r'){
        _receivedString += inChar;
    }
  }

  if(_isStringComplete){
    int _bufferNumber = 0;
    if(_receivedString.indexOf(" ") != -1){
      String _stringNumber = _receivedString.substring(_receivedString.indexOf(" ") + 1, _receivedString.length());
      Serial.println("'" + _stringNumber + "'");
      _bufferNumber = _stringNumber.toInt();
      _receivedString  = _receivedString.substring(0, _receivedString.indexOf(" "));
      Serial.println("'" + _receivedString + "'");
    }
    if(_receivedString == "DOOR:STATE?"){
      Serial.println((String) "DOOR:" + digitalRead(DOOR_INPUT));
    }
    else if(_receivedString == "SER:TEST"){
      Serial.println("SER:OK");
    }
    else if(_receivedString == "ESP:RESET"){
      Serial.println("ESP:RESET");
      ESP.restart();
    }
    else if(_receivedString == "TRIGG:L1"){
      if(!digitalRead(DOOR_INPUT)){
        if(!power_state){
          digitalWrite(POWER_SGNL, HIGH);
          power_timer = millis();
          power_state = true;
        } else{
          power_timer = millis();
        }
        Serial.println("TRIGG:L1:START");
        laser1Active = true;
        laser1Change = millis();
        int32_t buff = map(laser1Power, 0, 100, 0, 16383);
        ledcWrite(0, buff);
      } else {
        Serial.println("TRIGG:L1:BLOCKED");
      }
    }
    else if(_receivedString == "TRIGG:L2"){
      if(!digitalRead(DOOR_INPUT)){
        if(!power_state){
          digitalWrite(POWER_SGNL, HIGH);
          power_timer = millis();
          power_state = true;
        } else {
          power_timer = millis();
        }
        Serial.println("TRIGG:L2:START");
        laser2Active = true;
        laser2Change = millis();
        int32_t buff = map(laser2Power, 0, 100, 0, 16383);
        ledcWrite(1, buff);
      } else {
        Serial.println("TRIGG:L2:BLOCKED");
      }
    }
    else if(_receivedString == "CAL:L1:POWER"){
      parameters.begin("laser1Params", false);
      parameters.putChar("power", _bufferNumber);
      Serial.println("newLaser1Power: " + (String) _bufferNumber);
      laser1Power = _bufferNumber;
      parameters.end();
    }
    else if(_receivedString == "CAL:L1:TIME"){
      parameters.begin("laser1Params", false);
      parameters.putInt("time", _bufferNumber);
      Serial.println("newLaser1Time: " + (String) _bufferNumber);
      laser1Time = _bufferNumber;
      parameters.end();
    }
    else if(_receivedString == "CAL:L1:DELAY"){
      parameters.begin("laser1Params", false);
      parameters.putInt("delay", _bufferNumber);
      Serial.println("newLaser1Delay: " + (String) _bufferNumber);
      laser1Delay = _bufferNumber;
      parameters.end();
    }
    else if(_receivedString == "CAL:L2:POWER"){
      parameters.begin("laser2Params", false);
      parameters.putChar("power", _bufferNumber);
      Serial.println("newLaser2Power: " + (String) _bufferNumber);
      laser2Power = _bufferNumber;
      parameters.end();
    } 
    else if(_receivedString == "CAL:L2:TIME"){
      parameters.begin("laser2Params", false);
      parameters.putInt("time", _bufferNumber);
      Serial.println("newLaser2Time: " + (String) _bufferNumber);
      laser2Time = _bufferNumber;
      parameters.end();
    }
    else if(_receivedString == "CAL:L2:DELAY"){
      parameters.begin("laser2Params", false);
      parameters.putInt("delay", _bufferNumber);
      Serial.println("newLaser2Delay: " + (String) _bufferNumber);
      laser2Delay = _bufferNumber;
      parameters.end();
    }
    _isStringComplete = false;
    _receivedString = "";
  }
}

void loadParameterFromEEPROM(){
  parameters.begin("laser1Params", false);
  laser1Power = parameters.getChar("power", 0);
  laser1Time = parameters.getInt("time", 0);
  laser1Delay = parameters.getInt("delay", 0);
  parameters.end();

  parameters.begin("laser2Params", false);
  laser2Power = parameters.getChar("power", 0);
  laser2Time = parameters.getInt("time", 0);
  laser2Delay = parameters.getInt("delay", 0);
  parameters.end();

  Serial.println((String) "laser1Power: " + laser1Power);
  Serial.println((String) "laser1Time: " + laser1Time);
  Serial.println((String) "laser1Delay: " + laser1Delay);
  Serial.println((String) "laser2Power: " + laser2Power);
  Serial.println((String) "laser2Time: " + laser2Time);
  Serial.println((String) "laser2Delay: " + laser2Delay);
}