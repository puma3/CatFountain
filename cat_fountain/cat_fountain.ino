// **** INCLUDES *****
#include <StateMachine.h>

#define ECHO_PIN 2
#define TRIGGER_PIN 3
#define PUMP_PIN 12
#define MIN_DISTANCE 32.0
#define FLAG_PIN LED_BUILTIN

const unsigned long TIME_TO_DRINK = 3000;
const unsigned long TIME_BETWEEN_CHECKS = 500;

StateMachine machine = StateMachine();

State *S0 = machine.addState(&state_0);
State *S1 = machine.addState(&state_1);
State *S2 = machine.addState(&state_2);

float get_distance()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

bool cat_is_around()
{
  float distance = get_distance();
  bool is_around = distance < MIN_DISTANCE && distance > 0.5;
  Serial.print("Distance: ");
  Serial.println(distance);
  return is_around;
}

void turn_on_pump()
{
  analogWrite(PUMP_PIN, 255);
}

void turn_off_pump()
{
  analogWrite(PUMP_PIN, 0);
}

void setup()
{
  // Sonar
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // Pump
  pinMode(PUMP_PIN, OUTPUT);

  // State Machine
  S0->addTransition(&transitionS0S1, S1);
  S1->addTransition(&transitionS1S2, S2);
  S2->addTransition(&transitionS2S1, S1);
  S2->addTransition(&transitionS2S0, S0);

  // Debug
  Serial.begin(9600);
}

void loop()
{
  delay(250);
  machine.run();
}

// State 0: Waiting for Cat ===============================
void state_0()
{
  // Serial.println("State 0");
  turn_off_pump();
  delay(TIME_BETWEEN_CHECKS);
}

bool transitionS0S1()
{
  return cat_is_around();
}

// State 1: Water Flowing =================================
void state_1()
{
  // Serial.println("State 1");
  if (machine.executeOnce)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    turn_on_pump();
  }
}

bool transitionS1S2()
{
  return !cat_is_around();
}

// State 2: Delay until pump is turned off ================
void state_2()
{
  // Serial.println("State 2");
  digitalWrite(LED_BUILTIN, LOW);
  delay(TIME_TO_DRINK);
}

bool transitionS2S1()
{
  return cat_is_around();
}

bool transitionS2S0()
{
  true;
}
