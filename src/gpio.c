
#include <softPwm.h>
#include <wiringPi.h>

#include "definitions.h"

void init_pwm_fan() {
    wiringPiSetup();
    pinMode(FAN_WIRINGPI, OUTPUT);
    softPwmCreate(FAN_WIRINGPI, 1, 100);
}

void init_pwm_resistor() {
    wiringPiSetup();
    pinMode(RESISTOR_WIRINGPI, OUTPUT);
    softPwmCreate(RESISTOR_WIRINGPI, 1, 100);
}

void write_pwm_fan(int duty_cycle) {
    pinMode(FAN_WIRINGPI, OUTPUT);
    softPwmCreate(FAN_WIRINGPI, 0, 100);
    softPwmWrite(FAN_WIRINGPI, duty_cycle);
}

void write_pwm_resistor(int duty_cycle) {
    pinMode(RESISTOR_WIRINGPI, OUTPUT);
    softPwmCreate(RESISTOR_WIRINGPI, 0, 100);
    softPwmWrite(RESISTOR_WIRINGPI, duty_cycle);
}
