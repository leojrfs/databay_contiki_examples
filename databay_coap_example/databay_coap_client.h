#ifndef __DATABAY_COAP_CLIENT__
#define __DATABAY_COAP_CLIENT__
#include <stdlib.h>
void databay_coap_client_init(void (*on_response_data_handler_cb)(uint8_t *data, uint8_t data_len));
uint16_t databay_coap_post(const char *endpoint_str, const char *uri_path,
                           const char *databay_token, void *payload, size_t payload_len);
#endif // __DATABAY_COAP_CLIENT__