#ifndef GPIO_H_
#define GPIO_H_

void init_pwm_fan();

void init_pwm_resistor();

void write_pwm_fan(int duty_cycle);

void write_pwm_resistor(int duty_cycle);

#endif /* GPIO_H_ */
