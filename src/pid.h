#ifndef PID_H_
#define PID_H_

void pid_set_constants(double Kp_, double Ki_, double Kd_);

void pid_update_reference(double reference_);

double pid_control(double measured_output);

#endif /* PID_H_ */
