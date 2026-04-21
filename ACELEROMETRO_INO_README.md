## 🚀 Características
* [cite_start]**Señalización Automática**: Detecta la inclinación de la cabeza para activar flechas de giro (Izquierda/Derecha)[cite: 474, 552].
* [cite_start]**Temporizador de Seguridad**: La señal se mantiene activa durante **5 segundos** de forma automática[cite: 710, 809].
* [cite_start]**Cancelación por Contramovimiento**: Si se activa un giro pero el usuario inclina la cabeza hacia el lado opuesto, la señal anterior se cancela y comienza la nueva inmediatamente[cite: 746, 810].
* [cite_start]**Baja Sensibilidad**: El umbral está ajustado para requerir un movimiento pronunciado, evitando activaciones accidentales por baches o movimientos leves[cite: 713, 808].
* [cite_start]**Calibración Automática**: Al encender el dispositivo, el sistema toma la posición actual como el "punto cero" o posición recta[cite: 508, 767].

## 🛠️ Conexiones

[cite_start]Para asegurar la compatibilidad con el ESP32-S (lógica de 3.3V), sigue este esquema de cableado[cite: 649]:

### Acelerómetro MMA845x (I2C)
| Pin Sensor | Pin ESP32-S | Función |
| :--- | :--- | :--- |
| **VCC** | **3.3V** | [cite_start]Alimentación (¡No usar 5V!) [cite: 631, 635] |
| **GND** | **GND** | [cite_start]Tierra común [cite: 631] |
| **SCL** | **GPIO 22** | [cite_start]Reloj I2C [cite: 632, 642] |
| **SDA** | **GPIO 21** | [cite_start]Datos I2C [cite: 633, 642] |

### Matriz LED 8x8 MAX7219 (SPI)
| Pin Matriz | Pin ESP32-S | Función |
| :--- | :--- | :--- |
| **VCC** | **5V / Vin** | [cite_start]Alimentación de la matriz [cite: 615, 651] |
| **GND** | **GND** | [cite_start]Tierra común [cite: 615] |
| **DIN** | **GPIO 23** | [cite_start]Entrada de datos (MOSI) [cite: 616, 642] |
| **CS** | **GPIO 5** | [cite_start]Carga de datos (Chip Select) [cite: 616, 642] |
| **CLK** | **GPIO 18** | [cite_start]Reloj SPI [cite: 617, 642] |

## ⚙️ Explicación del Código

[cite_start]El programa utiliza la librería nativa **SPI.h** en lugar de `LedControl` para evitar errores de compilación comunes en arquitecturas modernas como la del ESP32[cite: 697, 820].

### Lógica de Funcionamiento:
1.  [cite_start]**Arranque y Calibración**: El sensor se activa y realiza una lectura inicial del eje Z. Este valor se guarda como `z_referencia`, permitiendo que el sistema funcione correctamente sin importar si el sensor quedó levemente torcido al pegarlo al casco[cite: 566, 568].
2.  [cite_start]**Cálculo de Inclinación**: Se utiliza la función `atan2` para convertir los valores de aceleración en grados reales de inclinación[cite: 587, 591].
3.  **Detección de Giro**: 
    * [cite_start]Si la diferencia entre la lectura actual y la referencia supera el **umbral (500)**, se activa el estado de giro[cite: 808, 825].
    * [cite_start]Se inicia un cronómetro usando la función `millis()` para contar 5 segundos[cite: 715, 809].
4.  [cite_start]**Prioridad de Cambio**: Si el sistema detecta una inclinación hacia el lado contrario mientras una flecha está activa, el cronómetro se reinicia y la matriz cambia el dibujo al instante para reflejar el nuevo movimiento[cite: 764, 827].

## 📚 Librerías Necesarias
* `Wire.h` (Nativa de Arduino para I2C).
* [cite_start]`math.h` (Nativa para cálculos trigonométricos)[cite: 589].
* [cite_start]`SPI.h` (Nativa para comunicación con la matriz LED)[cite: 702, 820].

