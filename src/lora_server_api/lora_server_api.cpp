#include "lora_server_api.h"

Lora_server_api::Lora_server_api( SPIClass &spi) {
    this->spi = &spi;
}


void Lora_server_api::init() {
    // LoRa.setPins(ss, reset, dio0);
    LoRa.setPins( 9, 10, 11);
    LoRa.setSPI(*spi);

    rslt = (uint8_t)LoRa.begin( 915E6);
    print_rslt("lora_server_api::init", rslt);

}

void Lora_server_api::print_rslt( const char app_name[], uint8_t rslt) {
    if( rslt != 1) {    
        Serial.printf("%s\t", app_name);

        switch(rslt) {
            default:
                Serial.printf("Error[%d]\n");
        }
    }
}
