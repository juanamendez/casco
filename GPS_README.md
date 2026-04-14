# casco
# 🛰️ ESP32 GPS Tracker con Telegram Bot

Este proyecto utiliza un microcontrolador **ESP32** y un módulo GPS (como el Neo-6M) para obtener coordenadas de ubicación en tiempo real. Una vez que el dispositivo obtiene un "fix" satelital válido, envía automáticamente un enlace de Google Maps directamente a tu teléfono a través de un Bot de **Telegram**.

## 📌 Características
- **Conexión Inalámbrica:** Utiliza la conexión WiFi del ESP32 (ideal para conectarlo al hotspot de tu celular en exteriores).
- **Lectura GPS precisa:** Integración con la librería `TinyGPSPlus` para parsear sentencias NMEA.
- **Notificaciones Seguras:** Usa peticiones HTTPS seguras para comunicarse con la API oficial de Telegram.
- **Prevención de Spam:** Lógica implementada para enviar el mensaje de ubicación solo una vez tras obtener alta precisión satelital.

## 🛠️ Hardware Necesario
* Placa de desarrollo **ESP32** (NodeMCU u otra variante).
* Módulo GPS (Ej. **Ublox Neo-6M** o similar) con antena.
* Cables Jumper.

### 🔌 Esquema de Conexiones
| Módulo GPS | ESP32 | Notas |
| :--- | :--- | :--- |
| **VCC** | 5V / VIN | Recomendado 5V para mayor estabilidad del GPS. |
| **GND** | GND | |
| **TX** | GPIO 13 | Pin RX del ESP32 |
| **RX** | GPIO 12 | Pin TX del ESP32 |

## 💻 Configuración del Software

1. **Librerías:** Instala `TinyGPSPlus` desde el Gestor de Librerías de Arduino IDE.
2. **Crear el Bot de Telegram:** - Busca a `@BotFather` en Telegram.
   - Envía el comando `/newbot` y sigue las instrucciones.
   - Guarda el **Bot Token** generado.
3. **Obtener tu Chat ID:**
   - Busca a `@userinfobot` en Telegram y envíale un mensaje para obtener tu ID numérico.
4. **Configurar el código:**
   - Abre el archivo `.ino`.
   - Modifica las credenciales de tu red WiFi. (Para uso real en la calle, pon el nombre y contraseña del punto de acceso móvil de tu celular).
   - Sustituye `TU_BOT_TOKEN_AQUI` y `TU_CHAT_ID_AQUI` por tus datos reales.

## ⚠️ Advertencias y Buenas Prácticas
* **Seguridad:** Nunca subas tus credenciales de WiFi o tu Bot Token a un repositorio público. El código de este repositorio incluye placeholders.
* **Uso del GPS:** Para obtener la primera señal (Cold Start), el módulo GPS **debe estar al aire libre** o tener visión directa al cielo. En interiores no funcionará. Puede tardar entre 2 y 15 minutos en el primer encendido.

## 👨‍💻 Autor
Creado por [Tu Nombre / Tu Usuario de GitHub] - Estudiante de la UA.
<img width="1064" height="728" alt="image" src="https://github.com/user-attachments/assets/166cc5fc-d4b1-44ed-a49b-e8b65cd167ef" />

