// **** INCLUDES *****
#include "LowPower.h"

#define ECHO_PIN     2
#define TRIGGER_PIN  3
#define PUMP_PIN     11
#define MIN_DISTANCE 25.0
#define FLAG_PIN LED_BUILTIN

const unsigned long TIME_TO_DRINK =       1000;
//const unsigned long TIME_BETWEEN_CHECKS = 500;
const unsigned long TIME_BETWEEN_CHECKS = SLEEP_500MS;

void toggle_pump_state(float distance) {
  if(distance == 0.0) {
    analogWrite(PUMP_PIN, 0);
  } else {
    float factor = (MIN_DISTANCE - distance) / MIN_DISTANCE;
    int level = 195 + (int)45 * factor;
    analogWrite(PUMP_PIN, level);
  }
}

float get_distance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  float duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

bool cat_is_around(float distance) {
  return distance < MIN_DISTANCE && distance > 0.0;
}

void set_flag(float distance) {
  digitalWrite(FLAG_PIN, cat_is_around(distance) ? HIGH : LOW);
}

void setup() {
  // Sonar
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // Pump
  pinMode(PUMP_PIN, OUTPUT);
//  Serial.begin(9600);
}

void sleep_while_waiting_for_cat() {
  LowPower.powerDown(TIME_BETWEEN_CHECKS, ADC_OFF, BOD_OFF);
//  delay(TIME_BETWEEN_CHECKS);
}

void sleep_while_water_flowing(int factor = 1) {
//  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  delay(TIME_TO_DRINK * factor);
}

void loop() {
  float distance = get_distance();
//  Serial.print("Distance: ");
//  Serial.println(distance);

  if(digitalRead(FLAG_PIN) == HIGH) {
    if(cat_is_around(distance)) {
      toggle_pump_state(distance);
    } else {
      sleep_while_water_flowing(3);
      toggle_pump_state(0.0);
    }
  } else if(!cat_is_around(distance)) {
    sleep_while_waiting_for_cat();
  }

  set_flag(distance);
}
