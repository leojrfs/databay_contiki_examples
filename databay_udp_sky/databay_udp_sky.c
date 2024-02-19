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
 *      Databay UDP Tmote Sky App
 * \author
 *      Leo Soares <leo@databay.dev>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project-conf.h"
#include "contiki.h"
#include "contiki-net.h"
#include "net/routing/routing.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include "dev/light-sensor.h"
#include "dev/battery-sensor.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "app"
#define LOG_LEVEL LOG_LEVEL_APP

#include "net/ipv6/uip.h"
static uip_ipaddr_t dest_ipaddr;
static struct simple_udp_connection udp_conn;
#define UDP_CLIENT_PORT 8765

static struct etimer publish_etimer;
static uint8_t publish_interval = APP_DEFAULT_PUBLISH_INTERVAL;

#define DATABAY_PRINTF(...) printf(__VA_ARGS__)
#include "databay_device_1002_v1.h"
databay_device_1002_t databay_device;

static uint8_t udp_msg_buff[UDP_MSG_BUFF_LEN];

static void on_publish_interval_10_update(uint16_t value)
{
  // dont allow publish interval to be 0 or more than 255 seconds
  if ((value > 0) && (value < UINT8_MAX))
  {
    LOG_INFO(">>> ACTION: on_publish_interval_10_update call with value='%" PRIu16 "'\n", value);
    publish_interval = value;
    etimer_set(&publish_etimer, publish_interval * CLOCK_SECOND);
    etimer_reset(&publish_etimer);
  }
  else
  {
    LOG_ERR(">>> ACTION: on_publish_interval_10_update call with invalid value='%" PRIu16 "'!\n", value);
  }
}

static void on_led_green_0_update(bool value)
{
  LOG_INFO(">>> ACTION: on_led_green_0_update call with value='%s'\n", value ? "true" : "false");
  if (value)
  {
    leds_on(LEDS_GREEN);
  }
  else
  {
    leds_off(LEDS_GREEN);
  }
}

static void on_led_yellow_1_update(bool value)
{
  LOG_INFO(">>> ACTION: on_led_yellow_1_update call with value='%s'\n", value ? "true" : "false");
  if (value)
  {
    leds_on(LEDS_YELLOW);
  }
  else
  {
    leds_off(LEDS_YELLOW);
  }
}

static void on_led_red_2_update(bool value)
{
  LOG_INFO(">>> ACTION: on_led_red_2_update call with value='%s'\n", value ? "true" : "false");
  if (value)
  {
    leds_on(LEDS_RED);
  }
  else
  {
    leds_off(LEDS_RED);
  }
}

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
  LOG_DBG("Received response (%" PRIu16 "): ", datalen);
  if (datalen < 100)
  {
    LOG_DBG_BYTES(data, datalen);
    LOG_DBG_("\n");
    databay_device_1002_reset(&databay_device);
    int ret = databay_device_1002_decode(&databay_device, data, datalen);
    if (ret)
    {
      LOG_ERR("databay message decode failed with error: %d\n", ret);
      return;
    }
    databay_device_1002_dispatch(&databay_device);
    databay_device_1002_print(&databay_device);
  }
}

PROCESS(databay_udp_sky_app, "Databay UDP Tmote Sky App");
AUTOSTART_PROCESSES(&databay_udp_sky_app);
PROCESS_THREAD(databay_udp_sky_app, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&publish_etimer, publish_interval * CLOCK_SECOND);

  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor);
  SENSORS_ACTIVATE(light_sensor);
  SENSORS_ACTIVATE(battery_sensor);

  // Initialize UDP connection
  uip_ip6addr(&dest_ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0x1);
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      DATABAY_PROXY_UDP_SERVER_PORT, udp_rx_callback);

  // init databay
  databay_device_1002_init(&databay_device);
  databay_device_1002_bind_led_green_0(&databay_device, on_led_green_0_update);
  databay_device_1002_bind_led_yellow_1(&databay_device, on_led_yellow_1_update);
  databay_device_1002_bind_led_red_2(&databay_device, on_led_red_2_update);
  databay_device_1002_bind_publish_interval_10(&databay_device, on_publish_interval_10_update);

  LOG_INFO("Press a button to send a message\n");

  while (1)
  {
    PROCESS_YIELD();

    if (etimer_expired(&publish_etimer))
    {
      LOG_INFO("Publish Timer Event Triggered\n");

      // reset data
      databay_device_1002_reset(&databay_device);

      int val = sht11_sensor.value(SHT11_SENSOR_TEMP);
      if (val != -1)
      {
        LOG_DBG("sht11_temp_int: %d\n", val);
        databay_device_1002_set_temperature_3(&databay_device, val);
      }
      val = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
      if (val != -1)
      {
        LOG_DBG("sht11_humidity_int: %d\n", val);
        databay_device_1002_set_relative_humidity_4(&databay_device, val);
      }

      val = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
      if (val != -1)
      {
        databay_device_1002_set_photosynthetically_active_radiation_5(&databay_device, val);
        LOG_DBG("light_photosynthetic_int: %d\n", val);
      }

      val = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
      if (val != -1)
      {
        databay_device_1002_set_total_solar_radiation_6(&databay_device, val);
        LOG_DBG("total_solar_radiation_int: %d\n", val);
      }

      val = battery_sensor.value(0);
      if (val != -1)
      {
        databay_device_1002_set_battery_level_7(&databay_device, val);
        LOG_DBG("battery_level_int: %d\n", val);
      }

      uint8_t leds_state = leds_get();
      LOG_DBG("led_green_0: %s\n", (leds_state & LEDS_GREEN) == LEDS_GREEN ? "true" : "false");
      databay_device_1002_set_led_green_0(&databay_device, (leds_state & LEDS_GREEN) == LEDS_GREEN);
      LOG_DBG("led_yellow_1: %s\n", (leds_state & LEDS_YELLOW) == LEDS_YELLOW ? "true" : "false");
      databay_device_1002_set_led_yellow_1(&databay_device, (leds_state & LEDS_YELLOW) == LEDS_YELLOW);
      LOG_DBG("led_red_2: %s\n", (leds_state & LEDS_RED) == LEDS_RED ? "true" : "false");
      databay_device_1002_set_led_red_2(&databay_device, (leds_state & LEDS_RED) == LEDS_RED);

      databay_device_1002_set_uptime_9(&databay_device, clock_seconds());
      databay_device_1002_set_publish_interval_10(&databay_device, publish_interval);

      if (NETSTACK_ROUTING.node_is_reachable())
      {
        // encode data before sending
        uint8_t udp_msg_buff_len = 0;
        databay_device_1002_encode(&databay_device, udp_msg_buff, &udp_msg_buff_len);
        LOG_DBG("Sending UDP Packet with payload: '");
        LOG_DBG_BYTES(udp_msg_buff, udp_msg_buff_len);
        LOG_DBG_("'\n");
        simple_udp_sendto(&udp_conn, udp_msg_buff, udp_msg_buff_len, &dest_ipaddr);
      }
      else
      {
        LOG_ERR("Node not connected to the network yet.\n");
      }

      etimer_reset(&publish_etimer);
    }
    else if (ev == sensors_event && data == &button_sensor)
    {
      LOG_INFO("Button Pressed Event Triggered\n");

      if (NETSTACK_ROUTING.node_is_reachable())
      {
        // reset data
        databay_device_1002_reset(&databay_device);
        databay_device_1002_set_button_8(&databay_device, true);

        // encode data before sending
        uint8_t udp_msg_buff_len = 0;
        databay_device_1002_encode(&databay_device, udp_msg_buff, &udp_msg_buff_len);
        LOG_DBG("Sending UDP Packet with payload: '");
        LOG_DBG_BYTES(udp_msg_buff, udp_msg_buff_len);
        LOG_DBG_("'\n");
        simple_udp_sendto(&udp_conn, udp_msg_buff, udp_msg_buff_len, &dest_ipaddr);
      }
      else
      {
        LOG_ERR("Node not connected to the network yet.\n");
      }
    }
  }

  PROCESS_END();
}
