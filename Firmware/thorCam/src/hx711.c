/*
    FILE UNUSED IN PROJECT
*/

#include "hx711.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void hx711_init(hx711_t *hx, uint gpio_dout, uint gpio_sck, const char *name) {
    hx->gpio_dout = gpio_dout;
    hx->gpio_sck = gpio_sck;
    hx->name = name;
    hx->gain = 1;
    hx->offset = 0;
    hx->scale = 1.0;
    gpio_init(gpio_dout);
    gpio_set_dir(gpio_dout, GPIO_IN);
    // gpio_init(gpio_sck);
    // gpio_set_dir(gpio_sck, GPIO_OUT);
}

void hx711_set_gain(hx711_t *hx, uint8_t gain) {
    switch (gain) {
        case 128:
            hx->gain = 1; // Channel A, gain factor 128
            break;
        case 64:
            hx->gain = 3; // Channel A, gain factor 64
            break;
        case 32:
            hx->gain = 2; // Channel B, gain factor 32
            break;
        default:
            hx->gain = 1; // Default to Channel A, gain factor 128
            break;
    }
}

bool hx711_is_ready(hx711_t *hx) {
    return gpio_get(hx->gpio_dout) == 0;
}

void hx711_wait_ready(hx711_t *hx, unsigned long delay_ms) {
    while (!hx711_is_ready(hx)) {
        sleep_ms(delay_ms);
    }
}

bool hx711_wait_ready_retry(hx711_t *hx, int retries, unsigned long delay_ms) {
    int count = 0;
    while (count < retries) {
        if (hx711_is_ready(hx)) {
            return true;
        }
        sleep_ms(delay_ms);
        count++;
    }
    return false;
}

bool hx711_wait_ready_timeout(hx711_t *hx, unsigned long timeout, unsigned long delay_ms) {
    unsigned long start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < timeout) {
        if (hx711_is_ready(hx)) {
            return true;
        }
        sleep_ms(delay_ms);
    }
    return false;
}

int32_t hx711_read_raw(hx711_t *hx) {
    int32_t count = 0;
    uint8_t data[3] = {0};
    uint8_t filler = 0x00;

    hx711_wait_ready(hx, 0);

    for (int i = 0; i < 24; i++) {
        gpio_put(hx->gpio_sck, 1);
        count = count << 1;
        gpio_put(hx->gpio_sck, 0);
        if (gpio_get(hx->gpio_dout)) {
            count++;
        }
    }

    for (int i = 0; i < hx->gain; i++) {
        gpio_put(hx->gpio_sck, 1);
        gpio_put(hx->gpio_sck, 0);
    }

    if (count & 0x800000) {
        count |= ~0xffffff;
    }

    return count;
}

int32_t hx711_read_average(hx711_t *hx, uint8_t times) {
    int64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read_raw(hx);
    }
    return sum / times;
}

double hx711_get_value(hx711_t *hx, uint8_t times) {
    return hx711_read_average(hx, times) - hx->offset;
}

float hx711_get_units(hx711_t *hx, uint8_t times) {
    return hx711_get_value(hx, times) / hx->scale;
}

void hx711_tare(hx711_t *hx, uint8_t times) {
    printf("Starting tare process for %s...\n", hx->name);
    int32_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        int32_t reading = hx711_read_raw(hx);
        printf("Tare Reading %d: %d\n", i + 1, reading);
        sum += reading;
    }
    hx->offset = sum / times;
    printf("Tare complete. Calculated Offset: %d\n", hx->offset);
}

void hx711_set_scale(hx711_t *hx, float scale) {
    hx->scale = scale;
}

float hx711_get_scale(hx711_t *hx) {
    return hx->scale;
}

void hx711_set_offset(hx711_t *hx, int32_t offset) {
    hx->offset = offset;
}

int32_t hx711_get_offset(hx711_t *hx) {
    return hx->offset;
}

void hx711_power_down(hx711_t *hx) {
    gpio_put(hx->gpio_sck, 0);
    gpio_put(hx->gpio_sck, 1);
}

void hx711_power_up(hx711_t *hx) {
    gpio_put(hx->gpio_sck, 0);
}
