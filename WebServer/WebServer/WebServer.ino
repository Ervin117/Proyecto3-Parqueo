/*************************************************************************************************
  ESP32 Web Server
  Ejemplo de creación de Web server 
  Basándose en los ejemplos de: 
  https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
  https://electropeak.com/learn
**************************************************************************************************/
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
//************************************************************************************************
// Variables globales
#define I2CSlaveAddress1 0x18 //Para la nucleo con pantalla
#define I2CSlaveAddress2 0x27 //Para la nucleo con 7SEG

#define I2C_SDA 21
#define I2C_SCL 22
uint8_t error = 0;  

unsigned long previousMillis = 0;
const long interval = 500;
//************************************************************************************************
// SSID & Password
const char* ssid = "Bryan";  // Enter your SSID here
const char* password = "marvelitas";  //Enter your Password here

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

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
int pinServo = 18; // Puedes usar el pin 18 o cualquier otro PWM
int posicion = 0;  // 0 para cerrado, 1 para abierto
//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  Wire.begin(I2C_SDA, I2C_SCL); 
  i2cScanner(); 

  pinMode(LED1pin, OUTPUT);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  // Only show dots until unitl the contection is ready
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial
 
  server.on("/", handle_OnConnect); // Directamente desde e.g. 192.168.0.8
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);

  ESP32PWM::allocateTimer(0); // Configuración necesaria para ESP32
  miServo.setPeriodHertz(50);    // Standard 50hz servo
  miServo.attach(pinServo, 500, 2400); // Attach con tiempos de pulso estándar
  miServo.write(0); // Iniciar en 0 grados
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {

  /*
  server.handleClient(); // Atiende las peticiones web
  
  // Aplicar el estado al LED físico
  digitalWrite(LED1pin, LED1status);
  // Lógica para interpretar datos del UART
  if (Serial.available() > 0) {
    char numero_recibido = Serial.read(); // Leemos el carácter

    // Lógica para variables
    if (numero_recibido == 'a') {
      A1_status = HIGH;
      Serial.println("Espacio A1 Ocupado");
    } 
    else if (numero_recibido == 'b') {
      A2_status = HIGH;
      Serial.println("Espacio A2 Ocupado");
    } 
    else if (numero_recibido == 'c') {
      A3_status = HIGH;
      Serial.println("Espacio A3 Ocupado");
    } 
    else if (numero_recibido == 'd') {
      A4_status = HIGH;
      Serial.println("Espacio A4 Ocupado");
    }
    
    // Lógica para variables B
    else if (numero_recibido == 'e') {
      B1_status = HIGH;
      Serial.println("Espacio B1 Ocupado");
    } 
    else if (numero_recibido == 'f') {
      B2_status = HIGH;
      Serial.println("Espacio B2 Ocupado");
    } 
    else if (numero_recibido == 'g') {
      B3_status = HIGH;
      Serial.println("Espacio B3 Ocupado");
    } 
    else if (numero_recibido == 'h') {
      B4_status = HIGH;
      Serial.println("Espacio B4 Ocupado");
    }
    
    // Lógica para variable H 
    else if (numero_recibido == 'x') {
      H_status = HIGH;
      Serial.println("Helipuerto ocupado");
    }

    // para recetear todos los estados
    else if (numero_recibido == 'r') {
      A1_status = A2_status = A3_status = A4_status = LOW;
      B1_status = B2_status = B3_status = B4_status = LOW;
      H_status = LOW;
    }
  }
  */




  /*
  server.handleClient(); // Atiende las peticiones web
  
  // Aplicar el estado al LED físico
  digitalWrite(LED1pin, LED1status);

  // Lógica para interpretar datos del UART (PC -> ESP32)
  if (Serial.available() > 0) {
    char numero_recibido = Serial.read(); 
    // Lógica para variables Nivel A
    if (numero_recibido == 'a') { A1_status = HIGH; Serial.println("Espacio A1 Ocupado"); } 
    else if (numero_recibido == 'b') { A2_status = HIGH; Serial.println("Espacio A2 Ocupado"); } 
    else if (numero_recibido == 'c') { A3_status = HIGH; Serial.println("Espacio A3 Ocupado"); } 
    else if (numero_recibido == 'd') { A4_status = HIGH; Serial.println("Espacio A4 Ocupado"); }
    
    // Lógica para variable H 
    else if (numero_recibido == 'x') { H_status = HIGH; Serial.println("Helipuerto ocupado"); }

    // para resetear todos los estados
    else if (numero_recibido == 'r') {
      A1_status = A2_status = A3_status = A4_status = LOW;
      B1_status = B2_status = B3_status = B4_status = LOW;
      H_status = LOW;
    }
  }

  // Lógica de Comunicación I2C (cada 500 ms)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 1. ENVIAR DATOS (NIVEL A) AL STM32
    Wire.beginTransmission(I2CSlaveAddress2); 
    Wire.write(A1_status);
    Wire.write(A2_status);
    Wire.write(A3_status);
    Wire.write(A4_status);
    error = Wire.endTransmission(); 

    if(error == 0) { // Si la escritura fue exitosa
      
      // 2. PEDIR DATOS (NIVEL B) AL STM32
      uint8_t bytesReceived = Wire.requestFrom((uint16_t)I2CSlaveAddress2, (uint8_t)4); 
      
      if (bytesReceived == 4) {
        // Leer los 4 bytes y actualizar las variables para la página web
        B1_status = Wire.read();
        B2_status = Wire.read();
        B3_status = Wire.read();
        B4_status = Wire.read();
      } else {
        Serial.println("Error: No se recibieron 4 bytes del STM32"); 
      }

    } else {
      Serial.print("Error I2C TX al STM32. Codigo: ");
      Serial.println(error);
    }
  }

  */
 server.handleClient(); // Atiende las peticiones web
  
  digitalWrite(LED1pin, LED1status);

  // Lógica de Comunicación I2C (cada 500 ms)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // ==========================================================
    // PASO 1: LEER SENSORES (Nivel A) DE LA PLACA 7 SEG (0x27)
    // ==========================================================
    uint8_t bytesA = Wire.requestFrom((uint16_t)I2CSlaveAddress2, (uint8_t)4); 
    if (bytesA == 4) {
      A1_status = Wire.read();
      A2_status = Wire.read();
      A3_status = Wire.read();
      A4_status = Wire.read();
    }

    // ==========================================================
    // PASO 2: LEER SENSORES (Nivel B) DE LA PLACA PANTALLA (0x18)
    // ==========================================================
    uint8_t bytesB = Wire.requestFrom((uint16_t)I2CSlaveAddress1, (uint8_t)4); 
    if (bytesB == 4) {
      B1_status = Wire.read();
      B2_status = Wire.read();
      B3_status = Wire.read();
      B4_status = Wire.read();
    }

    // ==========================================================
    // PASO 3: ENVIAR DATOS A LA PANTALLA (Animaciones Nivel A)
    // ==========================================================
    Wire.beginTransmission(I2CSlaveAddress1); 
    Wire.write(A1_status);
    Wire.write(A2_status);
    Wire.write(A3_status);
    Wire.write(A4_status);
    Wire.endTransmission(); 

    // ==========================================================
    // PASO 4: CALCULAR TOTAL Y ENVIAR AL 7 SEGMENTOS
    // ==========================================================
    uint8_t ocupados = A1_status + A2_status + A3_status + A4_status + 
                       B1_status + B2_status + B3_status + B4_status;
    
    uint8_t libres = 8 - ocupados;

    Wire.beginTransmission(I2CSlaveAddress2); 
    Wire.write(libres); // El byte [0] tiene el total de libres
    Wire.write(0);      // Byte [1] relleno (STM32 espera 4 bytes fijos)
    Wire.write(0);      // Byte [2] relleno
    Wire.write(0);      // Byte [3] relleno
    Wire.endTransmission();
  }
}
//************************************************************************************************
// Handler de Inicio página
//************************************************************************************************
//String htlm = SendHTML(LED1status, A1_status, A2_status, A3_status, A4_status, B1_status, B2_status, B3_status, B4_status, H_status);
void handle_OnConnect() {
  LED1status = LOW;
  //Serial.println("GPIO2 Status: OFF");
  server.send(200, "text/html", SendHTML());
}
//************************************************************************************************
// Handler de led1on
//************************************************************************************************
void handle_led1on() {
  LED1status = HIGH;
  miServo.write(90);
  //Serial.println("GPIO2 Status: ON");
  server.send(200, "text/html", SendHTML());
}
//************************************************************************************************
// Handler de led1off
//************************************************************************************************
void handle_led1off() {
  LED1status = LOW;
  miServo.write(0);
  //Serial.println("GPIO2 Status: OFF");
  server.send(200, "text/html", SendHTML());
}
//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML() {
  String ptr = "<!DOCTYPE html><html lang='es'><head>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<meta http-equiv='refresh' content='1'>";
  ptr += "<title>Sistema ingeligente de parqueos</title>";
  
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

  // SECCIÓN TALANQUERA
  ptr += "<div class='card'><h2>Acceso Parqueo</h2>";
  if (LED1status) {
    ptr += "<p>Estado: <strong style='color:#27ae60;'>ABIERTA</strong></p>";
    ptr += "<a href='/led1off' class='btn btn-off'>BAJAR BARRERA</a>";
  } else {
    ptr += "<p>Estado: <strong style='color:#c0392b;'>CERRADA</strong></p>";
    ptr += "<a href='/led1on' class='btn btn-on'>SUBIR BARRERA</a>";
  }
  ptr += "</div>";

  // MONITOR DE PARQUEOS
  ptr += "<div class='card'><h2>Disponibilidad de parqueos</h2>";
  
  // Fila A
  ptr += "<div class='row-title'>NIVEL A</div>";
  ptr += "<div class='parking-grid'>";
  ptr += generarHTMLSlot("A1", A1_status);
  ptr += generarHTMLSlot("A2", A2_status);
  ptr += generarHTMLSlot("A3", A3_status);
  ptr += generarHTMLSlot("A4", A4_status);
  ptr += "</div>";

  // Fila B
  ptr += "<div class='row-title'>NIVEL B</div>";
  ptr += "<div class='parking-grid'>";
  ptr += generarHTMLSlot("B1", B1_status);
  ptr += generarHTMLSlot("B2", B2_status);
  ptr += generarHTMLSlot("B3", B3_status);
  ptr += generarHTMLSlot("B4", B4_status);
  ptr += "</div>";

  // Helipuerto
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
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

//**************************************************
// Verificación de conexión con los esclavos
//**************************************************
void i2cScanner()
{
  byte  error, address; 
  int nDevices; 
  Serial.println("Escaniando.........."); 
  nDevices = 0; 
  for (address = 1; address < 127;  address++){
    Wire.beginTransmission(address); 
    error = Wire.endTransmission(); 
    if(error == 0)
    {
      Serial.print("I2C despositivo encontrado en dirección 0x"); 
      if(address < 16)
      {
        Serial.print("0"); 
      }
      Serial.println(address, HEX); 
      nDevices++; 
    }
    else if (error == 4)
    {
      Serial.print("Error en la conexión"); 
      if(address < 16)
      {
        Serial.print("0"); 
      }
      Serial.println(address, HEX); 
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No se encontro la direccción del dispositivo"); 
  }
  else
  {
    Serial.println("Nada"); 
  }
}

void comunucacionI2C()
{
  delay(1000); 
  
  //Mandar datos a un esclavo
  Wire.beginTransmission(I2CSlaveAddress1); 
  //Wire.write();//el valor a enviar
  Serial.println(); //Imprimir el valor enviado

  error = Wire.endTransmission(true); 
  Serial.print("Transmisión termianda"); 
  Serial.println(error);  

  //Recibir datos de un esclavo

  uint8_t bytesReceived = Wire.requestFrom(I2CSlaveAddress1, 4); 
  Serial.print("Respuesta: "); 
  Serial.println(bytesReceived); 
  if (bytesReceived > 0){
    uint8_t temp[10]; 
    for(int i = 0; i<bytesReceived; i++){
      temp[i] = Wire.read(); 
      Serial.print("Byte:"); 
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(temp[i]);
    }

  }else {
    Serial.println("No se reciben datos"); 
  } 
}