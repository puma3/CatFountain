// **** INCLUDES *****
#include "LowPower.h"

#define ECHO_PIN     2
#define TRIGGER_PIN  3
#define PUMP_PIN     7
#define MAX_DISTANCE 200

const unsigned long TIME_TO_DRINK =       3000;  // miliseconds
const unsigned long TIME_BETWEEN_CHECKS = 1000; // miliseconds

void toggle_pump_state(bool on) {
  if(on) {
    Serial.println("Pump turning on");
    analogWrite(PUMP_PIN, 140);
//    digitalWrite(PUMP_PIN, HIGH);
  } else {
    Serial.println("Pump turning off");
    analogWrite(PUMP_PIN, 0);
//    digitalWrite(PUMP_PIN, LOW);
  }
//  digitalWrite(PUMP_PIN, on ? HIGH : LOW);
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

bool cat_is_around() {
  float distance = get_distance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" => ");
  return distance > 0.0 && distance < 30.0;
}

void setup() {
  // Sonar
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // Pump
  pinMode(PUMP_PIN, OUTPUT);
  Serial.begin(9600);
}

void sleep_while_waiting_for_cat() {
//  delay(TIME_BETWEEN_CHECKS);
  LowPower.idle(SLEEP_4S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

void sleep_while_water_flowing() {
//  delay(TIME_TO_DRINK);
  LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

void loop() {
  if(cat_is_around()) {
    Serial.println("Cat IS AROUND and pump will be turned on");
    toggle_pump_state(true);
    sleep_while_water_flowing();
  } else {
    Serial.println("Cat IS NOT AROUND: Waiting for cat");
    toggle_pump_state(false);
    sleep_while_waiting_for_cat();
  }
}
