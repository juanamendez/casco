#include <Wire.h>
#include <math.h>
#include <SPI.h>

// --- PINES MATRIZ ---
#define CS_PIN 5
#define MMA845x_ADDRESS 0x1C 

// --- CONFIGURACIÓN DE SENSIBILIDAD ---
int umbralGiro = 550;           // Valor alto para que el movimiento sea BIEN marcado [cite: 513, 519]
int umbralAdelante = 600;       // Sensibilidad para detectar cabeza hacia adelante [cite: 513, 522]
unsigned long duracionSinal = 5000; // 5 segundos [cite: 490, 809]

// Variables de control
int16_t x_referencia = 0;       
int16_t z_referencia = 0;       
unsigned long tiempoInicio = 0;
int estadoActual = 0;           // 0: Recto, 1: Izquierda, 2: Derecha

// Dibujos
byte f_der[8] = {0x08,0x0C,0x0E,0xFF,0xFF,0x0E,0x0C,0x08};
byte f_izq[8] = {0x10,0x30,0x70,0xFF,0xFF,0x70,0x30,0x10};
byte f_rec[8] = {0x00,0x18,0x3C,0x7E,0x7E,0x3C,0x18,0x00};
byte vacio[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // Para el titileo

void enviarDato(byte reg, byte val) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg);
  SPI.transfer(val);
  digitalWrite(CS_PIN, HIGH);
}

void iniciarMatriz() {
  pinMode(CS_PIN, OUTPUT);
  SPI.begin();
  enviarDato(0x0F, 0x00); // Test mode OFF [cite: 405, 453]
  enviarDato(0x09, 0x00); 
  enviarDato(0x0A, 0x03); 
  enviarDato(0x0B, 0x07); 
  enviarDato(0x0C, 0x01); 
  for(int i=1; i<=8; i++) enviarDato(i, 0x00); 
}

void dibujar(byte img[]) {
  for (int i = 0; i < 8; i++) {
    enviarDato(i + 1, img[i]);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Pines I2C para ESP32-S [cite: 77, 261, 262]
  iniciarMatriz();
  
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x2A); Wire.write(0x01);
  Wire.endTransmission();
  
  delay(1000); 
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  x_referencia = x; // Guardamos el neutro para adelante/atrás [cite: 523, 524]
  z_referencia = z; // Guardamos el neutro para lateral [cite: 524]
  Serial.println("SISTEMA CALIBRADO");
}

void loop() {
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  
  int difZ = z - z_referencia; 
  int difX = x - x_referencia; 
  unsigned long ahora = millis();

  // 1. CANCELACIÓN POR CABEZA HACIA ADELANTE
  // Si agachas la cabeza (mirar manubrio), se apaga la flecha 
  if (difX > umbralAdelante) {
    estadoActual = 0;
  }

  // 2. DETECCIÓN Y CANCELACIÓN POR CONTRAMOVIMIENTO
  // Cambiamos los signos (< y >) para invertir Izq/Der 
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

  // 3. LÓGICA DE TIEMPO Y TITILEO
  if (estadoActual != 0) {
    // Si pasaron los 5 seg Y volvimos al centro lateral
    if (ahora - tiempoInicio > duracionSinal && difZ < umbralGiro) {
      estadoActual = 0;
    } else {
      // EFECTO TITILEO: Parpadea cada 300ms 
      bool encendido = (millis() / 300) % 2; 
      if (encendido) {
        dibujar(estadoActual == 1 ? f_izq : f_der);
      } else {
        dibujar(vacio);
      }
    }
  } 
  else {
    dibujar(f_rec); // Símbolo de centro [cite: 507, 360]
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
