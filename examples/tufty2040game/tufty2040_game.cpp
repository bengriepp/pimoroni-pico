#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "tufty2040.hpp"
#include "button.hpp"

using namespace pimoroni;

Tufty2040 tufty;

ST7789 st7789(
  Tufty2040::WIDTH,
  Tufty2040::HEIGHT,
  ROTATE_0,
  ParallelPins{
    Tufty2040::LCD_CS,
    Tufty2040::LCD_DC,
    Tufty2040::LCD_WR,
    Tufty2040::LCD_RD,
    Tufty2040::LCD_D0, 
    Tufty2040::BACKLIGHT
  }
);

PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

Button button_a(Tufty2040::A);
Button button_b(Tufty2040::B);
Button button_c(Tufty2040::C);
Button button_up(Tufty2040::UP);
Button button_down(Tufty2040::DOWN);

// Constants
const uint16_t SCREEN_WIDTH = 240;
const uint16_t SCREEN_HEIGHT = 240;

const uint16_t SHIP_WIDTH = 10;
const uint16_t SHIP_HEIGHT = 16;
const uint16_t SHIP_X = SCREEN_WIDTH / 2 - SHIP_WIDTH / 2;
const uint16_t SHIP_Y = SCREEN_HEIGHT - SHIP_HEIGHT - 10;
const uint16_t SHIP_SPEED = 2;

const uint16_t OBSTACLE_WIDTH = 20;
const uint16_t OBSTACLE_HEIGHT = 20;
const uint16_t OBSTACLE_MIN_SPEED = 1;
const uint16_t OBSTACLE_MAX_SPEED = 5;
const uint16_t OBSTACLE_SPAWN_INTERVAL = 100;
const uint16_t OBSTACLE_SCORE_VALUE = 10;

// Game state variables
bool game_over = false;
uint32_t score = 0;
uint32_t obstacle_spawn_timer = 0;
std::vector<Rectangle> obstacles;
Rectangle ship(SHIP_X, SHIP_Y, SHIP_WIDTH, SHIP_HEIGHT);

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

// Draw the game screen
void draw_game() {
  graphics.set_pen(0, 0, 0);
  graphics.clear();

  // Draw the ship
  graphics.set_pen(255, 255, 255);
  graphics.rectangle(ship);

  // Draw the obstacles
  for (const auto& obstacle : obstacles) {
    graphics.set_pen(255, 0, 0);
    graphics.rectangle(obstacle);
  }

  // Draw the score
  graphics.set_pen(255, 255, 255);
  graphics.text(std::to_string(score), Point(10, 10), 32);

  // Update the screen
  st7789.update(&graphics);
}

// Check for collisions between the ship and obstacles
void check_collisions() {
  for (const auto& obstacle : obstacles) {
    if (ship.intersects(obstacle)) {
      game_over = true;
      return;
    }
  }
}

int main() {
  // Initialize the display
  st7789.set_backlight(255);

  // Initialize the random number generator
  std::srand(time());

  // Set up the ship and obstacles
  obstacles.reserve(50);

  // Main game loop
  while (true) {
    // Spawn new obstacles
    if (!game_over) {
      obstacle_spawn_timer += 1;
      if (obstacle_spawn_timer >= OBSTACLE_SPAWN_INTERVAL) {
        uint16_t speed = OBSTACLE_MIN_SPEED + std::rand() % (OBSTACLE_MAX_SPEED - OBSTACLE_MIN_SPEED + 1);
        uint16_t x = std::rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
        Rectangle obstacle(x, 0, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, speed);
        obstacles.push_back(obstacle);
        obstacle_spawn_timer = 0;
      }
    }

    // Move the obstacles
    for (auto& obstacle : obstacles) {
      obstacle.move(0, obstacle.speed);
      if (obstacle.y + OBSTACLE_HEIGHT >= SCREEN_HEIGHT) {
        obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), obstacle), obstacles.end());
        score += OBSTACLE_SCORE_VALUE;
      }
    }

    // Move the ship
    if (!game_over) {
      if (button_left.is_pressed()) {
        ship.move(-SHIP_SPEED, 0);
      } else if (button_right.is_pressed()) {
        ship.move(SHIP_SPEED, 0);
      }
      check_collisions();
    }

    // Draw the game
    draw_game();

    // Update the LED
    if (!game_over) {
      tufty.led(score % 256);
    } else {
      tufty.led(0);
    }

    // Check for game over
    if (game_over) {
      graphics.set_pen(255, 0, 0);
      graphics.text("GAME OVER", Point(50, 100), 64);
      st7789.update(&graphics);
      break;
    }
  }

  return 0;
}