#ifndef _LORA_SERVER_API_H
#define _LORA_SERVER_API_H

#include "../lora/LoRa.h"

#include <stdint.h>

class Lora_server_api {
    public:
        uint8_t rslt;

        Lora_server_api() {};
	void init();

	void print_rslt( const char api_name[], uint8_t rslt);

    private:

};
#endif
