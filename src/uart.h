#ifndef UART_H_
#define UART_H_

int init_uart(char *path);

void close_uart();

int request_internal_temperature(float *dt);

int request_reference_temperature(float *dt);

int request_control_strategy(int *dt);

int send_control_signal(int dt);

#endif /* UART_H_ */