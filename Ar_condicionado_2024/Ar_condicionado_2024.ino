#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

#define MARK_EXCESS_MICROS    20    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.

#include <IRremote.hpp>


#include <dht.h>

dht DHT;

#define DHT22_PIN     12


int SEND_BUTTON_PIN = APPLICATION_PIN;
int ledUm = 6;
int ledZero = 7;

int DELAY_BETWEEN_REPEAT = 50;

void sendCode();

String data;
char d1; 



void setup() {
    pinMode(SEND_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ledZero, OUTPUT);
    pinMode(ledUm, OUTPUT);

    Serial.begin(9600);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

    IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
    Serial.print(F("Ready to send IR signals at pin " STR(IR_SEND_PIN) " on press of button at pin "));
    Serial.println(SEND_BUTTON_PIN);
}

void loop() {
  int chk = DHT.read22(DHT22_PIN);

  Serial.print("temperatura: ");
  Serial.println(DHT.temperature, 1);
  delay(2000);
  
  if(Serial.available()){
    data = Serial.readString();
    d1 = data.charAt(0);
    

    switch(d1){   
      case 'L':
        sendCode(1,1);
      break;

      case 'D':
        sendCode(1,0);
      break;

      case 'l':
        sendCode(0,1);
      break;

      case 'd':
        sendCode(0,0);
      break;
        
    }

  }
}

int sendCode(int NumeroDoAr, int Liga_Desliga) {
  if(NumeroDoAr){ //controla primeiro ar
    if(Liga_Desliga){
      Serial.println("Ar 1 ligado");
      digitalWrite(ledUm, 1);
      digitalWrite(ledZero, 0);
      uint32_t tRawData[]={0x126CB23, 0x8032400, 0x3D, 0x8100};
      IrSender.sendPulseDistanceWidthFromArray(38, 3250, 1750, 350, 1300, 350, 450, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
    else{
      Serial.println("Ar 1 desligado");
      digitalWrite(ledUm, 1);
      digitalWrite(ledZero, 0);    
      uint32_t tRawData[]={0x126CB23, 0x8032000, 0x3D, 0x7D00};
      IrSender.sendPulseDistanceWidthFromArray(38, 3400, 1600, 450, 1200, 450, 400, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
  }
  else{ //controla segundo ar
    if(Liga_Desliga){
      Serial.println("Ar 2 ligado");
      digitalWrite(ledUm, 0);
      digitalWrite(ledZero, 1);
      uint32_t tRawData[]={0x126CB23, 0x8032400, 0x3D, 0x8100};
      IrSender.sendPulseDistanceWidthFromArray(38, 3250, 1750, 350, 1300, 350, 450, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
    else{
      Serial.println("Ar 2 desligado");
      digitalWrite(ledUm, 0);
      digitalWrite(ledZero, 1);
      uint32_t tRawData[]={0x126CB23, 0x8032000, 0x3D, 0x7D00};
      IrSender.sendPulseDistanceWidthFromArray(38, 3400, 1600, 450, 1200, 450, 400, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);    
    }
  }
    
}
