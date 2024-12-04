#include <Servo.h> 


const int trigPin = 7;
const int echoPin = 6;
const int pirPin = 8;


const int ledPin = 10;
const int buzzerPin = 11;
const int servoPin = 12;


const int SIGNAL_PIN = 2;


long duration;
int distance;


bool pirState = false;
bool modoSeguro = false; 


Servo servo;

void setup() {
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(SIGNAL_PIN, INPUT); 

  
  servo.attach(servoPin);
  servo.write(0); 

  
  Serial.begin(9600);
  Serial.println("Sistema listo...");
}

void loop() {
  if (digitalRead(SIGNAL_PIN) == HIGH) {
    Serial.println("Señal recibida desde el ESP32. Reiniciando sistema...");
    desactivarModoSeguro(); 
    delay(500); 
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 10 && !modoSeguro) {
    activarModoSeguro("Sensor Ultrasónico");
  }

  pirState = digitalRead(pirPin);
  if (pirState && !modoSeguro) {
    activarModoSeguro("Sensor PIR");
  }

 
  if (modoSeguro) {
    digitalWrite(ledPin, HIGH);  
    tone(buzzerPin, 1000);      
  }

  delay(500); 
}


void activarModoSeguro(String sensor) {
  modoSeguro = true; 
  Serial.println("Intrusión detectada por: " + sensor);

  // Activar alarmas
  digitalWrite(ledPin, HIGH);   
  tone(buzzerPin, 1000);        
  servo.write(90);              

  Serial.println("Modo seguro activado. Alarmas encendidas.");
}


void desactivarModoSeguro() {
  modoSeguro = false; 
  Serial.println("Modo seguro desactivado. Alarmas apagadas.");

  // Apagar alarmas
  digitalWrite(ledPin, LOW);    
  noTone(buzzerPin);            
  servo.write(0);               
}
