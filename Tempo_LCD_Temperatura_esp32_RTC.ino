#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include <LiquidCrystal_I2C.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Tcl.h>
#include "RTClib.h"       



//configurações para dht
#include "DHT.h"
//here we use 14 of ESP32 to read data
#define DHTPIN 13
//our sensor is DHT11 type
#define DHTTYPE DHT11
//create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//Config para lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);   //LCD Object

 
//config para ac

int ar_ligado = 2;

IRsend irsend(14);  // An IR LED is controlled by GPIO pin 14 (D5)


// 0 : off
// 1 : on
unsigned int ac_power_on = 0;

uint32_t ac_code_to_sent;


//CONTROLE DE AC
void Ac_Send_Code(uint32_t code) {
  //Essa função serve para mandar os códigos ir para o AC da LG

#if SEND_LG
  irsend.sendLG(code, 28);
#else  // SEND_LG
  Serial.println("Can't send because SEND_LG has been disabled.");
#endif  // SEND_LG

  
}

void Ac_Power_Down() {//Essa função serve para desligar o AC da LG
    //Essa função serve para desligar o AC da LG
  ac_code_to_sent = 0x88C0051;

  Ac_Send_Code(ac_code_to_sent);

  //ac_power_on = 0;
  Serial.print("LG Desligado");

}

void Ac_Activate() {//Essa função serve para ligar o AC da LG
  //Essa função serve para ligar o AC da LG
  //Para escolher uma temperatura inicial diferente é nescessário mudar o código Hexadeximal abaixo. 
  //Esse código pode ser obtido através do monitor serial no programa de exemplo LG_SEND dabiblioteca irRemote
  
  // temperatura de inicialização = 18 ac_code_to_sent = 0x8800549;
  // temperatura de inicialização = 24 ac_code_to_sent = 0x8800729;
  ac_code_to_sent = 0x8800628; //temperatura de inicialização = 21

  Ac_Send_Code(ac_code_to_sent);
  
  Serial.print("LG Ligado");
}

//Configurações ir
const uint16_t kIrLed = 14;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRTcl112Ac ac(kIrLed);  // Set the GPIO to be used for sending messages.

void AC_TCL_Activate(){//Essa função liga o ar condicionado da marca TCL
  //Essa função liga o ar condicionado da marca TCL
  //Podemos ajustar a temperatura pelo comando setTemp
  
  ac.setPower(true);
  ac.setFan(kTcl112AcFanHigh);
  ac.setMode(kTcl112AcCool);
  ac.setTemp(22);

  // Now send the IR signal.
#if SEND_TCL112AC
  ac.send();
  Serial.print("TCL Ligado");
#else  // SEND_TCL112AC
  Serial.println("Can't send because SEND_TCL112AC has been disabled.");
#endif  // SEND_TCL112AC

}

void AC_TCL_Power_down(){//Essa função desliga o ar condicionado da marca TCL
  //Essa função serve para desligar o AC da LG
  ac.setPower(false);
  ac.send();
  Serial.print("TCL Desligado");

}
  
void printaArLigado(int ar_ligado){// Função printa no monitor serial e na tela lcd os aparelhos que foram ligados

  if(ar_ligado == 1){
    Serial.println("Ar Lg ligado");
    lcd.setCursor(4,0);
    lcd.print("LG");
    
  }
  if(ar_ligado == 12){
    Serial.println("Os dois aparelhos foram ligados");
    lcd.setCursor(3,0);
    lcd.print("12");
  
  }
  if(ar_ligado ==2){
    Serial.println("Ar TCL ligado");
    lcd.setCursor(3,0);
    lcd.print("TCL");
  }
}

//declaração do dht

RTC_DS3231 rtc;
int hora;
int minuto;
int contador = 7199;

int temp_Hum();


void setup()
{
  Serial.begin(115200);
  
  ac.begin(); //INICIA O CONTROLE DE IR
  irsend.begin();
  
  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();

  // iniciar DHT
  dht.begin(); // SENSOR DE TEMP E HUMIDADE

  // iniciar RTC

  if (! rtc.begin()) {
     Serial.println("Não foi possível encontrar RTC");
     while (1);
  }
  //rtc.adjust(DateTime(7, 6,2023, 13, 38, 30)); // para ajustar data

  delay(3000);

}

void loop()
{


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ar:");
  lcd.setCursor(6,0);
  lcd.print("ON");
  
  int temp = temp_Hum();
  DateTime now = rtc.now();


  hora = now.hour();
  minuto = now.minute();

  lcd.setCursor(9,0);
  lcd.print(hora, DEC);
  lcd.setCursor(11,0);
  lcd.print(":");
  lcd.setCursor(12,0);
  lcd.print(minuto, DEC);

 
  if(ar_ligado == 2){

    contador += 1;
    if(contador == 7200){
      Ac_Activate();
      AC_TCL_Power_down();
      contador = 0;
      ar_ligado = 1;
    }
    }
  else{ 
    contador += 1;
    if(contador == 7200){
    
      Ac_Power_Down();
      AC_TCL_Activate();
      contador = 0;
      ar_ligado = 2;
    }
    
    }

  if(temp >26){
    AC_TCL_Activate();
    Ac_Activate();
    ar_ligado = 12;
  }

  printaArLigado(ar_ligado);


  
  delay(1000);
  

}

//FUNÇÕES


//temperatura e humidade
int temp_Hum()
{
  //use the functions which are supplied by library.
  int humidade = dht.readHumidity();

  // Read temperature as Celsius (the default)
  int temp = dht.readTemperature();
  
  lcd.setCursor(0,1);
  lcd.print("Temp:"); //Tuesday 10:30:05
  lcd.setCursor(6,1);
  lcd.print(temp);
  lcd.setCursor(9,1);
  lcd.print("Hu:"); //Tuesday 10:30:05
  lcd.setCursor(12,1);
  lcd.print(humidade);

  Serial.print("temperatura: ");
  Serial.println(temp);
  Serial.print("humidade: ");
  Serial.println(humidade);
  
  return temp;
}
