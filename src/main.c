#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme280.h"
#include "definitions.h"
#include "gpio.h"
#include "handlers.h"
#include "lcd.h"
#include "report.h"
#include "screen.h"
#include "sysstate.h"
#include "uart.h"

SYSTEM_STATE initial_sys_state = {
    .is_on = 0,
    .kp = DEFAULT_KP,
    .ki = DEFAULT_KI,
    .kd = DEFAULT_KD,
    .hysteresis = MIN_HYSTERESIS,
    .ti = 0.0,
    .te = 0.0,
    .tr = 0.0,
    .control_strategy = ON_OFF_CODE,
    .tr_acquisition_mode = TR_TERMINAL,
    .cs_acquisition_mode = CONTROL_STRATEGY_TERMINAL,
    .fan_duty_cyle = 0.0,
    .resistor_duty_cyle = 0.0,
};

int main(int argc, const char *argv[]) {
    signal(SIGINT, handle_sigint);

    init_pwm_fan();
    init_pwm_resistor();
    init_uart(UART_PATH);
    init_lcd(LCD_ADDR);
    bme280Init(BME280_PORT, BME280_ADDR);
    init_report_file();
    init_screen();
    init_sys_state(&initial_sys_state);

    pthread_t uart_tid;
    pthread_create(&uart_tid, NULL, &handle_uart_reading, NULL);

    pthread_t bme_tid;
    pthread_create(&bme_tid, NULL, &handle_te_reading, NULL);

    pthread_t lcd_tid;
    pthread_create(&lcd_tid, NULL, &handle_lcd_update, NULL);

    pthread_t report_tid;
    pthread_create(&report_tid, NULL, &handle_report_update, NULL);

    pthread_t menu_wd_tid;
    pthread_create(&menu_wd_tid, NULL, &handle_menu_window, NULL);

    pthread_t state_wd_tid;
    pthread_create(&state_wd_tid, NULL, &handle_sys_state_window, NULL);

    pthread_t control_tid;
    pthread_create(&control_tid, NULL, &handle_control, NULL);

    pause();

    return 0;
}
