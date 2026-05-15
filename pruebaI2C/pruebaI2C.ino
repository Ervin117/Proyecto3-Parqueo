/*************************************************************************************************
  ESP32 Web Server - Maestro I2C con Debug
**************************************************************************************************/
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// --- DECLARACIÓN DE FUNCIONES ---
void i2cScanner();
void handle_OnConnect();
void handle_led1on();
void handle_led1off();
void handle_NotFound();
String SendHTML();
String generarHTMLSlot(String id, bool ocupado);

// --- VARIABLES GLOBALES ---
#define I2CSlaveAddress1 0x18 // Para la nucleo con pantalla
#define I2CSlaveAddress2 0x27 // Para la nucleo con 7SEG

#define I2C_SDA 21
#define I2C_SCL 22
uint8_t error = 0;  

unsigned long previousMillis = 0;
const long interval = 500; // Tiempo entre cada comunicación I2C

// --- WIFI ---
const char* ssid = "HAB309";  // Enter your SSID here
const char* password = "36266906";  //Enter your Password here

WebServer server(80);  

uint8_t LED1pin = 4;
bool LED1status = LOW;

bool A1_status = LOW;
bool A2_status = LOW;
bool A3_status = LOW;
bool A4_status = LOW;

bool B1_status = LOW;
bool B2_status = LOW;
bool B3_status = LOW;
bool B4_status = LOW;

bool H_status = LOW;

Servo miServo;
int pinServo = 18; 

//************************************************************************************************
// SETUP
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando Sistema...");

  // Iniciar I2C
  Wire.begin(I2C_SDA, I2C_SCL); 
  i2cScanner(); 

  pinMode(LED1pin, OUTPUT);

  // Conexión WiFi
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado exitosamente!");
  Serial.print("Abre tu navegador en la IP: ");
  Serial.println(WiFi.localIP());  
 
  // Configuración Web Server
  server.on("/", handle_OnConnect); 
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  // Configuración Servo
  ESP32PWM::allocateTimer(0); 
  miServo.setPeriodHertz(50);    
  miServo.attach(pinServo, 500, 2400); 
  miServo.write(0); 
  
  Serial.println("\n--- INICIANDO COMUNICACION I2C ---");
}

//************************************************************************************************
// LOOP PRINCIPAL
//************************************************************************************************
void loop() {
  server.handleClient(); // Atiende las peticiones web
  
  // Aplicar el estado al LED físico
  digitalWrite(LED1pin, LED1status);

  // Lógica para interpretar datos del UART (PC -> ESP32)
  if (Serial.available() > 0) {
    char numero_recibido = Serial.read(); 
    if (numero_recibido == 'a') { A1_status = HIGH; Serial.println("*** Espacio A1 Ocupado ***"); } 
    else if (numero_recibido == 'b') { A2_status = HIGH; Serial.println("*** Espacio A2 Ocupado ***"); } 
    else if (numero_recibido == 'c') { A3_status = HIGH; Serial.println("*** Espacio A3 Ocupado ***"); } 
    else if (numero_recibido == 'd') { A4_status = HIGH; Serial.println("*** Espacio A4 Ocupado ***"); }
    else if (numero_recibido == 'e') { B1_status = HIGH; Serial.println("*** Espacio B1 Ocupado (Simulado) ***"); } 
    else if (numero_recibido == 'f') { B2_status = HIGH; Serial.println("*** Espacio B2 Ocupado (Simulado) ***"); } 
    else if (numero_recibido == 'g') { B3_status = HIGH; Serial.println("*** Espacio B3 Ocupado (Simulado) ***"); } 
    else if (numero_recibido == 'h') { B4_status = HIGH; Serial.println("*** Espacio B4 Ocupado (Simulado) ***"); }
    else if (numero_recibido == 'x') { H_status = HIGH; Serial.println("*** Helipuerto ocupado ***"); }
    else if (numero_recibido == 'r') {
      A1_status = A2_status = A3_status = A4_status = LOW;
      B1_status = B2_status = B3_status = B4_status = LOW;
      H_status = LOW;
      Serial.println("*** Sistema Reseteado (Todos Libres) ***");
    }
  }

  // --- LÓGICA DE COMUNICACIÓN I2C (cada 500 ms) ---
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // DEBUG: Mostrar lo que el ESP32 va a enviar al STM32
    Serial.print(">> TX a STM32 (A1-A4): [");
    Serial.print(A1_status); Serial.print("] [");
    Serial.print(A2_status); Serial.print("] [");
    Serial.print(A3_status); Serial.print("] [");
    Serial.print(A4_status); Serial.println("]");

    // 1. ENVIAR DATOS (NIVEL A) AL STM32
    Wire.beginTransmission(I2CSlaveAddress2); 
    Wire.write(A1_status);
    Wire.write(A2_status);
    Wire.write(A3_status);
    Wire.write(A4_status);
    error = Wire.endTransmission(); 

    if(error == 0) { 
      // 2. PEDIR DATOS (NIVEL B) AL STM32
      uint8_t bytesReceived = Wire.requestFrom((uint16_t)I2CSlaveAddress2, (uint8_t)4); 
      
      if (bytesReceived == 4) {
        // Leer los 4 bytes 
        B1_status = Wire.read();
        B2_status = Wire.read();
        B3_status = Wire.read();
        B4_status = Wire.read();

        // DEBUG: Mostrar lo que el STM32 le respondió al ESP32
        Serial.print("<< RX de STM32 (B1-B4): [");
        Serial.print(B1_status); Serial.print("] [");
        Serial.print(B2_status); Serial.print("] [");
        Serial.print(B3_status); Serial.print("] [");
        Serial.print(B4_status); Serial.println("]");
        Serial.println("----------------------------------------");

      } else {
        Serial.println("Error I2C RX: No se recibieron 4 bytes del STM32"); 
      }
    } else {
      Serial.print("Error I2C TX al STM32. Codigo de error: ");
      Serial.println(error);
    }
  }
}

//************************************************************************************************
// HANDLERS DEL SERVIDOR WEB
//************************************************************************************************
void handle_OnConnect() {
  LED1status = LOW;
  server.send(200, "text/html", SendHTML());
}

void handle_led1on() {
  LED1status = HIGH;
  miServo.write(90);
  server.send(200, "text/html", SendHTML());
}

void handle_led1off() {
  LED1status = LOW;
  miServo.write(0);
  server.send(200, "text/html", SendHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Error 404: Pagina no encontrada");
}

//************************************************************************************************
// PROCESADOR DE HTML
//************************************************************************************************
String SendHTML() {
  String ptr = "<!DOCTYPE html><html lang='es'><head>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<meta http-equiv='refresh' content='1'>"; // Recarga la página cada 1 segundo
  ptr += "<title>Sistema inteligente de parqueos</title>";
  
  ptr += "<style>";
  ptr += "body { font-family: 'Segoe UI', sans-serif; margin: 0; background: #1a1b26; color: white; text-align: center; }";
  ptr += ".container { max-width: 600px; margin: auto; padding: 20px; }";
  ptr += ".card { background: rgba(255, 255, 255, 0.95); border-radius: 15px; padding: 20px; color: #333; margin-bottom: 20px; box-shadow: 0 8px 20px rgba(0,0,0,0.4); }";
  ptr += ".parking-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; margin-bottom: 20px; }";
  ptr += ".slot { background: #f0f0f0; border: 1px solid #ddd; border-radius: 8px; padding: 10px; font-size: 0.8em; }";
  ptr += ".slot img { width: 45px; display: block; margin: 5px auto; }";
  ptr += ".row-title { text-align: left; background: #34495e; color: white; padding: 5px 10px; border-radius: 5px; margin-bottom: 8px; font-size: 0.9em; }";
  ptr += ".heli { grid-column: 1 / -1; background: #2c3e50; color: white; border: 2px solid #f1c40f; }";
  ptr += ".btn { display: inline-block; padding: 15px 30px; text-decoration: none; border-radius: 8px; color: white; font-weight: bold; margin-top: 10px; }";
  ptr += ".btn-on { background: #27ae60; } .btn-off { background: #c0392b; }";
  ptr += ".status-label { font-weight: bold; font-size: 0.7em; }";
  ptr += "</style></head><body>";

  ptr += "<div class='container'><h1>PARQUEO INTELIGENTE</h1>";

  ptr += "<div class='card'><h2>Acceso Parqueo</h2>";
  if (LED1status) {
    ptr += "<p>Estado: <strong style='color:#27ae60;'>ABIERTA</strong></p>";
    ptr += "<a href='/led1off' class='btn btn-off'>BAJAR BARRERA</a>";
  } else {
    ptr += "<p>Estado: <strong style='color:#c0392b;'>CERRADA</strong></p>";
    ptr += "<a href='/led1on' class='btn btn-on'>SUBIR BARRERA</a>";
  }
  ptr += "</div>";

  ptr += "<div class='card'><h2>Disponibilidad de parqueos</h2>";
  
  ptr += "<div class='row-title'>NIVEL A</div>";
  ptr += "<div class='parking-grid'>";
  ptr += generarHTMLSlot("A1", A1_status);
  ptr += generarHTMLSlot("A2", A2_status);
  ptr += generarHTMLSlot("A3", A3_status);
  ptr += generarHTMLSlot("A4", A4_status);
  ptr += "</div>";

  ptr += "<div class='row-title'>NIVEL B</div>";
  ptr += "<div class='parking-grid'>";
  ptr += generarHTMLSlot("B1", B1_status);
  ptr += generarHTMLSlot("B2", B2_status);
  ptr += generarHTMLSlot("B3", B3_status);
  ptr += generarHTMLSlot("B4", B4_status);
  ptr += "</div>";

  ptr += "<div class='row-title'>ZONA VIP</div>";
  ptr += "<div class='parking-grid'><div class='slot heli'>";
  ptr += "<strong>HELIPUERTO H</strong>";
  if (H_status) {
    ptr += "<img src='https://images.vexels.com/media/users/3/138095/isolated/preview/5f69fe89ca6ee4a24dbc1858c5b8cdf9-icono-de-transporte-de-helicoptero.png'>";
    ptr += "<span style='color:#e74c3c' class='status-label'>OCUPADO</span>";
  } else {
    ptr += "<img src='https://png.pngtree.com/png-vector/20230218/ourmid/pngtree-helipad-vector-icon-design-illustration-png-image_6604127.png' style='filter:invert(1)'>";
    ptr += "<span style='color:#27ae60' class='status-label'>DISPONIBLE</span>";
  }
  ptr += "</div></div></div></div></body></html>";
  
  return ptr;
}

String generarHTMLSlot(String id, bool ocupado) {
  String s = "<div class='slot'><strong>" + id + "</strong>";
  if (ocupado) {
    s += "<img src='https://cdn-icons-png.flaticon.com/512/2830/2830180.png'>";
    s += "<span style='color:#e74c3c' class='status-label'>OCUPADO</span>";
  } else {
    s += "<img src='https://static.vecteezy.com/system/resources/previews/016/770/608/non_2x/parking-road-sign-on-transparent-background-free-png.png'>";
    s += "<span style='color:#27ae60' class='status-label'>LIBRE</span>";
  }
  s += "</div>";
  return s;
}

//**************************************************
// Verificación de conexión con los esclavos
//**************************************************
void i2cScanner() {
  byte error, address; 
  int nDevices = 0; 
  Serial.println("Escaneando dispositivos I2C.........."); 
  
  for (address = 1; address < 127;  address++){
    Wire.beginTransmission(address); 
    error = Wire.endTransmission(); 
    
    if(error == 0) {
      Serial.print("I2C dispositivo encontrado en direccion 0x"); 
      if(address < 16) Serial.print("0"); 
      Serial.println(address, HEX); 
      nDevices++; 
    }
    else if (error == 4) {
      Serial.print("Error desconocido en direccion 0x"); 
      if(address < 16) Serial.print("0"); 
      Serial.println(address, HEX); 
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No se encontro ningun dispositivo I2C. Revisa conexiones."); 
  } else {
    Serial.println("Escaneo de I2C completado exitosamente."); 
  }
}