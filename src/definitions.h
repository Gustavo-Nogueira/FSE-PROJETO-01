#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// Matrícula
static const unsigned char REGISTRATION[] = {4, 2, 5, 9};

// TEMPERATURE VARIATION
#define MAX_TEMP_VARIATION 5.0  // maximum variation in one second

// BME280
#define BME280_ADDR 0x76
#define BME280_PORT 0x01
#define BME280_DELAY 1000000  // 1 seg

// LCD
#define LCD_ADDR 0x27
#define LCD_PORT 0x01
#define LCD_DELAY 1000000  // 1 seg

// ESP
#define ESP_ADDR 0x01

// ACTUATORS
#define FAN_WIRINGPI 5       // GPIO 24
#define RESISTOR_WIRINGPI 4  // GPIO 23
#define MIN_FAN_POWER 41     // percentage

// UART
#define UART_PATH "/dev/serial0"
#define UART_DELAY 1000000  // 1 seg
#define SEND_CMD_CODE 0x16
#define REQUEST_CMD_CODE 0x23
#define CMD_SEND_CONTROL_SIGNAL 0xD1
#define CMD_REQUEST_TI 0xC1            // internal temperature
#define CMD_REQUEST_TR 0xC2            // outside temperature
#define CMD_REQUEST_SWITCH_STATE 0xC3  // control strategy

// REPORT
#define FILE_REPORT_PATH "report.csv"  // CSV FILE
#define REPORT_DELAY 2000000           // 2 seg

// SELECTIONS
#define PID_CODE 1
#define ON_OFF_CODE 0
#define TR_TERMINAL 0
#define TR_POTENTIOMETER 1
#define CONTROL_STRATEGY_TERMINAL 0
#define CONTROL_STRATEGY_SWITCH 1

// CONTROL
#define MIN_HYSTERESIS 4.0     // ON-OFF
#define DEFAULT_KP 5.0         // PID
#define DEFAULT_KI 1.0         // PID
#define DEFAULT_KD 5.0         // PID
#define CONTROL_DELAY 1000000  // 1 seg

// SCREEN
#define STATE_WINDOW_DELAY 1000000  // 1 seg

#endif /* DEFINITIONS_H_ */