#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme280.h"
#include "definitions.h"
#include "gpio.h"
#include "lcd.h"
#include "onoff.h"
#include "pid.h"
#include "report.h"
#include "screen.h"
#include "sysstate.h"
#include "uart.h"

void* handle_uart_reading(void* args) {
    float curr_ti, curr_tr, last_ti;
    int bytes, curr_cs, first_ti = 1;

    while (1) {
        // TI
        bytes = request_internal_temperature(&curr_ti);
        if (first_ti) {
            last_ti = curr_ti;
            first_ti = 0;
        }
        if (abs(curr_ti - last_ti) < MAX_TEMP_VARIATION) {
            set_ti((double)curr_ti);
            last_ti = curr_ti;
        }
        usleep(UART_DELAY);
        // TR
        if (get_tr_acquisition_mode() == TR_POTENTIOMETER) {
            bytes = request_reference_temperature(&curr_tr);
            set_tr((double)curr_tr);
        }
        usleep(UART_DELAY);
        // CS
        if (get_cs_acquisition_mode() == CONTROL_STRATEGY_SWITCH) {
            bytes = request_control_strategy(&curr_cs);
            set_control_strategy(curr_cs);
        }
        usleep(UART_DELAY);
    }
}

void* handle_ti_reading(void* args) {
    float curr, last;
    int bytes, first = 1;

    while (1) {
        bytes = request_internal_temperature(&curr);
        if (bytes == sizeof(curr)) {
            if (first) {
                last = curr;
                first = 0;
            }
            if (abs(curr - last) < MAX_TEMP_VARIATION) {
                set_ti((double)curr);
                last = curr;
            }
        }
        usleep(UART_DELAY);
    }
}

void* handle_te_reading(void* args) {
    int T, P, H;
    double curr, last;
    int checker, first = 1;

    while (1) {
        checker = bme280ReadValues(&T, &P, &H);
        if (checker == 0) {
            curr = (double)(T / 100.0);
            if (first) {
                last = curr;
                first = 0;
            }
            if (abs(curr - last) < MAX_TEMP_VARIATION) {
                set_te(curr);
                last = curr;
            }
        }
        usleep(BME280_DELAY);
    }
}

void* handle_tr_reading(void* args) {
    float curr;
    int bytes;

    while (1) {
        if (get_tr_acquisition_mode() == TR_POTENTIOMETER) {
            bytes = request_reference_temperature(&curr);
            if (bytes == sizeof(curr)) {
                set_tr((double)curr);
            }
        }
        usleep(UART_DELAY);
    }
}

void* handle_cs_reading(void* args) {
    int curr;
    int bytes;

    while (1) {
        if (get_cs_acquisition_mode() == CONTROL_STRATEGY_SWITCH) {
            bytes = request_control_strategy(&curr);
            if (bytes == sizeof(curr)) {
                set_control_strategy(curr);
            }
        }
        usleep(UART_DELAY);
    }
}

void* handle_lcd_update(void* args) {
    SYSTEM_STATE* st;
    while (1) {
        st = get_sys_state();
        update_lcd((float)st->ti, (float)st->te, (float)st->tr);
        usleep(LCD_DELAY);
        free(st);
    }
}

void* handle_report_update(void* args) {
    while (1) {
        if (get_is_on()) {
            update_report_file();
        }
        usleep(REPORT_DELAY);
    }
}

void* handle_menu_window(void* args) {
    call_draw_menu();
    return NULL;
}

void* handle_sys_state_window(void* args) {
    while (1) {
        call_draw_system_state();
        usleep(STATE_WINDOW_DELAY);
    }
}

void* handle_control(void* args) {
    int duty_cycle = 0;

    while (1) {
        if (get_is_on()) {
            if (get_control_strategy() == ON_OFF_CODE) {
                onoff_update_reference(get_tr());
                duty_cycle = (int)onoff_control(get_ti());
            } else if (get_control_strategy() == PID_CODE) {
                pid_update_reference(get_tr());
                pid_set_constants(get_kp(), get_ki(), get_kd());
                duty_cycle = (int)pid_control(get_ti());
            }
            if (duty_cycle > 0) {
                write_pwm_fan(0);
                write_pwm_resistor(duty_cycle);
                set_fan_duty_cyle(0);
                set_resistor_duty_cyle(duty_cycle);
            } else if (duty_cycle < 0 && abs(duty_cycle) >= MIN_FAN_POWER) {
                write_pwm_resistor(0);
                write_pwm_fan(abs(duty_cycle));
                set_resistor_duty_cyle(0);
                set_fan_duty_cyle(abs(duty_cycle));
            } else {
                write_pwm_fan(0);
                write_pwm_resistor(0);
                set_fan_duty_cyle(0);
                set_resistor_duty_cyle(0);
            }
            send_control_signal(duty_cycle);
        } else {
            write_pwm_resistor(0);
            write_pwm_fan(0);
        }
        usleep(CONTROL_DELAY);
    }
}

void handle_sigint(int sig) {
    signal(sig, handle_sigint);

    write_pwm_resistor(0);
    write_pwm_fan(0);

    close_screen();
    close_report_file();
    close_uart();
    close_lcd();
    bme280Close();

    exit(0);
}