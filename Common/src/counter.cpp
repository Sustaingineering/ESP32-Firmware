#include "Counter.h"

#include "Counter.h"

Counter ** Counter::createCounters(int counters)
{
    char letter = 'a';
    counter **counterArray = (counter **) malloc(sizeof(counter *) * counters);
    for (int j = 0; j < counters; j++)
    {
        String name = "Counter";
        name += j;
        counterArray[j] = new counter(j, name, "T", letter); 
        letter += 1;
    }

    return counterArray;
}

Counter::counter(int pin,  String name, String unit, char shortcut):farmSensor(pin, name, unit, shortcut)
{
    m_data = 1.0 * pin;
}

void Counter::initialize()
{
    /*Nothing to initialize*/
}

float Counter::readRaw()
{
    return m_data + 0.1;
}