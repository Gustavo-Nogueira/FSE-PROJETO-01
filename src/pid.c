#include "pid.h"

#include <stdio.h>

double sinal_de_controle;
double referencia = 0.0;
double Kp = 0.0;  // Ganho Proporcional
double Ki = 0.0;  // Ganho Integral
double Kd = 0.0;  // Ganho Derivativo
int T = 1.0;      // Período de Amostragem (ms)
unsigned long last_time;
double erro_total, erro_anterior = 0.0;
int sinal_de_controle_MAX = 100.0;
int sinal_de_controle_MIN = -100.0;

void pid_set_constants(double Kp_, double Ki_, double Kd_) {
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
}

void pid_update_reference(double reference_) {
    referencia = reference_;
}

double pid_control(double measured_output) {
    double erro = referencia - measured_output;

    erro_total += erro;  // Acumula o erro (Termo Integral)

    if (erro_total >= sinal_de_controle_MAX) {
        erro_total = sinal_de_controle_MAX;
    } else if (erro_total <= sinal_de_controle_MIN) {
        erro_total = sinal_de_controle_MIN;
    }

    double delta_error = erro - erro_anterior;  // Diferença entre os erros (Termo Derivativo)

    sinal_de_controle = Kp * erro + (Ki * T) * erro_total + (Kd / T) * delta_error;  // PID calcula sinal de controle

    if (sinal_de_controle >= sinal_de_controle_MAX) {
        sinal_de_controle = sinal_de_controle_MAX;
    } else if (sinal_de_controle <= sinal_de_controle_MIN) {
        sinal_de_controle = sinal_de_controle_MIN;
    }

    erro_anterior = erro;

    return sinal_de_controle;
}
