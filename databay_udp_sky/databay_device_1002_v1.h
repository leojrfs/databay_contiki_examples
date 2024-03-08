// --- HYPER GENERATED CODE %VERSION% 1644756134 ---
//
// WARNING
//
// This file was automatically generated by the Hyper Platform.
// Manual changes MUST NOT be made, consult documentation at hyper.ag for more
// information.

#ifndef __DATABAY_DEVICE_1002__
#define __DATABAY_DEVICE_1002__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#ifndef DATABAY_DISABLE_PRINT
// Allow overriding printf.
#ifndef DATABAY_PRINTF
#include <stdio.h>
#define DATABAY_PRINTF(...) printf(__VA_ARGS__)
#endif
#endif

#ifndef __DATABAY_MSGPACK_HELPERS__
#define __DATABAY_MSGPACK_HELPERS__

#ifdef WORDS_BIGENDIAN
#define is_bigendian() (WORDS_BIGENDIAN)
#else
static const int32_t _i = 1;
#define is_bigendian() ((*(const char *)&_i) == 0)
#endif

static uint16_t be16(uint16_t x)
{
    char *b = (char *)&x;

    if (!is_bigendian())
    {
        char swap = b[0];
        b[0] = b[1];
        b[1] = swap;
    }

    return x;
}

static int16_t sbe16(int16_t x)
{
    return (int16_t)be16((uint16_t)x);
}

static uint32_t be32(uint32_t x)
{
    char *b = (char *)&x;

    if (!is_bigendian())
    {
        char swap = b[0];
        b[0] = b[3];
        b[3] = swap;

        swap = b[1];
        b[1] = b[2];
        b[2] = swap;
    }

    return x;
}

static int32_t sbe32(int32_t x)
{
    return (int32_t)be32((uint32_t)x);
}

#define MSGPACK_ARRAY_MARKER(element_nr) (0x90 + element_nr)
#define MSGPACK_MAP_MARKER(element_nr) (0x80 + element_nr)
#define MSGPACK_BOOL_MARKER_VAL(val) (0xc2 + (uint8_t)val)
#define MSGPACK_UINT8_MARKER 0xcc
#define MSGPACK_UINT16_MARKER 0xcd
#define MSGPACK_UINT32_MARKER 0xce
#define MSGPACK_UINT64_MARKER 0xcf
#define MSGPACK_INT8_MARKER 0xd0
#define MSGPACK_INT16_MARKER 0xd1
#define MSGPACK_INT32_MARKER 0xd2
#define MSGPACK_NIL 0xc0

static void msgpack_write_bool(uint8_t *data, uint8_t *seek, bool val)
{
    // set marker and val
    data[*seek] = MSGPACK_BOOL_MARKER_VAL(val);
    *seek = *seek + sizeof(bool);
}

static void msgpack_write_uint8(uint8_t *data, uint8_t *seek, uint8_t val)
{
    // do not set uint8 marker if val is less than 128 to spare some bytes
    if (val >= 128)
    {
        data[*seek] = MSGPACK_UINT8_MARKER;
        *seek = *seek + sizeof(uint8_t);
    }
    data[*seek] = val;
    *seek = *seek + sizeof(uint8_t);
}

static void msgpack_write_uint16(uint8_t *data, uint8_t *seek, uint16_t val)
{
    // set marker
    data[*seek] = MSGPACK_UINT16_MARKER;
    *seek = *seek + sizeof(uint8_t);
    // set data
    val = be16(val);
    memcpy(&data[*seek], &val, sizeof(uint16_t));
    *seek = *seek + sizeof(uint16_t);
}

static void msgpack_write_uint32(uint8_t *data, uint8_t *seek, uint32_t val)
{
    if (val <= 0xFF)
    {
        return msgpack_write_uint8(data, seek, (uint8_t)val);
    }
    if (val <= 0xFFFF)
    {
        return msgpack_write_uint16(data, seek, (uint16_t)val);
    }

    // set marker
    data[*seek] = MSGPACK_UINT32_MARKER;
    *seek = *seek + sizeof(uint8_t);
    // set data
    val = be32(val);
    memcpy(&data[*seek], &val, sizeof(uint32_t));
    *seek = *seek + sizeof(uint32_t);
}

static void msgpack_write_int8(uint8_t *data, uint8_t *seek, int8_t val)
{
    // set marker
    data[*seek] = MSGPACK_INT8_MARKER;
    *seek = *seek + sizeof(uint8_t);
    // set data
    data[*seek] = val;
    *seek = *seek + sizeof(int8_t);
}

static void msgpack_write_int16(uint8_t *data, uint8_t *seek, int16_t val)
{
    // set marker
    data[*seek] = MSGPACK_INT16_MARKER;
    *seek = *seek + sizeof(uint8_t);
    // set data
    val = sbe16(val);
    memcpy(&data[*seek], &val, sizeof(int16_t));
    *seek = *seek + sizeof(int16_t);
}

static void msgpack_write_int32(uint8_t *data, uint8_t *seek, int32_t val)
{
    if (val >= 0)
    {
        return msgpack_write_uint32(data, seek, val);
    }
    if (val >= -128)
    {
        return msgpack_write_int8(data, seek, (int8_t)val);
    }
    if (val >= -32768)
    {
        return msgpack_write_int16(data, seek, (int16_t)val);
    }
    // write int16 instead if <
    // set marker
    data[*seek] = MSGPACK_INT32_MARKER;
    *seek = *seek + sizeof(uint8_t);
    // set data
    val = sbe32(val);
    memcpy(&data[*seek], &val, sizeof(int32_t));
    *seek = *seek + sizeof(int32_t);
}

static bool msgpack_read_key(const uint8_t *data, uint8_t *seek, uint8_t *key)
{
    // might or might not have 0xcc marker, key will never get to 204 check is okay
    if (data[*seek] == MSGPACK_UINT8_MARKER)
    {
        *seek = *seek + sizeof(uint8_t);
    }
    *key = data[*seek];
    *seek = *seek + sizeof(uint8_t);

    return true;
}

static bool msgpack_read_uint8(const uint8_t *data, uint8_t *seek, uint8_t *val)
{
    // read marker only if value is > 128
    if (data[*seek] >= 128)
    {
        // read data type
        if (data[*seek] != MSGPACK_UINT8_MARKER)
        {
            return false;
        }
        *seek = *seek + sizeof(uint8_t);
    }
    *val = data[*seek];
    *seek = *seek + sizeof(uint8_t);

    return true;
}

static bool msgpack_read_uint16(const uint8_t *data, uint8_t *seek, uint16_t *val)
{
    // read data type
    if (data[*seek] != MSGPACK_UINT16_MARKER)
    {
        return false;
    }
    *seek = *seek + sizeof(uint8_t);
    memcpy(val, &data[*seek], sizeof(uint16_t));
    *val = be16(*val);
    *seek = *seek + sizeof(uint16_t);

    return true;
}

static bool msgpack_read_bool(const uint8_t *data, uint8_t *seek, bool *val)
{
    // read data type
    if ((data[*seek] != MSGPACK_BOOL_MARKER_VAL(false)) && (data[*seek] != MSGPACK_BOOL_MARKER_VAL(true)))
    {
        return false;
    }

    *val = data[*seek] - MSGPACK_BOOL_MARKER_VAL(false);
    *seek = *seek + sizeof(bool);

    return true;
}

#endif // __DATABAY_MSGPACK_HELPERS__

// --- DEVICE TYPE ---
typedef struct
{
    uint16_t schema_id;
    bool led_green_0_set : 1;
    bool led_green_0;
    bool led_yellow_1_set : 1;
    bool led_yellow_1;
    bool led_red_2_set : 1;
    bool led_red_2;
    bool temperature_3_set : 1;
    int32_t temperature_3;
    bool relative_humidity_4_set : 1;
    int32_t relative_humidity_4;
    bool photosynthetically_active_radiation_5_set : 1;
    int32_t photosynthetically_active_radiation_5;
    bool total_solar_radiation_6_set : 1;
    int32_t total_solar_radiation_6;
    bool battery_level_7_set : 1;
    int32_t battery_level_7;
    bool button_8_set : 1;
    bool button_8;
    bool uptime_9_set : 1;
    uint32_t uptime_9;
    bool publish_interval_10_set : 1;
    uint16_t publish_interval_10;

    void (*on_led_green_0_update)(bool);
    void (*on_led_yellow_1_update)(bool);
    void (*on_led_red_2_update)(bool);
    void (*on_publish_interval_10_update)(uint16_t);

} databay_device_1002_t;

void databay_device_1002_reset(databay_device_1002_t *dev)
{
    dev->schema_id = 1002;
    dev->led_green_0_set = false;
    dev->led_green_0 = 0;
    dev->led_yellow_1_set = false;
    dev->led_yellow_1 = 0;
    dev->led_red_2_set = false;
    dev->led_red_2 = 0;
    dev->temperature_3_set = false;
    dev->temperature_3 = 0;
    dev->relative_humidity_4_set = false;
    dev->relative_humidity_4 = 0;
    dev->photosynthetically_active_radiation_5_set = false;
    dev->photosynthetically_active_radiation_5 = 0;
    dev->total_solar_radiation_6_set = false;
    dev->total_solar_radiation_6 = 0;
    dev->battery_level_7_set = false;
    dev->battery_level_7 = 0;
    dev->button_8_set = false;
    dev->button_8 = 0;
    dev->uptime_9_set = false;
    dev->uptime_9 = 0;
    dev->publish_interval_10_set = false;
    dev->publish_interval_10 = 0;
}

void databay_device_1002_init(databay_device_1002_t *dev)
{
    databay_device_1002_reset(dev);
    dev->on_led_green_0_update = NULL;
    dev->on_led_yellow_1_update = NULL;
    dev->on_led_red_2_update = NULL;
    dev->on_publish_interval_10_update = NULL;
}

// --- BIND ATTRIBUTES ---
void databay_device_1002_bind_led_green_0(databay_device_1002_t *device, void (*callback)(bool))
{
    device->on_led_green_0_update = callback;
}

void databay_device_1002_bind_led_yellow_1(databay_device_1002_t *device, void (*callback)(bool))
{
    device->on_led_yellow_1_update = callback;
}

void databay_device_1002_bind_led_red_2(databay_device_1002_t *device, void (*callback)(bool))
{
    device->on_led_red_2_update = callback;
}

void databay_device_1002_bind_publish_interval_10(databay_device_1002_t *device, void (*callback)(uint16_t))
{
    device->on_publish_interval_10_update = callback;
}

// --- SET ATTRIBUTES ---
void databay_device_1002_set_led_green_0(databay_device_1002_t *device, bool value)
{
    device->led_green_0_set = true;
    device->led_green_0 = value;
}

void databay_device_1002_set_led_yellow_1(databay_device_1002_t *device, bool value)
{
    device->led_yellow_1_set = true;
    device->led_yellow_1 = value;
}

void databay_device_1002_set_led_red_2(databay_device_1002_t *device, bool value)
{
    device->led_red_2_set = true;
    device->led_red_2 = value;
}

void databay_device_1002_set_temperature_3(databay_device_1002_t *device, int32_t value)
{
    device->temperature_3_set = true;
    device->temperature_3 = value;
}

void databay_device_1002_set_relative_humidity_4(databay_device_1002_t *device, int32_t value)
{
    device->relative_humidity_4_set = true;
    device->relative_humidity_4 = value;
}

void databay_device_1002_set_photosynthetically_active_radiation_5(databay_device_1002_t *device, int32_t value)
{
    device->photosynthetically_active_radiation_5_set = true;
    device->photosynthetically_active_radiation_5 = value;
}

void databay_device_1002_set_total_solar_radiation_6(databay_device_1002_t *device, int32_t value)
{
    device->total_solar_radiation_6_set = true;
    device->total_solar_radiation_6 = value;
}

void databay_device_1002_set_battery_level_7(databay_device_1002_t *device, int32_t value)
{
    device->battery_level_7_set = true;
    device->battery_level_7 = value;
}

void databay_device_1002_set_button_8(databay_device_1002_t *device, bool value)
{
    device->button_8_set = true;
    device->button_8 = value;
}

void databay_device_1002_set_uptime_9(databay_device_1002_t *device, uint32_t value)
{
    device->uptime_9_set = true;
    device->uptime_9 = value;
}

void databay_device_1002_set_publish_interval_10(databay_device_1002_t *device, uint16_t value)
{
    device->publish_interval_10_set = true;
    device->publish_interval_10 = value;
}

// --- ENCODE DEVICE ---
int databay_device_1002_encode(databay_device_1002_t *dev, uint8_t *out, uint8_t *out_len)
{
    // Encode message as array of elements.
    out[0] = MSGPACK_ARRAY_MARKER(2);
    // schema id
    out[1] = MSGPACK_UINT16_MARKER;
    uint16_t sid = be16(dev->schema_id);
    memcpy(&out[2], &(sid), sizeof(uint16_t));

    // map size
    uint8_t defined_attributes_count = 0;
    if (dev->led_green_0_set)
        ++defined_attributes_count;
    if (dev->led_yellow_1_set)
        ++defined_attributes_count;
    if (dev->led_red_2_set)
        ++defined_attributes_count;
    if (dev->temperature_3_set)
        ++defined_attributes_count;
    if (dev->relative_humidity_4_set)
        ++defined_attributes_count;
    if (dev->photosynthetically_active_radiation_5_set)
        ++defined_attributes_count;
    if (dev->total_solar_radiation_6_set)
        ++defined_attributes_count;
    if (dev->battery_level_7_set)
        ++defined_attributes_count;
    if (dev->button_8_set)
        ++defined_attributes_count;
    if (dev->uptime_9_set)
        ++defined_attributes_count;
    if (dev->publish_interval_10_set)
        ++defined_attributes_count;

    out[4] = MSGPACK_MAP_MARKER(defined_attributes_count);

    *out_len = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t);

    // 0: led_green_0
    if (dev->led_green_0_set)
    {
        // map element 0
        msgpack_write_uint8(out, out_len, 0);
        msgpack_write_bool(out, out_len, dev->led_green_0);
    }

    // 1: led_yellow_1
    if (dev->led_yellow_1_set)
    {
        // map element 1
        msgpack_write_uint8(out, out_len, 1);
        msgpack_write_bool(out, out_len, dev->led_yellow_1);
    }

    // 2: led_red_2
    if (dev->led_red_2_set)
    {
        // map element 2
        msgpack_write_uint8(out, out_len, 2);
        msgpack_write_bool(out, out_len, dev->led_red_2);
    }

    // 3: temperature_3_set
    if (dev->temperature_3_set)
    {
        // map element 3
        msgpack_write_uint8(out, out_len, 3);
        msgpack_write_int32(out, out_len, dev->temperature_3);
    }

    // 4: relative_humidity_4_set
    if (dev->relative_humidity_4_set)
    {
        // map element 4
        msgpack_write_uint8(out, out_len, 4);
        msgpack_write_int32(out, out_len, dev->relative_humidity_4);
    }

    // 5: photosynthetically_active_radiation_5_set
    if (dev->photosynthetically_active_radiation_5_set)
    {
        // map element 5
        msgpack_write_uint8(out, out_len, 5);
        msgpack_write_int32(out, out_len, dev->photosynthetically_active_radiation_5);
    }

    // 6: total_solar_radiation_6_set
    if (dev->total_solar_radiation_6_set)
    {
        // map element 6
        msgpack_write_uint8(out, out_len, 6);
        msgpack_write_int32(out, out_len, dev->total_solar_radiation_6);
    }

    // 7: battery_level_7_set
    if (dev->battery_level_7_set)
    {
        // map element 7
        msgpack_write_uint8(out, out_len, 7);
        msgpack_write_int32(out, out_len, dev->battery_level_7);
    }

    // 8: button_8_set
    if (dev->button_8_set)
    {
        // map element 8
        msgpack_write_uint8(out, out_len, 8);
        msgpack_write_bool(out, out_len, dev->button_8);
    }

    // 9: uptime_9_set
    if (dev->uptime_9_set)
    {
        // map element 9
        msgpack_write_uint8(out, out_len, 9);
        msgpack_write_uint32(out, out_len, dev->uptime_9);
    }

    // 10: publish_interval_10_set
    if (dev->publish_interval_10_set)
    {
        // map element 10
        msgpack_write_uint8(out, out_len, 10);
        msgpack_write_uint16(out, out_len, dev->publish_interval_10);
    }

    return 0;
}

// --- DECODE DEVICE ---
int databay_device_1002_decode(databay_device_1002_t *dev, const uint8_t *in, uint8_t in_size)
{
    uint8_t seek = 0;
    if ((in[seek] == MSGPACK_NIL) && (in_size == 1))
    {
        // Received NIL, no data to decode
        return 0;
    }
    // TODO: check sizes

    // check array
    if (in[seek] != MSGPACK_ARRAY_MARKER(3))
    {
        return 1;
    }
    seek = seek + sizeof(uint8_t);

    // check time marker and skip data bytes
    if (in[seek] == MSGPACK_UINT64_MARKER)
    {
        seek = seek + sizeof(uint64_t) + sizeof(uint8_t);
    }
    else if (in[seek] == MSGPACK_UINT32_MARKER)
    {
        seek = seek + sizeof(uint32_t) + sizeof(uint8_t);
    }
    else
    {
        return 2;
    }

    // check and match schema_id
    if ((in[seek] != MSGPACK_UINT16_MARKER) || (in[seek + 1] != 0x03) || (in[seek + 2] != 0xea))
    {
        return 3;
    }

    seek = seek + sizeof(uint8_t) + sizeof(uint16_t);

    // check map nr of items
    uint8_t map_nr_of_items = in[seek] - MSGPACK_MAP_MARKER(0);
    // max 4 items (3 leds + publish_interval)
    if ((map_nr_of_items == 0) || (map_nr_of_items > 4))
    {
        return 4;
    }

    seek = seek + sizeof(uint8_t);

    uint8_t key = 0xFF;
    for (uint8_t i = 0; i < map_nr_of_items; ++i)
    {
        if (msgpack_read_key(in, &seek, &key))
        {
            switch (key)
            {
            case 0: // led_green_0
                if (msgpack_read_bool(in, &seek, &dev->led_green_0))
                {
                    dev->led_green_0_set = true;
                }
                else
                {
                    return 5;
                }
                break;
            case 1: // led_yellow_1
                if (msgpack_read_bool(in, &seek, &dev->led_yellow_1))
                {
                    dev->led_yellow_1_set = true;
                }
                else
                {
                    return 6;
                }
                break;
            case 2: // led_red_2
                if (msgpack_read_bool(in, &seek, &dev->led_red_2))
                {
                    dev->led_red_2_set = true;
                }
                else
                {
                    return 7;
                }
                break;
            case 10: // publish_interval_10
                if (msgpack_read_uint16(in, &seek, &dev->publish_interval_10))
                {
                    dev->publish_interval_10_set = true;
                }
                else
                {
                    uint8_t tmp;
                    if (msgpack_read_uint8(in, &seek, &tmp))
                    {
                        dev->publish_interval_10 = tmp;
                        dev->publish_interval_10_set = true;
                    }
                    else
                    {
                        return 8;
                    }
                }
                break;
            default:
                return 9;
            }
        }
    }

    return 0;
}

// --- DISPATCH DEVICE ---
uint8_t databay_device_1002_dispatch(databay_device_1002_t *dev)
{
    uint8_t count = 0;
    if (dev->on_led_green_0_update && dev->led_green_0_set)
    {
        dev->on_led_green_0_update(dev->led_green_0);
        ++count;
    }
    if (dev->on_led_yellow_1_update && dev->led_yellow_1_set)
    {
        dev->on_led_yellow_1_update(dev->led_yellow_1);
        ++count;
    }
    if (dev->on_led_red_2_update && dev->led_red_2_set)
    {
        dev->on_led_red_2_update(dev->led_red_2);
        ++count;
    }
    if (dev->on_publish_interval_10_update && dev->publish_interval_10_set)
    {
        dev->on_publish_interval_10_update(dev->publish_interval_10);
        ++count;
    }
    return count;
}

void databay_device_1002_print(databay_device_1002_t *dev)
{
#if DATABAY_DISABLE_PRINT
#else
    DATABAY_PRINTF(
        "(databay_device_1002_t\n"
        "  (schema_id %lld)\n",
        (long long int)dev->schema_id);

    if (dev->led_green_0_set)
    {
        DATABAY_PRINTF("  (led_green_0 (Bool %s))\n", dev->led_green_0 ? "true" : "false");
    }
    else
    {
        DATABAY_PRINTF("  (led_green_0 (Bool none))\n");
    }
    if (dev->led_yellow_1_set)
    {
        DATABAY_PRINTF("  (led_yellow_1 (Bool %s))\n", dev->led_yellow_1 ? "true" : "false");
    }
    else
    {
        DATABAY_PRINTF("  (led_yellow_1 (Bool none))\n");
    }
    if (dev->led_red_2_set)
    {
        DATABAY_PRINTF("  (led_red_2 (Bool %s))\n", dev->led_red_2 ? "true" : "false");
    }
    else
    {
        DATABAY_PRINTF("  (led_red_2 (Bool none))\n");
    }

    if (dev->temperature_3_set)
    {
        DATABAY_PRINTF("  (temperature_3 (Int32 %" PRIi32 "))\n", dev->temperature_3);
    }
    else
    {
        DATABAY_PRINTF("  (temperature_3 (Int32 none))\n");
    }

    if (dev->relative_humidity_4_set)
    {
        DATABAY_PRINTF("  (relative_humidity_4 (Int32 %" PRIi32 "))\n", dev->relative_humidity_4);
    }
    else
    {
        DATABAY_PRINTF("  (relative_humidity_4 (Int32 none))\n");
    }

    if (dev->photosynthetically_active_radiation_5_set)
    {
        DATABAY_PRINTF("  (photosynthetically_active_radiation_5 (Int32 %" PRIi32 "))\n", dev->photosynthetically_active_radiation_5);
    }
    else
    {
        DATABAY_PRINTF("  (photosynthetically_active_radiation_5 (Int32 none))\n");
    }

    if (dev->total_solar_radiation_6_set)
    {
        DATABAY_PRINTF("  (total_solar_radiation_6 (Int32 %" PRIi32 "))\n", dev->total_solar_radiation_6);
    }
    else
    {
        DATABAY_PRINTF("  (total_solar_radiation_6 (Int32 none))\n");
    }

    if (dev->battery_level_7_set)
    {
        DATABAY_PRINTF("  (battery_level_7 (Int32 %" PRIi32 "))\n", dev->battery_level_7);
    }
    else
    {
        DATABAY_PRINTF("  (battery_level_7 (Int32 none))\n");
    }

    if (dev->button_8_set)
    {
        DATABAY_PRINTF("  (button_8 (Bool %s))\n", dev->button_8 ? "true" : "false");
    }
    else
    {
        DATABAY_PRINTF("  (button_8 (Bool none))\n");
    }

    if (dev->uptime_9_set)
    {
        DATABAY_PRINTF("  (uptime_9 (Uint32 %" PRIu32 "))\n", dev->uptime_9);
    }
    else
    {
        DATABAY_PRINTF("  (uptime_9 (Uint32 none))\n");
    }

    if (dev->publish_interval_10_set)
    {
        DATABAY_PRINTF("  (publish_interval_10 (Uint16 %" PRIu16 "))\n", dev->publish_interval_10);
    }
    else
    {
        DATABAY_PRINTF("  (publish_interval_10 (Uint16 none))\n");
    }
    DATABAY_PRINTF(")\n");
#endif
}

#endif // __DATABAY_DEVICE_1002__
