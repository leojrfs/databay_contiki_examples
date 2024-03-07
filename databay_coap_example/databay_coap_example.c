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
 *      Databay CoAP Example.
 * \author
 *      Leo Soares <leo@databay.dev>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project-conf.h"
#include "contiki.h"
#include "contiki-net.h"
#include "dev/button-hal.h"
#include "dev/leds.h"
#if PLATFORM_HAS_TEMPERATURE
#include "common/temperature-sensor.h"
#endif

#include "sys/log.h"
/* Log configuration */
#define LOG_MODULE "app"
#define LOG_LEVEL LOG_LEVEL_APP

#include "databay_coap_client.h"
static char coap_server_uri_path[COAP_SERVER_URI_MAX_LEN] = {0};

#define DATABAY_PRINTF(...) LOG_INFO_(__VA_ARGS__)
#include "databay_device_1000_v1.gen.h"
databay_device_1000_t databay_device = {0};

static struct etimer publish_etimer;
static uint8_t publish_interval = APP_DEFAULT_PUBLISH_INTERVAL;
static bool led_red_state = false;
static bool led_green_state = false;

static void on_publish_interval_update(int64_t value)
{
  // dont allow publish interval to be 0 or more than 255 seconds
  if ((value > 0) || (value > UINT8_MAX))
  {
    LOG_INFO(">>> ACTION: on_publish_interval_update call with value='%lld'", value);
    publish_interval = value;
    etimer_set(&publish_etimer, publish_interval * CLOCK_SECOND);
    etimer_reset(&publish_etimer);
  }
  else
  {
    LOG_ERR(">>> ACTION: on_publish_interval_update call with invalid value='%lld'!", value);
  }
}

static void on_led_red_update(bool value)
{
  LOG_INFO(">>> ACTION: on_led_red_update call with value='%s'", value ? "true" : "false");
  led_red_state = value;
  if (led_red_state)
  {
    leds_on(LEDS_RED);
  }
  else
  {
    leds_off(LEDS_RED);
  }
}

static void on_led_green_update(bool value)
{
  LOG_INFO(">>> ACTION: on_led_green_update call with value='%s'", value ? "true" : "false");
  led_green_state = value;
  if (led_green_state)
  {
    leds_on(LEDS_GREEN);
  }
  else
  {
    leds_off(LEDS_GREEN);
  }
}

void coap_response_callback(uint8_t *data, uint8_t data_len)
{
  if (data_len > 0)
  {
    LOG_INFO("coap_response_callback hex (size: %u): ", data_len);
    LOG_INFO_BYTES(data, data_len);
    LOG_INFO_("\n");
    databay_device_1000_reset(&databay_device);
    databay_result_t res = databay_device_1000_decode(&databay_device, data, data_len);
    if (res != DATABAY_OK)
    {
      LOG_ERR("exiting on databay_device_decode error...\n");
      return;
    }
    databay_device_1000_dispatch(&databay_device);
    LOG_INFO("Device after update:\n");
    databay_device_1000_print(&databay_device);
  }
}

void databay_publish(databay_device_1000_t *dev, const char *uri_path)
{
  // TODO: better databay_msg size.
  uint8_t databay_msg[sizeof(databay_device_1000_t) * 2];
  uint8_t databay_msg_len = 0;
  if (databay_device_1000_encode(dev, databay_msg, &databay_msg_len) == DATABAY_OK)
  {
    databay_device_1000_print(&databay_device);
    databay_coap_post(DATABAY_COAP_SERVER_ENDPOINT, uri_path, DATABAY_ACCESS_TOKEN, databay_msg, databay_msg_len);
  }
  else
  {
    LOG_ERR("error while encoding data");
  }
  databay_device_1000_reset(dev);
}

PROCESS(databay_example_app, "Databay example app");
AUTOSTART_PROCESSES(&databay_example_app);
PROCESS_THREAD(databay_example_app, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&publish_etimer, publish_interval * CLOCK_SECOND);

  // get macaddr in hex string
  char device_id[13];
  snprintf(device_id, sizeof(device_id), "%02x%02x%02x%02x%02x%02x",
           linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
           linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
           linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);
  LOG_DBG("device_id: %s\n", device_id);
  strcat(coap_server_uri_path, COAP_SERVER_URI_BASE_PATH);
  strcat(coap_server_uri_path, device_id);

  // init coap_client
  databay_coap_client_init(coap_response_callback);

  // init databay device
  databay_device_1000_init(&databay_device);
  databay_device_1000_bind_led_red(&databay_device, on_led_red_update);
  databay_device_1000_bind_led_green(&databay_device, on_led_green_update);
  databay_device_1000_bind_publish_interval(&databay_device, on_publish_interval_update);

  LOG_INFO("Press a button to request '%s/%s'\n", DATABAY_COAP_SERVER_ENDPOINT, coap_server_uri_path);
  while (1)
  {
    PROCESS_YIELD();

    if (etimer_expired(&publish_etimer))
    {
      LOG_INFO("Publish Timer Event Triggered\n");

      databay_device_1000_reset(&databay_device);
#if PLATFORM_HAS_TEMPERATURE
#ifdef NRF52840_H
      // on nrf52, internal sensor resolution is 0.25°C
      databay_device_1000_set_temperature(&databay_device, temperature_sensor.value(0) * 0.25f);
#endif
#endif
      databay_device_1000_set_led_red(&databay_device, led_red_state);
      databay_device_1000_set_led_green(&databay_device, led_green_state);
      databay_device_1000_set_publish_interval(&databay_device, publish_interval);
      databay_device_1000_set_uptime(&databay_device, clock_seconds());
      databay_publish(&databay_device, coap_server_uri_path);
      etimer_reset(&publish_etimer);
    }
    if (ev == button_hal_release_event)
    {
      LOG_INFO("Button Pressed Event Triggered\n");
      databay_device_1000_reset(&databay_device);
      databay_device_1000_set_button(&databay_device, true);
      databay_publish(&databay_device, coap_server_uri_path);
    }
  }

  PROCESS_END();
}
