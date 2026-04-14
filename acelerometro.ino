#include <Wire.h>
#include <math.h> // <--- Agregamos esto para las funciones matemáticas

#define MMA845x_ADDRESS 0x1C 

int16_t z_referencia = 0;
float angulo_referencia = 0; // Para guardar el ángulo inicial
int umbral = 150; 

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 

  // 1. Despertar al sensor
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x2A); 
  Wire.write(0x01); 
  Wire.endTransmission();
  
  delay(1000); 

  // 2. CALIBRACIÓN
  Serial.println("Calibrando... quédate quieto");
  
  int16_t x, y, z;
  leerSensor(&x, &y, &z);
  
  z_referencia = z; 
  // Calculamos el ángulo inicial usando Z e Y (el plano de inclinación lateral)
  angulo_referencia = atan2((float)z, (float)y) * 180.0 / PI;
  
  Serial.print("Cero seteado. Angulo inicial: ");
  Serial.println(angulo_referencia);
}

void loop() {
  int16_t x, y, z;
  leerSensor(&x, &y, &z);

  // 1. Calculamos el ángulo actual
  float angulo_actual = atan2((float)z, (float)y) * 180.0 / PI;
  
  // 2. Calculamos la diferencia de ángulo respecto al cero
  float angulo_final = angulo_actual - angulo_referencia;

  // 3. Tu lógica original de diferencia cruda (para el umbral)
  int diferencia = z - z_referencia;

  // IMPRESIÓN
  Serial.print("Z: ");
  Serial.print(z);
  Serial.print(" | Angulo: ");
  Serial.print(angulo_final, 1); // Imprime con 1 decimal
  Serial.print("°");

  // Lógica de decisión
  if (diferencia > umbral) {
    Serial.println(" -> IZQUIERDA");
  } 
  else if (diferencia < -umbral) {
    Serial.println(" -> DERECHA");
  } 
  else {
    Serial.println(" -> RECTO");
  }

  delay(300); 
}

void leerSensor(int16_t* x, int16_t* y, int16_t* z) {
  Wire.beginTransmission(MMA845x_ADDRESS);
  Wire.write(0x01);
  Wire.endTransmission(false);
  Wire.requestFrom(MMA845x_ADDRESS, 6);

  if (Wire.available() >= 6) {
    *x = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
    *y = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
    *z = (int16_t)(Wire.read() << 8 | Wire.read()) >> 4;
  }
}