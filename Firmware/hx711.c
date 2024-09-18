#include "hx711.h"
#include "pico/stdlib.h"

void hx711_init(hx711_t *hx, uint gpio_dout, uint gpio_sck, char *name) {
    hx->gpio_dout = gpio_dout;
    hx->gpio_sck = gpio_sck;
    hx->name = name;
    gpio_init(gpio_dout);
    gpio_set_dir(gpio_dout, GPIO_IN);
    gpio_init(gpio_sck);
    gpio_set_dir(gpio_sck, GPIO_OUT);
}

void hx711_tare(hx711_t *hx) {
    int64_t sum = 0; // Use int64_t to avoid overflow
    printf("Taring %s...\n", hx->name);
    for (int i = 0; i < 10; i++) {
        int32_t reading = hx711_read_raw(hx); // Use a raw read function
        printf("Raw Reading %d: %d\n", i, reading);
        sum += reading;
    }
    hx->offset = sum / 10;
    printf("Calculated Offset: %d\n", hx->offset);
}

int32_t hx711_read_raw(hx711_t *hx) {
    int32_t count = 0;
    while (gpio_get(hx->gpio_dout));
    for (int i = 0; i < 24; i++) {
        gpio_put(hx->gpio_sck, 1);
        count = count << 1;
        gpio_put(hx->gpio_sck, 0);
        if (gpio_get(hx->gpio_dout)) {
            count++;
        }
    }
    gpio_put(hx->gpio_sck, 1);
    count = count ^ 0x800000;
    gpio_put(hx->gpio_sck, 0);
    return count;
}

int32_t hx711_read(hx711_t *hx) {
    int32_t raw_value = hx711_read_raw(hx);
    int32_t adjusted_value = raw_value - hx->offset;
    if(adjusted_value < 0 || adjusted_value > 1000000){
        adjusted_value = 0;
    }
    return adjusted_value;
}