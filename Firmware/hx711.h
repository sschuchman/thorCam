#ifndef HX711_H
#define HX711_H

#include "pico/stdlib.h"

typedef struct {
    char *name;
    uint gpio_dout;
    uint gpio_sck;
    int32_t offset;
} hx711_t;

void hx711_init(hx711_t *hx, uint gpio_dout, uint gpio_sck, char *name);
void hx711_tare(hx711_t *hx);
int32_t hx711_read_raw(hx711_t *hx);
int32_t hx711_read(hx711_t *hx);

#endif // HX711_H