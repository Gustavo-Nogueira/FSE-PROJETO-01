/**
 * Communication via UART using the MODBUS-RTU Protocol.
*/
#include <fcntl.h>  //Used for UART
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>  //Used for UART
#include <unistd.h>   //Used for UART

#include "crc16.h"
#include "definitions.h"

int uart_fd;

int init_uart(char *path) {
    uart_fd = -1;

    uart_fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);  // Open in non blocking read/write mode
    if (uart_fd < 0) {
        // printf("Erro - Nao foi possivel iniciar a UART.\n");
        return -1;
    } else {
        // printf("UART inicializada!\n");
    }

    // configure the uart
    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;  // Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    return 0;
}

void close_uart() {
    if (uart_fd > 0) {
        close(uart_fd);
    }
}

int read_uart(void *data, int size) {
    // check for any rx bytes
    if (uart_fd != -1) {
        // printf("Lendo dados da UART ...");
        int rx_bytes = read(uart_fd, data, size);  // Filestream, buffer to store in, number of bytes to read (max)

        if (rx_bytes < 0) {
            data = NULL;
            // printf("UART RX error\n");
        } else if (rx_bytes == 0) {
            data = NULL;
            // printf("Nenhum dado disponivel.\n");
        } else {
            // printf("%d bytes lidos\n", rx_bytes);
        }

        return rx_bytes;
    }
    return -1;
}

int write_uart(void *data, int size) {
    if (uart_fd != -1) {
        // printf("Escrevendo dados na UART ...");
        int tx_bytes = write(uart_fd, data, size);

        if (tx_bytes < 0) {
            // printf("UART TX error\n");
        } else {
            // printf("escrito.\n");
        }

        return tx_bytes;
    }
    return -1;
}

int request_data(int cmd, void *res, int res_size) {
    int tx_size = 0;
    unsigned char tx_buffer[200];
    const int crc_size = sizeof(short);

    tx_buffer[tx_size++] = ESP_ADDR;
    tx_buffer[tx_size++] = REQUEST_CMD_CODE;
    tx_buffer[tx_size++] = cmd;

    memcpy(&tx_buffer[tx_size], &REGISTRATION, sizeof(REGISTRATION));
    tx_size += sizeof(REGISTRATION);

    short crc = calculate_CRC(tx_buffer, tx_size);
    memcpy(&tx_buffer[tx_size], &crc, crc_size);
    tx_size += crc_size;

    int tx_bytes = write_uart((void *)tx_buffer, tx_size);

    if (tx_bytes < 0) {
        // printf("Falha ao enviar solicitacao.\n");
        return -1;
    }

    usleep(UART_DELAY);

    unsigned char rx_buffer[200];
    const int rx_header_size = 3;
    int rx_bytes = read_uart((void *)rx_buffer, rx_header_size + res_size + crc_size);

    if (rx_bytes > 0) {
        // check the crc
        short rx_crc_rec;
        memcpy(&rx_crc_rec, &rx_buffer[rx_header_size + res_size], sizeof(short));
        short rx_crc_pkg = calculate_CRC(rx_buffer, rx_header_size + res_size);

        if (rx_crc_rec == rx_crc_pkg) {
            memcpy(res, &rx_buffer[rx_header_size], res_size);  // retrieve the data
        } else {
            // printf("CRC - Falha ao receber a resposta.\n");
            return -1;
        }
    }

    return rx_bytes;
}

int send_data(int cmd, void *data, int dt_size) {
    int tx_size = 0;
    unsigned char tx_buffer[200];
    const int crc_size = sizeof(short);

    tx_buffer[tx_size++] = ESP_ADDR;
    tx_buffer[tx_size++] = SEND_CMD_CODE;
    tx_buffer[tx_size++] = cmd;

    memcpy(&tx_buffer[tx_size], &REGISTRATION, sizeof(REGISTRATION));
    tx_size += sizeof(REGISTRATION);

    memcpy(&tx_buffer[tx_size], data, dt_size);
    tx_size += dt_size;

    short crc = calculate_CRC(tx_buffer, tx_size);
    memcpy(&tx_buffer[tx_size], &crc, crc_size);
    tx_size += crc_size;

    int tx_bytes = write_uart((void *)tx_buffer, tx_size);

    return tx_bytes;
}

int request_internal_temperature(float *dt) {
    int rx_bytes = request_data(CMD_REQUEST_TI, (void *)dt, sizeof(float));
    return rx_bytes;
}

int request_reference_temperature(float *dt) {
    int rx_bytes = request_data(CMD_REQUEST_TR, (void *)dt, sizeof(float));
    return rx_bytes;
}

int request_control_strategy(int *dt) {
    int rx_bytes = request_data(CMD_REQUEST_SWITCH_STATE, (void *)dt, sizeof(int));
    return rx_bytes;
}

int send_control_signal(int dt) {
    int tx_bytes = send_data(CMD_SEND_CONTROL_SIGNAL, (void *)&dt, sizeof(int));
    return tx_bytes;
}