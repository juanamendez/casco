# Lector de Presión y Temperatura con ESP32 y BMP180

Este proyecto utiliza un microcontrolador ESP32 y un sensor de presión barométrica y temperatura BMP180 para obtener lecturas ambientales en tiempo real, comunicándose a través del protocolo I2C.

## 🛠️ Hardware Necesario

* 1x Placa de desarrollo ESP32
* 1x Módulo sensor BMP180
* 4x Cables jumper (M-H o M-M dependiendo de tu conexión)

## 🔌 Conexiones (Cableado)

El sensor se comunica vía I2C. Los pines predeterminados para I2C en el ESP32 son el GPIO 21 (SDA) y GPIO 22 (SCL).

| Sensor BMP180 | Pin ESP32 | Descripción |
| :--- | :--- | :--- |
| **VIN / VCC** | **3V3** | Alimentación (¡Es fundamental usar 3.3V para no dañar el ESP32!) |
| **GND** | **GND** | Tierra |
| **SDA** | **GPIO 21** | Línea de datos I2C |
| **SCL** | **GPIO 22** | Línea de reloj I2C |

## 📚 Instalación de Librerías

El código requiere la librería oficial de SparkFun. Para evitar errores de dependencias en el gestor del IDE de Arduino, se recomienda la instalación manual mediante archivo ZIP:

1.  Descarga el archivo ZIP desde el [repositorio oficial de SparkFun BMP180 en GitHub](https://github.com/sparkfun/BMP180_Breakout_Arduino_Library).
2.  En el IDE de Arduino, ve a **Programa > Incluir Librería > Añadir biblioteca .ZIP...**
3.  Selecciona el archivo descargado.

## 💻 Código Fuente

El siguiente programa inicializa el sensor y realiza lecturas compensadas, mostrando los resultados en el Monitor Serie.

```cpp
// incluyo las librerias necesarias
#include <SFE_BMP180.h>
#include <Wire.h>

// creo una instancia del sensor
SFE_BMP180 bmp180;

void setup()
{
  Serial.begin(115200);

  // inicializo sensor
  if (bmp180.begin())
    Serial.println("BMP180 iniciado correctamentente");
  else
  {
    Serial.println("Error al iniciar el BMP180");
    while(1); // bucle infinito
  }
}

void loop()
{
  char status; 
  double T,P;

  status = bmp180.startTemperature();       // Inicio de lectura de temperatura
  if (status != 0)
  {   
    delay(500); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T);      // Obtener la temperatura
    if (status != 0)
    {
      status = bmp180.startPressure(3);     // Inicio lectura de presión (resolución máxima)
      if (status != 0)
      {        
        delay(500);                         //Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T);   // Obtenemos la presión compensada por temperatura
        if (status != 0)
        {                  
          Serial.print("Temperatura: ");
          Serial.print(T,2);
          Serial.print(" *C , ");
          Serial.print("Presion: ");
          Serial.print(P,2);
          Serial.println(" mb");          
        }      
      }      
    }   
  } 
  delay(1000);
}
