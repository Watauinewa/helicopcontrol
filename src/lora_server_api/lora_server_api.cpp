#include "lora_server_api.h"

void Lora_server_api::init() {
    LoRa.setPins( 10, 9, 2);
//     LoRa.setSPI( spi);

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
