double onoff_hysteresis, onoff_reference, onoff_signal = 100.0;

void onoff_set_hysteresis(double h) {
    onoff_hysteresis = h;
}

void onoff_update_reference(double r) {
    onoff_reference = r;
}

double onoff_control(double measured_output) {
    double upper_limit = onoff_reference + onoff_hysteresis / 2.0;
    double lower_limit = onoff_reference - onoff_hysteresis / 2.0;

    if (lower_limit > measured_output) {
        onoff_signal = 100.0;
    } else if (upper_limit < measured_output) {
        onoff_signal = -100.0;
    }

    return onoff_signal;
}