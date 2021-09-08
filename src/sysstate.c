#include "sysstate.h"

#include <stdlib.h>
#include <string.h>

SYSTEM_STATE _system_state;

void init_sys_state(SYSTEM_STATE *st) {
    memcpy(&_system_state, st, sizeof(SYSTEM_STATE));
}

// Getters
SYSTEM_STATE *get_sys_state() {
    SYSTEM_STATE *st = malloc(sizeof(SYSTEM_STATE));
    memcpy(st, &_system_state, sizeof(SYSTEM_STATE));
    return st;
}

int get_is_on() {
    return _system_state.is_on;
}

double get_kp() {
    return _system_state.kp;
}

double get_ki() {
    return _system_state.ki;
}

double get_kd() {
    return _system_state.kd;
}

double get_hysteresis() {
    return _system_state.hysteresis;
}

double get_ti() {
    return _system_state.ti;
}

double get_te() {
    return _system_state.te;
}

double get_tr() {
    return _system_state.tr;
}

int get_control_strategy() {
    return _system_state.control_strategy;
}

int get_tr_acquisition_mode() {
    return _system_state.tr_acquisition_mode;
}

int get_cs_acquisition_mode() {
    return _system_state.cs_acquisition_mode;
}

double get_fan_duty_cyle() {
    return _system_state.fan_duty_cyle;
}

double get_resistor_duty_cyle() {
    return _system_state.resistor_duty_cyle;
}

// Setters
void set_is_on(int value) {
    _system_state.is_on = value;
}

void set_kp(double value) {
    _system_state.kp = value;
}

void set_ki(double value) {
    _system_state.ki = value;
}

void set_kd(double value) {
    _system_state.kd = value;
}

void set_hysteresis(double value) {
    _system_state.hysteresis = value;
}

void set_ti(double value) {
    _system_state.ti = value;
}

void set_te(double value) {
    _system_state.te = value;
}

void set_tr(double value) {
    _system_state.tr = value;
}

void set_control_strategy(int value) {
    _system_state.control_strategy = value;
}

void set_tr_acquisition_mode(int value) {
    _system_state.tr_acquisition_mode = value;
}

void set_cs_acquisition_mode(int value) {
    _system_state.cs_acquisition_mode = value;
}

void set_fan_duty_cyle(double value) {
    _system_state.fan_duty_cyle = value;
}

void set_resistor_duty_cyle(double value) {
    _system_state.resistor_duty_cyle = value;
}
