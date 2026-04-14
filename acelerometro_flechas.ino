#include <Wire.h>
#include <math.h>
#include <SPI.h> // Usamos la librería nativa del ESP32

// --- PINES MATRIZ ---
#define CS_PIN 5
#define MMA845x_ADDRESS 0x1C 

// --- CONFIGURACIÓN DE SENSIBILIDAD ---
int umbral = 500;               // Subí a 500 para que el movimiento sea BIEN marcado
unsigned long duracionSinal = 5000; // 5 segundos

// Variables de control
int16_t z_referencia = 0;
unsigned long tiempoInicio = 0;
int estadoActual = 0; // 0: Recto, 1: Izquierda, 2: Derecha

// Dibujos de las flechas
byte f_der[8] = {0x08,0x0C,0x0E,0xFF,0xFF,0x0E,0x0C,0x08};
byte f_izq[8] = {0x10,0x30,0x70,0xFF,0xFF,0x70,0x30,0x10};
byte f_rec[8] = {0x00,0x18,0x3C,0x7E,0x7E,0x3C,0x18,0x00};

// Función para hablar con la MAX7219 (Sustituye a LedControl)
void enviarDato(byte reg, byte val) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg);
  SPI.transfer(val);
  digitalWrite(CS_PIN, HIGH);
}

void iniciarMatriz() {
  pinMode(CS_PIN, OUTPUT);
  SPI.begin();
  enviarDato(0x0F, 0x00); // Test mode OFF (Esto evita el bloque rojo)
  enviarDato(0x09, 0x00); // Decode mode: none
  enviarDato(0x0A, 0x03); // Intensidad (0-15)
  enviarDato(0x0B, 0x07); // Scan limit: 8 columnas
  enviarDato(0x0C, 0x01); // Shutdown mode: Normal
  for(int i=1; i<=8; i++) enviarDato(i, 0x00); // Limpiar pantalla
}

void dibujar(byte img[]) {
  for (int i = 0; i < 8; i++) {
    enviarDato(i + 1, img[i]);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  iniciarMatriz();
  
  // Despertar sensor
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x2A); Wire.write(0x01);
  Wire.endTransmission();
  
  delay(1000); 
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  z_referencia = z;
  Serial.println("SISTEMA CALIBRADO");
}

void loop() {
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  int diferencia = z - z_referencia;
  unsigned long ahora = millis();

  // 1. DETECCIÓN Y CANCELACIÓN POR CONTRAMOVIMIENTO
  // Si inclino fuerte a un lado, reseteo el tiempo y cambio el estado
  if (diferencia > umbral) {
    if (estadoActual != 1) { // Si no era Izquierda, ahora lo es
      estadoActual = 1;
      tiempoInicio = ahora;
    }
  } 
  else if (diferencia < -umbral) {
    if (estadoActual != 2) { // Si no era Derecha, ahora lo es
      estadoActual = 2;
      tiempoInicio = ahora;
    }
  }

  // 2. LÓGICA DE TIEMPO (5 SEGUNDOS)
  if (estadoActual != 0) {
    // Si ya pasaron los 5 segundos Y volvimos al centro
    if (ahora - tiempoInicio > duracionSinal && abs(diferencia) < umbral) {
      estadoActual = 0;
    } else {
      // Mientras esté activo el giro, mostramos la flecha
      dibujar(estadoActual == 1 ? f_izq : f_der);
    }
  } 
  else {
    dibujar(f_rec); // Estado por defecto
  }

  delay(50);
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