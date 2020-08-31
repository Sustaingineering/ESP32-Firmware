#include <Arduino.h>
#include "PinConfig.h"
#ifndef electron
//#include "Restarter.h"
#include "RealTimeClock.h"
#include "SdCard.h"
#include "farmSensor.h"
#include "temp.h"
#include "volt.h"
#include "current.h"
#include "Flow.h"
#include "LoRaTransceiver.h"
#endif
#include "counter.h"

#define PUMP_ID 0

//Global Objects
//Restarter restarter(5);
String message;

#if CURRENT
current hall_effect(CURRENT_PIN, "Current", "Amps", 'i');
#endif

#if VOLTAGE
volt volt_divider(VOLT_PIN, analog, "Voltage", "Volt", 'v', 1000, 1000);
#endif

#if TEMP
temp thermocouple(TEMP_PIN, digital, "Temperature", "Celsius", 't'); //pretty slow response and depends greatly on the surface temperature of the thermocouple tip
#endif

#if FLOW
flow waterflow(FLOW_PIN,"WaterFlow", "L/min", 'f');
#endif

#if ERTC
RealTimeClock rtc(false);
#endif

#if SDCARD
SdCard memory;
#endif

#if LORA
bool LoRaStatus;
packet packets[NUMBER_OF_PACKETS];
#if GSM
LoRaTransceiver requester(LORA_SELECT_PIN, LORA_RST_PIN, LORA_DIO0_PIN, LORA_SECRET_WORD);
#else
LoRaTransceiver responder(LORA_SELECT_PIN, LORA_RST_PIN, LORA_DIO0_PIN, LORA_SECRET_WORD, PUMP_ID);
#endif //GSM
#endif //LORA

#if COUNTERS
counter counter1(0, "Counter1", "T", 'c');
counter counter2(0, "Counter2", "T", 'd');
counter counter3(0, "Counter3", "T", 'e');
counter counter4(0, "Counter4", "T", 'f');
counter counter5(0, "Counter5", "T", 'g');
counter counter6(0, "Counter6", "T", 'h');
#endif

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("\nHello Sustaingineering!\n");

#if CURRENT
  Serial.println("Initializing Current Sensor...");
  hall_effect.initialize();
  Serial.println("Current Sensor Initialized.\n");
#endif

#if VOLTAGE
  Serial.println("Initializing Voltage Sensor...");
  volt_divider.initialize();
  Serial.println("Voltage Sensor Initialized.\n");
#endif
  
#if TEMP
  Serial.println("Initializing Thermocouple...");
  thermocouple.initialize();
  Serial.println("Thermocouple Initialized.\n");
#endif

#if FLOW
  Serial.println("Initializing Waterflow...");
  waterflow.initialize();
  Serial.println("WaterFlow Initialized.\n");
#endif

#if ERTC
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
#if GSM
  requester.initialize();
#else
  responder.initialize();
#endif // GSM
  Serial.println("LoRa Initialized.\n");
#endif // LORA

  //Sensors Initializers go here.
  
  Serial.println("Setup Done!\n");
}

void loop()
{
  message = "";
  //Sampling Sensors
#if COUNTERS
  message += counter1.read();
  message += counter2.read();
  message += counter3.read();
  message += counter4.read();
  message += counter5.read();
  message += counter6.read();
#endif

#if CURRENT
  message += hall_effect.read();
#endif

#if VOLTAGE
  message += volt_divider.read();
#endif

#if TEMP
  message += thermocouple.read();
#endif

#if FLOW
  message += waterflow.read();
#endif

#if ERTC
  message += rtc.getTimeStamp();
#endif

  message += String("\n");
  Serial.print(message);
  
#if SDCARD
  //Writing on Sd Card
  memory.appendFile("/" + rtc.getDate() + ".txt", message);
#endif

#if LORA
#ifdef electron 
  // make requests
  requester.request(0, packets, NUMBER_OF_PACKETS);
  Serial.print("received = [");
  for (int i = 0; i < NUMBER_OF_PACKETS; i++)
  {
    Serial.print(packets[i].type);
    Serial.print(packets[i].data);
    if (i != (NUMBER_OF_PACKETS - 1))
      Serial.print(", ");
  }
  Serial.print("]");
  delay(100);
#else //electron
  //Responding to a request from LoRa
  packets[0] = counter1.pack();
  packets[1] = counter2.pack();
  packets[2] = counter3.pack();
  packets[3] = counter4.pack();
  packets[4] = counter5.pack();
  packets[5] = counter6.pack();
  LoRaStatus = responder.respond(packets, NUMBER_OF_PACKETS);
#endif // electron
#else //LORA
  delay(1000);
#endif //LORA

#ifdef electron
String pumpId = "Pump" + String(PUMP_ID);
Particle.publish(pumpId.c_str(), message.c_str(), PUBLIC);
#endif

  //restarter.takeAction(LoRaStatus);
  Serial.println();
}
