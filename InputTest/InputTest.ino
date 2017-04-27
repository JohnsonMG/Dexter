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
const int RAMP_LOW = 10;
const int RAMP_MID = 50;
const int RAMP_HIGH = 50;
const int POLL_INTERVAL = 25;

// Digital Pins
const int mot_fl = 2;
const int mot_fr = 3;
const int mot_bl = 4;
const int mot_br = 5;

const int enc_flA = 18;
const int enc_flB = 19;
const int enc_frA = 20;
const int enc_frB = 21;
const int enc_blA = 22;
const int enc_blB = 23;
const int enc_brA = 24;
const int enc_brB = 25;

// Analog Pins
const int joyX = 0;
const int joyY = 1;
const int joyR = 3;

// Initial conditions
int valX = HOME;
int valY = HOME;
int valR = HOME;
int motors[] = {0, 0, 0, 0}; // {fl, fr, bl, br}
int prevMotors[] = {0, 0, 0, 0};
volatile long encoders[] = {0, 0, 0, 0};
long prevEncoders[] = {0, 0, 0, 0};
int velocities[] = {0, 0, 0, 0};
int pollCounter = 0;

unsigned long newTime = 0;
unsigned long oldTime = 0;

void setup() {
  Serial.begin(9600);

  // Attach servos to pins
  servo_fl.attach(mot_fl);
  servo_fr.attach(mot_fr);
  servo_bl.attach(mot_bl);
  servo_br.attach(mot_br);
  
  pinMode(enc_flA, INPUT);
  pinMode(enc_flB, INPUT);
  pinMode(enc_frA, INPUT);
  pinMode(enc_frB, INPUT);
  pinMode(enc_blA, INPUT);
  pinMode(enc_blB, INPUT);
  pinMode(enc_brA, INPUT);
  pinMode(enc_brB, INPUT);

  digitalWrite(enc_flA, HIGH);
  digitalWrite(enc_flB, HIGH);
  digitalWrite(enc_frA, HIGH);
  digitalWrite(enc_frB, HIGH);
  digitalWrite(enc_blA, HIGH);
  digitalWrite(enc_blB, HIGH);
  digitalWrite(enc_brA, HIGH);
  digitalWrite(enc_brB, HIGH);

  attachInterrupt(digitalPinToInterrupt(18), encoderInterrupt, RISING);
}

// Interrupt only reads movement on front-left encoder
void encoderInterrupt()
{
  for(int i = 0; i < 4; i++){
    if (digitalRead(18 + (i*2)) == digitalRead(19 + (i*2))) {
      encoders[i]++;
    } else {
      encoders[i]--;
    }
  }
}

// Main Body
void loop() {
  set_inputs();
  set_motors();
  // Control polling frequency
  delay(POLL_INTERVAL);
}

void set_inputs(){
  read_input();  
  read_encoders();
  shift_inputs();
  send_motors();
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

  //testing print
  Serial.print("X: ");
  Serial.println(valX);
  Serial.print("Y: ");
  Serial.println(valY);
  Serial.print("R: ");
  Serial.println(valR);
}

void read_encoders() {
  newTime = millis();
  for (int i = 0; i < 4; i ++){    
    velocities[i] = (encoders[i]-prevEncoders[i]) * 1000 /(newTime-oldTime);
    Serial.print ("speed = ");
    Serial.println (vel);
    prevEncoders[i] = encoders[i];
  }
  oldTime = newTime;
}

// Adjust input range from 0 <-> 1023 to -512 <-> 512
void shift_inputs() {
  valX = valX - HOME;
  valY = valY - HOME;
  valR = valR - HOME;
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

// Controller method for populating motor values array
void set_motors() {
  initial_outputs();
  scale_outputs();
  send_motors();
}

void initial_outputs() {
  //logic to set motor value. Adding HOME shifts range back to 0-1024
  motors[0] = valY + valX + valR; //front left
  motors[1] = -(valY - valX - valR); //front right
  motors[2] = valY - valX + valR; //back left
  motors[3] = -(valY + valX - valR); //back right
}

void scale_outputs() {
  //ensure range of 0 to 1023 then map for servo input range of 0-180 
  for (int i = 0; i < 4; i++) {
    if (motors[i] > 512) {
      motors[i] = 512;
    }else if (motors[i] < -512){
      motors[i] = -512;
    }
    
    ramp_outputs(i);
    
    prevMotors[i] = motors[i];
    if (motors[i] == 0)
    {
      motors[i] = 90;
    }
    else if (motors[i] < 0)
    {
      motors[i] = (int)map(motors[i], -512, 0, 70, 85);
    }
    else
    {
      motors[i] = (int)map(motors[i], 0, 512, 101, 116);
    }
  }
}

// Increment the motor values gradually 
void ramp_outputs(int i){
    int ramp = RAMP_LOW;
    if(prevMotors[i] >= 0)
    {
      if((prevMotors[i] > 200) || (prevMotors[i] < -200)) {ramp = RAMP_HIGH;}
      else if((prevMotors[i] > 80) || (prevMotors[i] < -80)) {ramp = RAMP_MID;}
      else {ramp = RAMP_LOW;}
    }
    if(((prevMotors[i] > 0) && (motors[i] < prevMotors[i])) || ((prevMotors[i] < 0) && (motors[i] > prevMotors[i])))
    {
      ramp = ramp*2;
    }
    //Forwards
    if (prevMotors[i] - motors[i] < 0){
      if(prevMotors[i] + ramp < motors[i]){
        motors[i] = prevMotors[i] + ramp;
      }
    // Backwards
    } else if (prevMotors[i] - motors[i] > 0){
      if(prevMotors[i] - ramp > motors[i]){
        motors[i] = prevMotors[i] - ramp;
      }
    }
}

