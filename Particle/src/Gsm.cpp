#include "Gsm.h"


bool Gsm::initialize()
{
    delimiter = "\n";
    m_buffer = "";
    m_counter = 0;
}

String Gsm::Publish(String pumpId, String message)
{
    /**
     * 1. If buffer is empty, add to it & start tracking time 
     * 2. If message time < MAXTIME_BTWN_MESSAGES; reject message; else add to buffer
     * 3. If m_counter == TOTAL_MESSAGES_CAP; publish & return message; else return empty string
     * 
     * delimiter = "\n"
     */
    int time = Time.now();

    if (m_buffer == "")
    {
        // Add to empty buffer & set up counter & timer
        m_buffer += pumpId + delimiter;
        m_buffer += message + delimiter;
        m_counter++;
        m_timeFromLastMessage = Time.now();

    }
    else if (time - m_timeFromLastMessage >= TIME_BTWN_MESSAGES)
    {
        Serial.println("Adding to buffer " + message);
        m_buffer += message + delimiter;
        m_counter++;
        m_timeFromLastMessage = time;

        if (m_counter == TOTAL_MESSAGES_CAP)
        {
            // Publish method may block (20 secs - 10 mins)
            // https://docs.particle.io/reference/device-os/firmware/#particle-publish-
            if (Particle.publish(m_buffer.c_str()))
            {
                Serial.println("Succesfully Published Message: " + m_buffer);
                String result = m_buffer;
                m_buffer = "";
                m_counter = 0;
                return result;
            }
            else
                Serial.println("Could not publish message?????!!!");
                // Should probably do more here
        }
    }
    
    Serial.printf("Not printing message yet. m_counter: %d; m_timeFromLastMessage: %d\n", m_counter, m_timeFromLastMessage);
    Serial.println("Current Message: " + m_buffer);
    return String("");
}

int Gsm::getTotalDataUsage_()
{
    CellularData data;
    if (!Cellular.getDataUsage(data)) {
        Serial.println("Error! Not able to get data.");
        return 0;
    }
    else {
        Serial.println(data);
        return data.tx_total + data.rx_total;
    }
}
