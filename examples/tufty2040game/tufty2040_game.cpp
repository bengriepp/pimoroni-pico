#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;

int main() {
  // Initialize the LED pin
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // Main program loop
  while (true) {
    // Turn the LED on
    gpio_put(LED_PIN, 1);

    // Wait for 0.5 seconds
    sleep_ms(500);

    // Turn the LED off
    gpio_put(LED_PIN, 0);

    // Wait for another 0.5 seconds
    sleep_ms(500);
  }

  return 0;
}