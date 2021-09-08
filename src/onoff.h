#ifndef ONOFF_H_
#define ONOFF_H_

void onoff_set_hysteresis(double h);

void onoff_update_reference(double r);

double onoff_control(double measured_output);

#endif /* ONOFF_H_ */