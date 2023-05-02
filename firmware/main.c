#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "ripplecounter_motor.h"


static const uint32_t TickIntervalUs = 1000;

static const uint32_t MotorMeasurementIntervalMs = 100;
static const uint32_t StateChangeIntervalMs = 2000;

enum SystemState {
    SystemStateInit = 0,
    SystemStateIdle,
    SystemStateForward,
    SystemStateReverse
};

uint32_t system_state = SystemStateInit;

uint32_t motor_power = 0;
uint32_t motor_measurement_timer = 1;
uint32_t state_timer = 1;
int32_t last_position = 0;


// Main loop

void tick(uint32_t millis, uint32_t dt_micros) {
  if (millis >= motor_measurement_timer) {
    int32_t position = ripplecounter_get_position_counts();
    int32_t delta_position = position - last_position;

    printf("motor current (mA): %d\n", ripplecounter_get_current_ma());
    printf("position (counts): %d\n", position);
    printf("speed (counts/s): %d\n\n", (delta_position > 0 ? delta_position : -delta_position) * 1000 / MotorMeasurementIntervalMs);

    motor_measurement_timer = millis + MotorMeasurementIntervalMs;
    last_position = position;
  }

  if (system_state == SystemStateInit) {
    motor_set_direction(true);
    motor_set_power(0);
    system_state = SystemStateForward;
  } else if (system_state == SystemStateForward) {
    if (millis >= state_timer) {
      system_state = SystemStateReverse;
      state_timer = millis + StateChangeIntervalMs;
      motor_set_direction(true);
      motor_set_power(motor_power);
    }
  } else if (system_state == SystemStateReverse) {
    if (millis >= state_timer) {
      system_state = SystemStateForward;
      state_timer = millis + StateChangeIntervalMs;
      motor_set_direction(false);
      motor_set_power(motor_power);
    }
  }
}

int main() {
  stdio_init_all();

  ripplecounter_motor_init();
  motor_power = motor_get_max_power();

  uint64_t micros;
  uint32_t dt;

  while (true) {
    micros = time_us_64();
    tick(us_to_ms(micros), dt);
    dt = time_us_64() - micros;
    if (dt < TickIntervalUs) {
      absolute_time_t target;
      update_us_since_boot(&target, micros + TickIntervalUs);
      busy_wait_until(target);
    }
  }
}
