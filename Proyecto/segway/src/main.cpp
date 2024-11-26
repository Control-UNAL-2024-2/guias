#include <Arduino.h>
#include "definitions.h"
#include <ESP32Encoder.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>
#include <MPU9250_WE.h>

// Encoders
ESP32Encoder encoderMotorA;
ESP32Encoder encoderMotorB;

// IMU
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);
int status;

// Timers
hw_timer_t *timer = NULL;

// Control Variables

// Sampling time
float T = SAMPLING_TIME;

// Saturation voltages
int max_voltage = 5;
int min_voltage = -5;

// Motors

const int R = 0;
const int L = 1;

// Records
float prev_motor_speed[2] = {0, 0};

float segwayReference = 0;
float motorSpeedReference = 0;




// function declarations here

float motorSaturation(float u){
  return constrain(u, max_voltage, min_voltage);
}


void voltsToMotor(float volts, int motor){
    // This function convert a voltage value given in variable volts
    // to a bipolar pwm signal for controlling the motor

    unsigned int pwm = abs(volts)*percent2pwm;

    int signal_1=0;
    int signal_2=0;

    if (volts < 0){
        // if var volts is negative use CH_PWM_AIN2 to output a pwm signal
        // proportional to the input voltage
        signal_1 = 0;
        signal_2 = pwm;
    }
    else{
        // if var volts is negative use CH_PWM_AIN1 to output a pwm signal
        // proportional to the input voltage
        signal_1 = pwm;
        signal_2 = 0;
    }

    if(motor == R){
      ledcWrite(CH_PWM_AIN1, signal_1);
      ledcWrite(CH_PWM_AIN2, signal_2);
    }else{
      ledcWrite(CH_PWM_BIN1, signal_1);
      ledcWrite(CH_PWM_BIN2, signal_2);
    }
}

void motorSpeedControl(float reference, int motorNumber, float P, float I, float D){
  // reference: deg/s
  // motor: Encoder object
  // motorName: Name of the motor

  float speed = 0;

  if(motorNumber == R){
    speed = encoderMotorA.getCount() * pulses2degrees/T;
    encoderMotorA.clearCount();
  }else if(motorNumber == L){
    speed = encoderMotorB.getCount() * pulses2degrees/T;
    encoderMotorB.clearCount();
  }

  // Compute U
  float u = 0;


  // Saturation cut
  u = motorSaturation(u);


  // Update motor speed
  voltsToMotor(0, motorNumber);

  // Update speeds and poss
  prev_motor_speed[motorNumber] = speed;
}


void controlSegway(){
  // Get IMU states


  // IMU


  // Compute control actions


  // set motors references

  // Control motors
  motorSpeedControl(motorSpeedReference, R, 1, 1, 1);
  motorSpeedControl(motorSpeedReference, L, 1, 1, 1);
}

void setup() {
    setCpuFrequencyMhz(80);
    // put your setup code here, to run once:
    Serial.begin(115200);
    //setting the encoders
    encoderMotorA.attachFullQuad (CH_ENC_A1, CH_ENC_B1);
    encoderMotorB.attachFullQuad (CH_ENC_A2, CH_ENC_B2);
    encoderMotorA.setCount(0);
    encoderMotorB.setCount(0);


    // PWM and motor initialization

    // R Motor
    ledcSetup(CH_PWM_AIN1, FREQUENCY_PWM, RESOLUTION_PWM);
    ledcSetup(CH_PWM_AIN2, FREQUENCY_PWM, RESOLUTION_PWM);

    ledcAttachPin(PIN_AIN1, CH_PWM_AIN1);
    ledcAttachPin(PIN_AIN2, CH_PWM_AIN2);


    // L Motor
    ledcSetup(CH_PWM_BIN1, FREQUENCY_PWM, RESOLUTION_PWM);
    ledcSetup(CH_PWM_BIN2, FREQUENCY_PWM, RESOLUTION_PWM);

    ledcAttachPin(PIN_BIN1, CH_PWM_BIN1);
    ledcAttachPin(PIN_BIN2, CH_PWM_BIN2);

    voltsToMotor(0, R);
    voltsToMotor(0, L);

    // Timer initialization (interruption for sampling time)
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &controlSegway, true);
    timerAlarmWrite(timer, SAMPLING_TIME*1000000, true);
    timerAlarmEnable(timer);

    Wire.begin(21, 22);

    if(!myMPU9250.init()){
      Serial.println("MPU9250 does not respond");
    }
    else{
      Serial.println("MPU9250 is connected");
    }

    myMPU9250.autoOffsets();
    Serial.println("Done!");

    myMPU9250.setSampleRateDivider(5);
    myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);

    myMPU9250.enableAccDLPF(true);
    myMPU9250.setAccDLPF(MPU9250_DLPF_6);
}



void loop() {
  xyzFloat accRaw = myMPU9250.getGyrValues();
  float pitch = myMPU9250.getPitch();
  float roll = myMPU9250.getRoll();
  
  Serial.println("pitch:");
  Serial.println(pitch);


  Serial.println("roll:");
  Serial.println(roll);
  
  delay(100);
}