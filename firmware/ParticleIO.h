
#ifndef _ParticleIO_h
#define _ParticleIO_h

#include "MQTT/MQTT.h"



struct IOPin {
    char *    Name;
    unsigned  PackagePin;
    bool      Input_Pin;
    bool      Digital_Pin;
    char *    MQTT_Topic;
    
    IOPin()
        : Name(0), PackagePin(0), Input_Pin(false), Digital_Pin(false), MQTT_Topic(0)
        {  }
    IOPin(const char *_name, unsigned _packagepin, bool _input_pin, bool _digital_pin, const char * _mqtt_topic) {
        Name        = const_cast<char*>(_name);         // fix const issues with string literals
        PackagePin  = _packagepin;
        Input_Pin   = _input_pin;
        Digital_Pin = _digital_pin;
        MQTT_Topic  = const_cast<char*>(_mqtt_topic);   // fix const issues with string literals
    }
    
    bool is_configured() const { return Name != 0; }
    bool is_virtual()    const { return PackagePin == 0; }

};

struct ParticleIO {
    IOPin          *pins;
    uint16_t       *last_value;
    bool           *value_changed;
    unsigned       num_pins;
    
    ParticleIO(unsigned _num_pins) {
        num_pins      = _num_pins;

        pins          = new IOPin[num_pins];
        last_value    = new uint16_t[num_pins];
        value_changed = new bool[num_pins];
    }
    
    void add_pin(unsigned index, IOPin pin) {
        if (index < num_pins) {
            pins[index] = pin;
        }
    }

    void configure_pins();

    uint16_t get_last_value(unsigned index) { return last_value[index]; }
    bool     get_value_changed(unsigned index) { return value_changed[index]; }

    uint16_t read_pin(unsigned index, bool * value_changed=0);
    bool     write_pin(unsigned index, uint16_t value);
    
    void     read_all_pins();
    
    bool     publish_pin_MQTT(unsigned index, MQTT *mqtt);
    void     publish_changed_MQTT(MQTT *mqtt, bool publish_all=false);
};

#endif  // _ParticleIO_h