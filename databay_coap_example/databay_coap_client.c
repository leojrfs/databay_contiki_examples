/*
 * Copyright (c) 2024, databay.dev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *      CoAP Client Helper functions.
 * \author
 *      Leo Soares <leo@databay.dev>
 */

#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "coap-blocking-api.h"
#include "coap-log.h"
#include "databay_coap_client.h"

#define LOG_MODULE "db_coap_client"
#define LOG_LEVEL LOG_LEVEL_INFO

static coap_endpoint_t server_ep;
static coap_message_t request;
void (*on_response_data_handler)(uint8_t *data, uint8_t data_len) = NULL;

static void coap_request_cb(coap_message_t *response)
{
    LOG_DBG_("[coap_request_cb]\n");
    coap_status_t response_code = response->code;
    const uint8_t *data;
    int data_len = coap_get_payload(response, &data);
    // TODO: print response_code string and mid
    switch (response_code)
    {
    case NO_ERROR:
        break;
    case CONTENT_2_05:

        if (data_len > 0)
        {
            on_response_data_handler((uint8_t *)data, data_len);
        }
        else
        {
            on_response_data_handler(NULL, 0);
            LOG_INFO("Response code='%d' -- No data received!\n", response_code);
        }
        break;
    default:
        if (data_len > 0)
        {
            LOG_INFO("CoAP Payload hex (len=%u): '", data_len);
            LOG_INFO_BYTES(data, data_len);
            LOG_INFO_("'\n");
            LOG_INFO("CoAP Payload string (len=%u): '%.*s'\n", data_len, (int)data_len, data);
        }
        else
        {
            LOG_INFO("Response code='%d' -- No data received!\n", response_code);
        }
    }
}

void databay_coap_client_init(void (*on_response_data_handler_cb)(uint8_t *data, uint8_t data_len))
{
    on_response_data_handler = on_response_data_handler_cb;
}

PROCESS(databay_coap_client_send_request_process, "databay_coap_client_send_request process");
PROCESS_THREAD(databay_coap_client_send_request_process, ev, data)
{
    // Process start
    PROCESS_BEGIN();

    // blocks here until completed
    COAP_BLOCKING_REQUEST(&server_ep, &request, coap_request_cb);

    // Process end
    PROCESS_END();
}

// CoAP POST, returns mid if successful, 0 otherwise
uint16_t databay_coap_post(const char *endpoint_str, const char *uri_path,
                           const char *databay_token, void *payload, size_t payload_len)
{
    if (coap_endpoint_parse(endpoint_str, strlen(endpoint_str), &server_ep) == 0)
    {
        LOG_ERR("Failed parsing CoAP endpoint `coap_endpoint_parse()`!");
        return 0;
    }
    coap_init_message(&request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(&request, uri_path);
    coap_set_header_uri_query(&request, databay_token);
    coap_set_payload(&request, payload, payload_len);
    LOG_INFO("CoAP POST '%s/%s'\n", endpoint_str, uri_path);
    LOG_INFO("payload hex (size: %u): '", payload_len);
    LOG_INFO_BYTES(payload, payload_len);
    LOG_INFO_("'\n");
    // Start the thread
    process_start(&databay_coap_client_send_request_process, NULL);

    return request.mid;
}