# Animaciones en Matriz LED MAX7219 con Arduino

Este proyecto demuestra cómo mostrar dibujos personalizados (mapas de bits) y animaciones básicas en un módulo de matriz de LED 8x8 controlado por el chip MAX7219, utilizando la potencia de la librería Adafruit GFX.

## 🚀 Funcionalidades
El código principal alterna entre dos imágenes diseñadas a medida mediante una cuadrícula binaria:
- Un corazón ❤️
- Una carita feliz 🙂

## 🛠️ Hardware Necesario
- 1x Placa Arduino (Uno, Nano, Mega) o ESP32.
- 1x Módulo Matriz LED 8x8 con MAX7219.
- Cables jumper.

## 🔌 Conexiones (Esquema SPI)

Aquí tienes la lista de pines para conectar la matriz a un ESP32:

* **VCC** de la matriz ---> **5V** (o VIN) del ESP32
* **GND** de la matriz ---> **GND** del ESP32
* **DIN** de la matriz ---> **Pin 23** del ESP32
* **CS** de la matriz ---> **Pin 5** del ESP32
* **CLK** de la matriz ---> **Pin 18** del ESP32

## 📚 Librerías Requeridas
Para que este código compile correctamente, debes instalar las siguientes librerías desde el *Gestor de Librerías* del IDE de Arduino:
1. `Adafruit GFX Library`
2. `Max72xxPanel`

## 💡 Cómo orientar la matriz
Dependiendo del fabricante de tu módulo, es posible que las imágenes salgan giradas 90 grados. No es necesario modificar el cableado, simplemente cambia el parámetro en la función `setup()`:

```cpp
matrix.setRotation(0); // Prueba con 1, 2 o 3 hasta que quede derecho.
