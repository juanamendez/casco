#include <Wire.h>
#include <math.h>
#include <SPI.h>

// --- PINES MATRICES ---
#define CS_IZQ_PIN 5    
#define CS_DER_PIN 17   

// --- PINES LEDs Y BUZZER ---
#define LED_IZQ_PIN 26
#define LED_DER_PIN 27
#define BUZZER_PIN 14   

#define MMA845x_ADDRESS 0x1C 

// --- CONFIGURACIÓN DE SENSIBILIDAD ---
int umbralGiro = 550;           
int umbralAdelante = 300;       
unsigned long duracionSinal = 5000; 

// Variables de control
int16_t x_referencia = 0;        
int16_t z_referencia = 0;        
unsigned long tiempoInicio = 0;
int estadoActual = 0;           // 0: Recto, 1: Izquierda, 2: Derecha

// --- DIBUJOS ---
byte f_izq[8] = {0x08,0x0C,0x0E,0xFF,0xFF,0x0E,0x0C,0x08}; 
byte f_der[8] = {0x08,0x0C,0x0E,0xFF,0xFF,0x0E,0x0C,0x08}; 
byte f_rec[8] = {0x00,0x18,0x3C,0x7E,0x7E,0x3C,0x18,0x00}; 
byte vacio[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

void enviarDato(int csPin, byte reg, byte val) {
  digitalWrite(csPin, LOW);
  SPI.transfer(reg);
  SPI.transfer(val);
  digitalWrite(csPin, HIGH);
}

void iniciarMatriz(int csPin) {
  pinMode(csPin, OUTPUT);
  enviarDato(csPin, 0x0F, 0x00); 
  enviarDato(csPin, 0x09, 0x00); 
  enviarDato(csPin, 0x0A, 0x03); 
  enviarDato(csPin, 0x0B, 0x07); 
  enviarDato(csPin, 0x0C, 0x01); 
  for(int i=1; i<=8; i++) enviarDato(csPin, i, 0x00); 
}

void dibujar(int csPin, byte img[]) {
  for (int i = 0; i < 8; i++) {
    enviarDato(csPin, i + 1, img[i]);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  
  // Iniciar pines de LEDs y Buzzer
  pinMode(LED_IZQ_PIN, OUTPUT);
  pinMode(LED_DER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Apagar todo al inicio por seguridad
  digitalWrite(LED_IZQ_PIN, LOW);
  digitalWrite(LED_DER_PIN, LOW);
  noTone(BUZZER_PIN); 
  
  SPI.begin(); 
  iniciarMatriz(CS_IZQ_PIN);
  iniciarMatriz(CS_DER_PIN);
  
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x2A); Wire.write(0x01);
  Wire.endTransmission();
  
  delay(1000); 
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  x_referencia = x; 
  z_referencia = z; 
  Serial.println("SISTEMA CALIBRADO");
}

void loop() {
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  
  int difZ = z - z_referencia; 
  int difX = x - x_referencia; 
  unsigned long ahora = millis();

  // 1. CANCELACIÓN POR CABEZA HACIA ADELANTE
  if (difX > umbralAdelante) {
    estadoActual = 0;
  }

  // 2. DETECCIÓN Y CANCELACIÓN POR CONTRAMOVIMIENTO
  if (difZ < -umbralGiro) { 
    if (estadoActual != 1) { 
      estadoActual = 1;
      tiempoInicio = ahora;
    }
  } 
  else if (difZ > umbralGiro) {
    if (estadoActual != 2) {
      estadoActual = 2;
      tiempoInicio = ahora;
    }
  }

  // 3. LÓGICA DE TIEMPO, TITILEO MATRICES, LEDS Y BUZZER
  if (estadoActual != 0) {
    if (ahora - tiempoInicio > duracionSinal && abs(difZ) < umbralGiro) {
      estadoActual = 0;
    } else {
      bool encendido = (millis() / 300) % 2; 
      
      // --- EFECTO TIC-TOC MÁS GRAVE ---
      unsigned long tiempoFase = millis() % 300;
      
      if (tiempoFase < 50) {
        if (encendido) {
          tone(BUZZER_PIN, 1200);  // "TIC" (Tono medio-grave)
        } else {
          tone(BUZZER_PIN, 600);   // "TOC" (Tono bien grave)
        }
      } else {
        noTone(BUZZER_PIN);        
      }
      // ---------------------------------------
      
      if (estadoActual == 1) {
        // GIRO IZQUIERDA
        dibujar(CS_IZQ_PIN, encendido ? f_izq : vacio);
        digitalWrite(LED_IZQ_PIN, encendido ? HIGH : LOW);
        
        dibujar(CS_DER_PIN, vacio);
        digitalWrite(LED_DER_PIN, LOW);
      } 
      else if (estadoActual == 2) {
        // GIRO DERECHA
        dibujar(CS_DER_PIN, encendido ? f_der : vacio);
        digitalWrite(LED_DER_PIN, encendido ? HIGH : LOW);
        
        dibujar(CS_IZQ_PIN, vacio);
        digitalWrite(LED_IZQ_PIN, LOW);
      }
    }
  } 
  else {
    // CENTRO/NEUTRO
    dibujar(CS_IZQ_PIN, f_rec); 
    dibujar(CS_DER_PIN, f_rec);
    
    digitalWrite(LED_IZQ_PIN, LOW);
    digitalWrite(LED_DER_PIN, LOW);
    noTone(BUZZER_PIN); 
  }

  delay(40);
}

void leerSensor(int16_t* x, int16_t* y, int16_t* z) {
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x01);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MMA845x_ADDRESS, (uint8_t)6);
  if (Wire.available() >= 6) {
    *x = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
    *y = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
    *z = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
  }
}