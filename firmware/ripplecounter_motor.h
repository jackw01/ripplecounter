#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void motor_set_direction(bool direction);
void motor_set_power(uint32_t power);
uint32_t motor_get_max_power();
void ripplecounter_motor_init();
int32_t ripplecounter_get_current_ma();
int32_t ripplecounter_get_position_counts();
void ripplecounter_reset_position();

#ifdef __cplusplus
}
#endif
