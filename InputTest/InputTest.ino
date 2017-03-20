/*
Dexter Movement
*/

#include <Servo.h>

Servo servo_fl;
Servo servo_fr;
Servo servo_bl;
Servo servo_br;

const int HOME = 512;

int joyX = 0;
int joyY = 1;
int joyR = 3;
int pin_fl = 2;
int pin_fr = 3;
int pin_bl = 4;
int pin_br = 5;
int valX = HOME;
int valY = HOME;
int valR = HOME;
int motors[] = {HOME, HOME, HOME, HOME}; // {fl, fr, bl, br}
int dead_min = 485;
int dead_max = 520;

void setup() {
  Serial.begin(9600);

  servo_fl.attach(pin_fl);
  servo_fr.attach(pin_fr);
  servo_bl.attach(pin_bl);
  servo_br.attach(pin_br);

//  pinMode(pin_fl, OUTPUT);
//  pinMode(pin_fr, OUTPUT);
//  pinMode(pin_bl, OUTPUT);
//  pinMode(pin_br, OUTPUT);
}

void loop() {
  read_input();
  set_motors();
}

void send_motors() {
  //send motor values to output pins

  Serial.print("motors[]= ");
  Serial.print(motors[0]);
  Serial.print(", ");
  Serial.print(motors[1]);
  Serial.print(", ");
  Serial.print(motors[2]);
  Serial.print(", ");
  Serial.println(motors[3]);

  servo_fl.write(motors[0]);
  servo_fr.write(motors[1]);
  servo_bl.write(motors[2]);
  servo_br.write(motors[3]);

//  analogWrite(pin_fl, motors[0]);
//  analogWrite(pin_fr, motors[1]);
//  analogWrite(pin_bl, motors[2]);
//  analogWrite(pin_fr, motors[3]);
}

void read_input() {
  // read the value from the sensor:
  int x = analogRead(joyX);
  int y = analogRead(joyY);
  int r = 768; //analogRead(joyR);

  //nullify deadzone
  valX = (x >= dead_max || x <= dead_min) ? x : HOME;
  valY = (y >= dead_max || y <= dead_min) ? y : HOME;
  valR = (r >= dead_max || r <= dead_min) ? r : HOME;
  shift_inputs();

  //testing print
  Serial.print("X: ");
  Serial.println(valX);
  Serial.print("Y: ");
  Serial.println(valY);

  //control polling frequency
  delay(500);
}

//Adjust input range from 0 <-> 1023 to -512 <-> 512
void shift_inputs() {
  valX = valX - HOME;
  valY = valY - HOME;
  valR = valR - HOME;
}

void set_motors() {
  //logic to set motor value. Adding HOME shifts range back to 0-1024
  motors[0] = valY + valX + valR + HOME; //front left
  motors[1] = valY - valX - valR + HOME; //front right
  motors[2] = valY - valX + valR + HOME; //back left
  motors[3] = valY + valX - valR + HOME; //back right

  scale_outputs();

  send_motors();
}

void scale_outputs() {
  //ensure range of 0 to 1023 then map for servo input range of 0-180
  for (int i = 0; i < 4; i++) {
    if (motors[i] > 1023) motors[i] = 1023;
    if (motors[i] < 0) motors[i] = 0;

    motors[i] = (int)map(motors[i], 0, 1023, 0, 180);
  }
}



