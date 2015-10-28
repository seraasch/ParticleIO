
#include "ParticleIO.h"
#include "application.h"

void ParticleIO::configure_pins() {
    for (unsigned i=0; i<num_pins; i++) {
        
        if (!pins[i].Input_Pin) {
            //  Set pin for OUTPUT
            pinMode(pins[i].PackagePin, OUTPUT);
        }
        else {
            //  Input pins differ by type
            if (pins[i].Digital_Pin) {
            pinMode(pins[i].PackagePin, INPUT_PULLUP);
            }
            else {
                pinMode(pins[i].PackagePin, INPUT);
            }
        }
    }
}

uint16_t ParticleIO::read_pin(unsigned index, bool *updated) {
    uint16_t rv = 0;
    
    if ( !pins[index].is_configured() ) {
        return 0;
    }
    
    if ( !pins[index].is_virtual() ) {
        
        //
        //  Physical Pin
        //
        
        unsigned id = pins[index].PackagePin;
        
        if (pins[index].Digital_Pin) {
            rv = digitalRead(id);
        }
        else {
            rv = analogRead(id);
        }
    
        //  Update change indication if 'value_changed' pointer is provided by the user
        if (updated != 0) {
            if (rv != last_value[id]) {
                *updated = true;
                
                //  Keeping track internally
                value_changed[index] = true;
            }
            else {
                *updated = false;       

                //  Keeping track internally
                value_changed[index] = false;
            }
        }
        last_value[index] = rv;
    }
    else {
        //
        //  Virtual Pin
        //
        
        rv = last_value[index];
    }
    
    return rv;
}


//  Read all pins and update the last_value and value_changed arrays
void ParticleIO::read_all_pins() {
    for (unsigned i=0; i<num_pins; i++) {
        read_pin(i);
    }
}

bool ParticleIO::write_pin(unsigned index, uint16_t value) {

    if ( !pins[index].is_configured() )
        return false;

    if ( !pins[index].is_virtual() ) {
        
        //
        //  Physical Pin
        //
        
        if (!pins[index].Input_Pin) {
            if (pins[index].Digital_Pin) {
                digitalWrite(pins[index].PackagePin, (value != 0));  // Convert uint to bool for single-bit write
            }
            else {
                analogWrite(pins[index].PackagePin, value);
            }
        
            return true;
        }
        return false;
    }
    else {
        last_value[index] = value;
        return true;
    }
}

bool ParticleIO::publish_pin_MQTT(unsigned index, MQTT *mqtt) {
    char s[10];
    unsigned length;
    
    if ( !pins[index].is_configured() )
        return false;
    
    if (pins[index].Digital_Pin) {
        strncpy(s,last_value[index] ? "1" : "0", 10);
    }
    else {
        sprintf(s,"0x%04X", last_value[index]);
    }
    
    mqtt->publish(pins[index].MQTT_Topic, (uint8_t *)s, strlen(s));
    
    return true;
}

void ParticleIO::publish_changed_MQTT(MQTT *mqtt, bool publish_all) {
    for (unsigned i=0; i<num_pins; i++) {
        if (publish_all || value_changed[i])
        publish_pin_MQTT(i, mqtt);
    }
}
