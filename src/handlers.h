#ifndef HANDLERS_H_
#define HANDLERS_H_

void* handle_uart_reading(void* args);

void* handle_ti_reading(void* args);

void* handle_te_reading(void* args);

void* handle_tr_reading(void* args);

void* handle_cs_reading(void* args);

void* handle_lcd_update(void* args);

void* handle_report_update(void* args);

void* handle_menu_window(void* args);

void* handle_sys_state_window(void* args);

void* handle_control(void* args);

void handle_sigint(int sig);

#endif /* HANDLERS_H_ */
