#ifndef SYSSTATE_H_
#define SYSSTATE_H_

// SYSTEM STATE
typedef struct {
    int is_on;
    double kp, ki, kd;
    double hysteresis;
    double ti, te, tr;
    int control_strategy;
    int tr_acquisition_mode;
    int cs_acquisition_mode;
    double fan_duty_cyle;
    double resistor_duty_cyle;
} SYSTEM_STATE;

void init_sys_state(SYSTEM_STATE *st);

// Getters
SYSTEM_STATE *get_sys_state();

int get_is_on();

double get_kp();

double get_ki();

double get_kd();

double get_hysteresis();

double get_ti();

double get_te();

double get_tr();

int get_control_strategy();

int get_tr_acquisition_mode();

int get_cs_acquisition_mode();

double get_fan_duty_cyle();

double get_resistor_duty_cyle();

// Setters
void set_is_on(int value);

void set_kp(double value);

void set_ki(double value);

void set_kd(double value);

void set_hysteresis(double value);

void set_ti(double value);

void set_te(double value);

void set_tr(double value);

void set_control_strategy(int value);

void set_tr_acquisition_mode(int value);

void set_cs_acquisition_mode(int value);

void set_fan_duty_cyle(double value);

void set_resistor_duty_cyle(double value);

#endif /* SYSSTATE_H_ */