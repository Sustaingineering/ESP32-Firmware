/*
 * Content: class that controls Temperature sensor
 * Author : Tarek Al Ayoubi
 * Team   : Sustaingineering UBC
 */

#pragma once
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "LoRaTransceiver.h"
#include "farmSensor.h"

class temp: public farmSensor{
private:
    OneWire m_oneWire;
    DallasTemperature m_sensors; 
protected:
    float readRaw() override;
public: 
    temp(int pin, sensorType type, String name, String unit, char shortcut);
    void initialize() override;
};

temp::temp(int pin, sensorType type, String name, String unit, char shortcut): 
                farmSensor(pin, type, name, unit, shortcut), m_oneWire(pin), m_sensors(&m_oneWire) {}


void temp::initialize()
{
    m_sensors.begin();
}

float temp::readRaw()
{
    m_sensors.requestTemperatures();
    return m_sensors.getTempCByIndex(0);
}



/* ------------------------- Original Code By Tarek ------------------------- */

/*
class temp
{
private:
    int m_pin;
    OneWire m_oneWire;
    DallasTemperature m_sensors;
    int m_data;
    float readRaw();

public:
    temp(int pin);
    void initialize();
    packet pack(); //for sending packets
    String read();

};

temp::temp(int pin): m_oneWire(pin), m_sensors(&m_oneWire)
{
}

void temp::initialize()
{
    m_sensors.begin();
}

float temp::readRaw()
{
    m_sensors.requestTemperatures();
    return m_sensors.getTempCByIndex(0);
}

packet temp::pack()
{
    packet ret;
    ret.type = 't';
    ret.data = m_data;
    return ret;
}

String temp::read()
{
    m_data = readRaw(); //temperature read
    return String("Temperature") + String(": ") + String(m_data) + String(" (") + String("degrees Celsius") + String(")") + String(" | ");
}
*/