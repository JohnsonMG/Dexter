/*
Dexter Movement
*/

#include <Servo.h>

Servo servo_fl;
Servo servo_fr;
Servo servo_bl;
Servo servo_br;


// Constants
const int HOME = 512;
const int DEAD_MIN = 485;
const int DEAD_MAX = 520;
const int RAMP_INTERVAL = 50;
const int POLL_INTERVAL = 500;


// Digital Pins
const int mot_fl = 2;
const int mot_fr = 3;
const int mot_bl = 4;
const int mot_br = 5;

// Analog Pins
const int joyX = 0;
const int joyY = 1;
const int joyR = 3;

// Initial conditions
int valX = HOME;
int valY = HOME;
int valR = HOME;
int motors[] = {HOME, HOME, HOME, HOME}; // {fl, fr, bl, br}
int prevMotors[] = {HOME, HOME, HOME, HOME};

void setup() {
  Serial.begin(9600);

  // Attach servos to pins
  servo_fl.attach(mot_fl);
  servo_fr.attach(mot_fr);
  servo_bl.attach(mot_bl);
  servo_br.attach(mot_br);
}

// Main Body
void loop() {
  read_input();
  set_motors();
}


void send_motors() {
  Serial.print("motors[]= ");
  Serial.print(motors[0]);
  Serial.print(", ");
  Serial.print(motors[1]);
  Serial.print(", ");
  Serial.print(motors[2]);
  Serial.print(", ");
  Serial.println(motors[3]);

  //send motor values to output pins
  servo_fl.write(motors[0]);
  servo_fr.write(motors[1]);
  servo_bl.write(motors[2]);
  servo_br.write(motors[3]);
}

void read_input() {
  // Read joystick inputs
  int x = analogRead(joyX);
  int y = analogRead(joyY);
  int r = analogRead(joyR);

  // Nullify deadzone
  valX = (x >= DEAD_MAX || x <= DEAD_MIN) ? x : HOME;
  valY = (y >= DEAD_MAX || y <= DEAD_MIN) ? y : HOME;
  valR = (r >= DEAD_MAX || r <= DEAD_MIN) ? r : HOME;
  shift_inputs();

  //testing print
  Serial.print("X: ");
  Serial.println(valX);
  Serial.print("Y: ");
  Serial.println(valY);

  // Control polling frequency
  delay(POLL_INTERVAL);
}

// Adjust input range from 0 <-> 1023 to -512 <-> 512
void shift_inputs() {
  valX = valX - HOME;
  valY = valY - HOME;
  valR = valR - HOME;
}

// Controller method for populating motor values array
void set_motors() {
  initial_outputs();
  scale_outputs();
  send_motors();
}

void initial_outputs() {
  //logic to set motor value. Adding HOME shifts range back to 0-1024
  motors[0] = valY + valX + valR + HOME; //front left
  motors[1] = valY - valX - valR + HOME; //front right
  motors[2] = valY - valX + valR + HOME; //back left
  motors[3] = valY + valX - valR + HOME; //back right
}

void scale_outputs() {
  //ensure range of 0 to 1023 then map for servo input range of 0-180
  for (int i = 0; i < 4; i++) {
    if (motors[i] > 1023) {
      motors[i] = 1023;
    }else if (motors[i] < 0){
      motors[i] = 0;
    }

    ramp_outputs();
    
    prevMotors[i] = motors[i];

    motors[i] = (int)map(motors[i], 0, 1023, 0, 180);
  }
}

// Increment the motor values gradually 
void ramp_outputs(){
  for (int i = 0; i < 4; i++){
    //Forwards
    if (prevMotors[i] - motors[i] > 0){
      if(prevMotors[i] + RAMP_INTERVAL < motors[i]{
        motors[i] = prevMotors[i] + RAMP_INTERVAL;
      }
    // Backwards
    } else if (prevMotors[i] - motors[i] < 0){
      if(prevMotors[i] - RAMP_INTERVAL > motors[i]{
        motors[i] = prevMotors[i] - RAMP_INTERVAL;
      }
    }
  }
}

