/*
                    _________________________
                   |EN          MOSI:VSPI:D23|* -> SD:MOSI
                  ?|D36:VP        SCL:I2C:D22|* -> RTC:SCL
                  ?|D39:VN   [ESP32]   TX0:D1|x
                  o|D34                RX0:D3|x
                  o|D35           SDA:I2C:D21|* -> RTC:SDA
                  o|D32         MISO:VSPI:D19|* -> SD:MISO
                  o|D33          CLK:VSPI:D18|* -> SD:SCK
                  o|D25            CS:VSPI:D5|* -> SD:SS
     LoRa:DIO0 <- *|D26               TX2:D17|o
    LoRa:RESET <- *|D27               RX2:D16|o
      LoRa:SCK <- *|D14:HSPI:CLK           D4|o
     LoRa:MISO <- *|D12:HSPI:MISO          D2|* -> BUILTIN_LED
     LoRa:MOSI <- *|D13:HSPI:MOSI CS:HSPI:D15|* -> LoRa:NSS
                  o|GND                   GND|*
                  o|VIN___________________3V3|*

(o): unused.
(*): used.
(x): cannot be used.
(?): WTF.
*/

// Hardware connected to ESP32 is true. Otherwise false.
#define RTC       true
#define SDCARD    true
#define LORA      true
#define COUNTERS  true
#define VOLTAGE   false
#define CURRENT   false
#define TEMP      false
#define FLOW      false

#include <Arduino.h>
#include "Restarter.h"
#include "watch.h"
#include "SdCard.h"
#include "farmSensor.h"
#include "temp.h"
#include "LoRaTransceiver.h"

//Global Objects
//Restarter restarter(5);
String message;

#if RTC
watch rtc(false);
#endif

#if SDCARD
SdCard memory;
#endif

#if LORA
#define PUMP_ID 0
#define LoRa_SECRET_WORD 0xF3
bool LoRaStatus;
packet packets[NUMBER_OF_PACKETS];
LoRaTransceiver responder(15, 27, 26, LoRa_SECRET_WORD, PUMP_ID);
#endif

#if COUNTERS
farmSensor counter1(0, counter, "Counter1", "T", 'c');
farmSensor counter2(0, counter, "Counter2", "T", 'd');
farmSensor counter3(0, counter, "Counter3", "T", 'e');
farmSensor counter4(0, counter, "Counter4", "T", 'f');
farmSensor counter5(0, counter, "Counter4", "T", 'g');
farmSensor counter6(0, counter, "Counter4", "T", 'h');
#endif
//Sensors Constructors go here.

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("\nHello Sustaingineering!\n");
  
#if RTC
  Serial.println("Initializing RTC...");
  rtc.initialize();
  Serial.println("RTC Initialized.\n");
#endif

#if SDCARD
  Serial.println("Initializing MicroSD Card...");
  memory.initialize();
  Serial.println("MicroSD Card Initialized.\n");
#endif

#if LORA
  Serial.println("Initializing LoRa...");
  responder.initialize();
  Serial.println("LoRa Initialized.\n");
#endif

  //Sensors Initializers go here.
  
  Serial.println("Setup Done!\n");
}

void loop()
{
  //Sampling Sensors
  digitalWrite(BUILTIN_LED, HIGH);
#if COUNTERS
  message  = counter1.read();
  message += counter2.read();
  message += counter3.read();
  message += counter4.read();
  message += counter5.read();
  message += counter6.read();
#endif

#if RTC
  message += rtc.getTimeStamp();
#endif

  message += String("\n");
  Serial.print(message);
  
#if SDCARD
  //Writing on Sd Card
  memory.appendFile("/" + rtc.getDate() + ".txt", message);
#endif

  digitalWrite(BUILTIN_LED, LOW);

#if LORA
  //Responding to a request from LoRa
  packets[0] = counter1.pack();
  packets[1] = counter2.pack();
  packets[2] = counter3.pack();
  packets[3] = counter4.pack();
  packets[4] = counter5.pack();
  packets[5] = counter6.pack();
  LoRaStatus = responder.respond(packets, NUMBER_OF_PACKETS);
#else
  delay(1000);
#endif

  //restarter.takeAction(LoRaStatus);
  Serial.println();
}
