#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "definitions.h"
#include "helpers.h"
#include "sysstate.h"

FILE *report_file_fp;

int init_report_file() {
    report_file_fp = fopen(FILE_REPORT_PATH, "w");

    if (report_file_fp == NULL) return -1;

    fprintf(report_file_fp, "data_hora,ti,te,tr,duty_cycle_resistor,duty_cycle_ventoinha\n");

    return 0;
}

void close_report_file() {
    fclose(report_file_fp);
}

int update_report_file() {
    char datatime[25];
    SYSTEM_STATE *sys_st = get_sys_state();

    get_datatime(datatime);

    return fprintf(report_file_fp, "%s,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf\n",
                   datatime, sys_st->ti, sys_st->te, sys_st->tr, sys_st->resistor_duty_cyle, sys_st->fan_duty_cyle);

    free(sys_st);
}
