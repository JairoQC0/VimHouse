#include <SPI.h>
#include <MFRC522.h>
#include "BluetoothSerial.h"


#define RST_PIN  5    
#define SDA_PIN  4   

MFRC522 rfid(SDA_PIN, RST_PIN); 


BluetoothSerial SerialBT;
const String comandoDesactivar = "DESACTIVAR";

const byte UID_VALIDO[] = {0x33, 0x7A, 0xD9, 0x11};
const byte LONGITUD_UID = sizeof(UID_VALIDO);


#define OUTPUT_PIN 15

void setup() {

  SPI.begin(18, 19, 23); 
  rfid.PCD_Init();      


  SerialBT.begin("ESP32_Alarmas"); 
  Serial.begin(9600);              


  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW);

  Serial.println("ESP32 iniciado. Esperando tarjeta NFC o comando Bluetooth...");
}

void loop() {

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (esTarjetaValida()) {
      Serial.println("Tarjeta válida. Enviando señal al Arduino...");
      enviarSenalArduino();
      delay(1000); 
    } else {
      Serial.println("Tarjeta no válida.");
    }
    rfid.PICC_HaltA();
  }


  if (SerialBT.available()) {
    String comando = SerialBT.readStringUntil('\n');
    comando.trim(); 
    Serial.println("Comando recibido: " + comando);

    if (comando.equalsIgnoreCase(comandoDesactivar)) {
      Serial.println("Comando Bluetooth válido. Enviando señal al Arduino...");
      enviarSenalArduino();
    } else {
      Serial.println("Comando no reconocido.");
    }
  }
}


bool esTarjetaValida() {
  if (rfid.uid.size != LONGITUD_UID) return false;

  for (byte i = 0; i < LONGITUD_UID; i++) {
    if (rfid.uid.uidByte[i] != UID_VALIDO[i]) {
      return false;
    }
  }
  return true;
}


void enviarSenalArduino() {
  digitalWrite(OUTPUT_PIN, HIGH); 
  delay(500);                   
  digitalWrite(OUTPUT_PIN, LOW); 
}
