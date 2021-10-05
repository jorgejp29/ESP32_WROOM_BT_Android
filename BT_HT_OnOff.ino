//**************************************************
//
//        Hum_y_T_ESP-WROOM-32_Android
//
//        Autor: Jorge Miguel Jaimes Ponce
//        UAM_Azc
//        Departamento de Electrónica
//        Ingeniería Electrónica
//        Desarrollo de Instrumentos
//
//        29 de Septiembre del 2021
//
//**************************************************

/* Este progama es parte de un sistema, que mide
 *  humedad y temperatura.
 *  Como sensor utiliza el HDT22
 *  Para desplegar la medición es por medio de un 
 *  display OLED, y una aplicación para Android.
 *  Desde la aplicación Android se controla el
 *  encendido y apagado de dos leds, que son las
 *  salidas para activar por ejemplo un relevador,
 *  y poder encender o apagar cargas de alta potecia.
*/


#include "BluetoothSerial.h"
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED -> ancho del display, en pixels
#define SCREEN_HEIGHT 64 // OLED -> alto del display, en pixels

//**********************************
//************ HDT22  **************
//**********************************
#include <DHT.h>
#include <Adafruit_Sensor.h>
#define DHTPin Sens_DHT22

const byte Sens_DHT22 = 2;  
DHT Sens_DHT(DHTPin,DHT22);

float hum = 0;
float temp = 0;
double  t_0;
double  t_1;
byte Flag = 1;

//**********************************

// Configuración SSD1306 conexión a I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define Led_1 19  //26
#define Led_2 18  //25

char dato;

BluetoothSerial SerialBT;

void setup() {

  Sens_DHT.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  } 
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Nombre del dispositivo BT
  Serial.println("Dispositivo iniciado, ahora puede ser dado de alta por BT!");
  pinMode(Led_1,OUTPUT);
  pinMode(Led_2,OUTPUT);

//  Inicialización de Display SSD1306
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(WHITE); 

   t_0 = millis();
}



void loop() {
  
   //*****   Adquirir valores del DHT22   *****

  if(Flag == 1){
          hum = Sens_DHT.readHumidity();      //Valor de Humedad
          temp = Sens_DHT.readTemperature();  //Valor de Temperatura
          Sensor_DHT22();
        
          //*****   FIN valores del DHT22   *****
        
          //Envío de Humedad y Temperatura hacia Android
      
          SerialBT.print(hum);
          SerialBT.print("  %HR");
          SerialBT.print(',');
          SerialBT.print(temp);
          SerialBT.print("  °C");

          Flag = 0;
          
        //delay(2000);
  }
        retardo();
   //Transmisión BT de encendido y apagado
   On_Off();
  
 }


//Subrutina para desplegar en Display
 void Sensor_DHT22(){
  display.display();

 //  Desplegado del encabezado
  display.clearDisplay();
  display.setCursor(0,0);             
  display.println("Hum y Temp");

  display.setCursor(0,10);
  display.println("    ");
  
  //  Desplegado de la Humedad
  display.setCursor(0,28); 
  display.write(37);               // Escribir caracter '%'       
  display.setCursor(12,28);        
  display.println("HR:  ");
  display.setCursor(46,28);
  display.print(hum);              // Imprimir valor de humedad
  
  //  Desplegado de Temperatura
  display.setCursor(6,46);  
  display.write(248);             // Escribir caracter '°'     
  display.setCursor(12,46);      
  display.println(" C: ");
  display.setCursor(46,46);
  display.print(temp);            // Imprimir valor de temperatura
  
  display.println(" ");
  delay(1000);            //  Mantener datos en dispplay 1000 ms
  
}

void On_Off(){
  if (Serial.available()) {
    //SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    dato = SerialBT.read();
    Serial.write(dato);
    
//Encendido y apagado de Led_1
    if(dato == 'a'){
      digitalWrite(Led_1,HIGH);
    }
    if(dato == 'b'){
      digitalWrite(Led_1,LOW);
    }

//Encendido y apagado de Led_2
    if(dato == 'c'){
      digitalWrite(Led_2,HIGH);
    }
     if(dato == 'd'){
      digitalWrite(Led_2,LOW);
    } 
  }
}

void retardo(){

  t_1 = millis();
  if ((t_1 - t_0)> 2000){
    Flag = 1;
    t_0 = t_1;
  }
}
