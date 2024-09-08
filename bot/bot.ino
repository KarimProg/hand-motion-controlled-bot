#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

struct values{
  float pitch = 0;
  float roll = 0;
};

int max=1, min=1;
int counter=0;

#define enA 6
#define enB 3
#define in1 7
#define in2 5
#define in3 4
#define in4 2

values angles;

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
float motor_left=0, motor_right=0;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    counter = 0;
    radio.read(&angles, sizeof(values));
    // if(abs(angles.roll<15)){
    //   motor_left=motor_right=-angles.pitch*2;
    // }
    
    motor_left = (-angles.pitch + angles.roll*0.5)*2;
    motor_right = (-angles.pitch - angles.roll*0.5)*2;
    
  }

  scaleValues();

  if(motor_left>0){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else{
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  analogWrite(enA, abs(motor_left));

  if(motor_right>0){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
  else{
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
  analogWrite(enB, (int)abs(motor_right)*0.8);

  Serial.println(motor_right);
  Serial.println(motor_left);

  if(!radio.available())
    counter++;
    if (counter>20){
      motor_left=0;
      motor_right=0;
    }

}

void scaleValues() {
  if(motor_left>255 || motor_right>255){
    max = max(motor_left, motor_right);
    motor_left = map(motor_left, -max, max, -255, 255);
    motor_right = map(motor_right, -max, max, -255, 255);
  }
  else if(motor_left<-255 || motor_right<-255){
    min = min(motor_left, motor_right);
    motor_left = map(motor_left, -min, min, -255, 255);
    motor_right = map(motor_right, -min, min, -255, 255);
  }
}