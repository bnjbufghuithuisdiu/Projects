#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include<Servo.h>
#include <MPU6050_light.h>
// Adafruit_MPU6050 mpu;
MPU6050 mpu(Wire);

// ----------------- Motor Pins -----------------
#define ENA 6
#define IN1 A2
#define IN2 A3
#define ENB 5
#define IN3 9
#define IN4 4
#define encodPinAR 2   
#define encodPinBR 3   
#define encodPinAL 7   
#define encodPinBL 8  
#define MAX_INPUT 100
double POS = 0;
char inputBuffer[MAX_INPUT];
int idx = 0;
volatile long wheel_pulse_count_left = 0;
volatile long wheel_pulse_count_right = 0;
int prevA_right = LOW;
int prevA_left  = LOW;
// loop timing 
double fs = 200.0;
double dt = 1.0 / fs;
uint16_t preLoader=65536-(16*pow(10,6)*dt/1024);
long int lastTime=0;
//complementary filter
double tau = 0.05;
double alpha = tau / (tau + dt);
// float alpha = 0.98;
double angle = 0;
double prevAccFiltered = 0;
bool ch=0;
int count=0;
//gyro calibration
float gyroX_offset = -0.03;


// tilt PID
double Kp = 0.0;
double Ki = 0.0;
double Kd = 0.0;
double Ka = 0.0;
//position pid
double Kp_p = 0;
double Ki_p = 0;
double Kd_p = 0;

double errorTilt, prevErrorTilt = 0;
double integralTilt = 0;
double errorPOS, prevErrorPOS = 0;
double prevVelTilt=0;
double integralPOS = 0;
bool isPositive=true;
Servo servo1;
Servo servo2;
char data[64];
volatile bool flag=true;
void servoGrip(char cmd){
  if(cmd=='o'){
    for (int angle = 15; angle <= 35; angle++) {
      servo1.write(angle);
      // servo2.write(angle);
      delay(25); 
    }
  }
  

  // 180 to 0 degrees
  if(cmd=='c'){
    for (int angle = 35; angle >= 15; angle--) {
      servo1.write(angle);
      // servo2.write(angle);
      delay(25); 
    }
  }
  

}
void servoArm(char cmd){
  if(cmd=='u'){
    for (int angle = 45; angle <= 90; angle++) {
      servo2.write(angle);
      delay(25); 
    }
  }
  

  // Sweep from 180 to 0 degrees
  if(cmd=='d'){
    for (int angle = 90; angle >= 45; angle--) {
      servo2.write(angle);
      delay(25); 
    }
  }
}
void processInput(char *data) {
  char *token;
  
  token = strtok(data, ",");
  if (token != NULL) Kp = atof(token);

  token = strtok(NULL, ",");
  if (token != NULL) Ki = atof(token);

  token = strtok(NULL, ",");
  if (token != NULL) Kd = atof(token);
  token = strtok(NULL, ",");
  if (token != NULL) Ka = atof(token);
  token = strtok(NULL, ",");
  if (token != NULL) Kp_p = atof(token);

  token = strtok(NULL, ",");
  if (token != NULL) Ki_p = atof(token);
  token = strtok(NULL, ",");
  if (token != NULL) Kd_p = atof(token);
  
  // Round to 3 decimal places
  Kp = round(Kp * 1000) / 1000.0;
  Ki = round(Ki * 1000) / 1000.0;
  Kd = round(Kd * 1000) / 1000.0;
  Kp_p = round(Kp_p * 1000) / 1000.0;
  Ki_p = round(Ki_p * 1000) / 1000.0;
  Kd_p = round(Kd_p * 1000) / 1000.0;
  // Serial.println("Received values:");
  // Serial.print("k1 = ");
  // Serial.println(Kp, 3);
  // Serial.print("k2 = ");
  // Serial.println(Ki, 3);
  // Serial.print("k3 = ");
  // Serial.println(Kd, 3);
}


ISR(TIMER1_OVF_vect){
  flag=true;
  TCNT1=preLoader;
}



// ------------------------------------------------
//              MOTOR CONTROL
// ------------------------------------------------
void setMotor(int pwm) {
  //pwm=100;
  
  if (pwm >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  pwm = constrain(abs(pwm), 0, 255);
  
  // // Deadband boost for 100:1 backlash
  // if (abs(angle)*57.2958 > 17.0 | ch==1) {
  //   pwm= 255;
  //   ch=1;
  // }
  pwm = constrain(pwm, 0, 255);
  //Serial.println(pwm);
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
  // Serial.print(",");
  // Serial.println(pwm);
}


// void calibrateGyro() {
//   Serial.println("Calibrating Gyro... Keep robot still");
//   double sum = 0;

//   for (int i = 0; i < 1000; i++) {
//     sensors_event_t a, g, temp;
//     mpu.getEvent(&a, &g, &temp);
//     sum += g.gyro.x;
//     delay(2);
//   }

//   gyroX_offset = sum / 1000.0;
//   Serial.print("Gyro X Offset: ");
//   Serial.println(gyroX_offset);
// }
double accY_offset = 0;
double accZ_offset = 0;

// void calibrateAccel() {
//   long ay=0,az=0;
//   for (int i=0; i<500; i++) {
//     sensors_event_t a,g,temp;
//     mpu.getEvent(&a,&g,&temp);
//     ay+=a.acceleration.y;
//     az+=a.acceleration.z;
//     delay(2);
//   }
//   accY_offset=ay / 500.0;
//   accZ_offset=(az/500.0)-9.81;  // remove gravity
//   Serial.println("offaccy");
//   Serial.println(accY_offset);
//   Serial.println("offsetz");
//   Serial.println(accZ_offset);
// }
void encodeR(){
  if (prevA_right==LOW) {     
    if (digitalRead(encodPinBR)==HIGH) wheel_pulse_count_right++;
    else wheel_pulse_count_right--;
  }
  prevA_right=HIGH;
}
void encodeL(){
  if (prevA_left==LOW) {     
    if (digitalRead(encodPinBL)==HIGH) wheel_pulse_count_left++;
    else wheel_pulse_count_left--;
  }
  prevA_left=HIGH;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // if (!mpu.begin()) {
  //   Serial.println("MPU6050 not found!");
  //   while (1)
  //     ;
  // }

  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  //delay(1000);
  // calibrateGyro();
  // calibrateAccel();
  //Manipulator
  servo1.attach(10);
  servo2.attach(11);
  servo1.write(35);
  servo2.write(90);
  //encoder
  pinMode(encodPinAR, INPUT_PULLUP);
  pinMode(encodPinBR, INPUT_PULLUP);
  pinMode(encodPinAL, INPUT_PULLUP);
  pinMode(encodPinBL, INPUT_PULLUP);
  prevA_right=digitalRead(encodPinAR);
  prevA_left=digitalRead(encodPinAL);
  // attachInterrupt(digitalPinToInterrupt(encodPinAR),encodeR,RISING);
  // attachInterrupt(digitalPinToInterrupt(encodPinAL),encodeL,RISING);
  //Timer
  // Serial.println(preLoader);
  Serial.println(preLoader);
  noInterrupts();
  TCCR1A=0;
  TCCR1B=0;
  TCCR1B|=(1<<CS12)|(1<<CS10);
  TCNT1=preLoader;
  TIMSK1|=(1<<TOIE1);
  interrupts();
  byte status = mpu.begin();
  //mpu.calcOffsets();
  Serial.println("System Ready");
}


void loop() {
  //commands from UI

  // if(Serial.available()){
  //   char val=Serial.read();
  //   servoGrip(val);
  //   servoArm(val);
  // }
  //bluethooth tuning
  mpu.update();
//   int aR = digitalRead(encodPinAR);
//   if (prevA_right == LOW && aR == HIGH) {     
//     if (digitalRead(encodPinBR) == HIGH) wheel_pulse_count_right++;
//     else wheel_pulse_count_right--;
//   }
//   prevA_right = aR;
//  int aL = digitalRead(encodPinAL);
//   if (prevA_left == LOW && aL == HIGH) {     
//     if (digitalRead(encodPinBL) == HIGH) wheel_pulse_count_left++;
//     else wheel_pulse_count_left--;
//   }
//   prevA_left = aL;
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {  // EOL
      inputBuffer[idx] = '\0';     // Terminate string
      idx = 0;
      
      Serial.println(inputBuffer[0]);
      processInput(inputBuffer);

    } else {
      if (idx < MAX_INPUT - 1) {
        inputBuffer[idx++] = c;
      }
    }
  }

//   //encoder counting
//   prevA_right= digitalRead(encodPinAR);
//   prevA_left= digitalRead(encodPinAL);
  
  
  if ((millis()-lastTime)>=1/fs*1000) {
    //Serial.println(millis()-lastTime);
    lastTime=millis();
    // POS=(-wheel_pulse_count_right+wheel_pulse_count_left)/2;
    // sensors_event_t acc, gyro, temp;
    // mpu.getEvent(&acc, &gyro, &temp);
    // // ---- Accelerometer angle (pitch) ----
    // double accAngle = atan2(acc.acceleration.y,acc.acceleration.z-1.7);
   
    
    // // ---- Low-pass filter ----
    // double accFiltered = (1 - alpha) * accAngle + alpha * prevAccFiltered;
    // prevAccFiltered = accFiltered;

    // // ---- Gyro rate (offset corrected) ----
    // double gyroRate = gyro.gyro.x +0.04;

    // // ---- Complementary filter ----
    // angle = (1 - alpha) * (angle + gyroRate * dt) + alpha * accFiltered;
    // // if (abs(angle) > 95) {
    // //   setMotor(0);
    // //   return;
    // // }
    // //float time=millis()/1000;
    // // ---- PID Tilt ----
    // errorTilt = 0 -(angle*57.2958+0.8);
    errorTilt=0-mpu.getAngleX()-2;
    integralTilt += errorTilt*dt;
   
    int sw=0;
    // if((abs(errorTilt)-abs(prevErrorTilt))<0){
    //   sw=false;
    // }
    // if(count==0){
    //   count=1;
    //   if(errorTilt<0){
    //     isPositive=!isPositive;
    //   }
    // }
    // else if(isPositive!=(errorTilt>0)){
      
    //   integralTilt=abs((errorTilt*dt)*((errorTilt*dt)-1)*0.1/2);
    //   if(errorTilt<0){
    //     integralTilt=-integralTilt;
    //   }
    //   isPositive=!isPositive;
    //   lastTime=millis();
    // }
    // int factor=1;
    // if(errorTilt<0){
    //   factor=-1;
    // }
    // if(abs(errorTilt - prevErrorTilt)>1.7){
    //   sw=1;
    //   prevErrorTilt = errorTilt;
    // }
    integralTilt = constrain(integralTilt, -80/Ki, 80/Ki);
    float propError=constrain(errorTilt,-100/Kp,100/Kp);
    float velocityTilt=(errorTilt - prevErrorTilt) / dt;
    double uTilt = Kp*propError + Ki * integralTilt + Kd * velocityTilt+Ka*(int)(velocityTilt-prevVelTilt)/dt;
    prevVelTilt=velocityTilt;
    // Serial.print(angle * 57.2958+0.8,1);
    // // Serial.print(" | ");
    // Serial.print(",");
    // Serial.print(uTilt,1);
    // Serial.print(",");
    // //Serial.println(uPOS);
    // Serial.println(Ki*integralTilt,1);
    uTilt = constrain(uTilt, -255, 255);
    
    // if(abs(errorTilt)>4.5 ){
      
    //   if(angle<0){
    //     for(int i=0;i<170;i+=50){
    //       uTilt=Kp* errorTilt+i;
    //       setMotor(uTilt);
    //       delay(25)
    //     }
        
    //   }
    //   else{
    //     for(int i=0;i<170;i+=50){
    //       uTilt=Kp* errorTilt-i;
    //       setMotor(uTilt);
    //       delay(25);
          
    //     }
        
    //   }
    //   Serial.println("activted");
      
    // }
    //Serial.print(Kp*errorTilt);
    // Serial.print(", ");
    // Serial.print(Ki*integralTilt);
    // Serial.print(", ");
    //Serial.println((errorTilt-prevErrorTilt)/dt);
    // Serial.print(" | ");
   
    //POS pid
    errorPOS = 0-POS;
    integralPOS += errorPOS * dt;
    integralPOS = constrain(integralPOS, -55/Ki_p, 55/Ki_p);
    double uPOS = Kp_p * errorPOS + Ki_p * integralPOS + Kd_p * (errorPOS - prevErrorPOS) / dt;
    prevErrorPOS = errorPOS;
    double uSys = uTilt + uPOS;
    if(uSys>0){
      uSys+=20;
    }
    else if(uSys<0) {
      uSys-=20;
    }

    setMotor(uSys);

    // ---- Debug ----
    Serial.print("Angle: ");
    Serial.print(uSys);
    //Serial.print(",");
    
    //Serial.print(",");
    Serial.println(errorTilt);
    //snprintf(data,sizeof(data),"%.4f,%.4f,%.4f,%.4f,%.4f",time,angle,uSys,Ki*integralTilt,Kd*velocityTilt);
    //Serial.println("06.123,-02.456,0.123");
    flag=false;
    
  }
  
  
}
