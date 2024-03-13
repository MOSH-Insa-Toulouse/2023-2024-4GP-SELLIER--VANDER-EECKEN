#include <SoftwareSerial.h>
#define rxPin 7 //Broche 7 en tant que RX, raccorder sur TX du HC-05
#define txPin 6 //Broche 6 en tant que RX, raccorder sur TX du HC-05
#define baudrate 9600
SoftwareSerial mySerial(rxPin ,txPin); //Définition du software serial

void setup(){
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
    
  mySerial.begin(baudrate);
  Serial.begin(baudrate);
}

void loop(){
    int i = 0; 
	char someChar[32] ={0};
	//when characters arrive over the serial port...

	while (Serial.available()) {
	   do{
		someChar[i++] = Serial.read();
		delay(3);		
	   }while (Serial.available() > 0);
	   
	   mySerial.println(someChar); 
	   Serial.println(someChar); 
	}
	while (mySerial.available()) {
		Serial.print((char)mySerial.read());
	}
}