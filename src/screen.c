#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "definitions.h"
#include "sysstate.h"

#define KEY_SELECT_OPT '\n'

#define MIN_COLS 110
#define MIN_LINES 40

#define STATE_WD_WIDTH 1
#define STATE_WD_HEIGHT 0.25
#define MENU_WD_WIDTH 1  // 0.5
#define MENU_WD_HEIGHT 0.75
#define LOGS_WD_WIDTH 0.5
#define LOGS_WD_HEIGHT 0.75

typedef struct {
    int key;
    char *label;
} MENU_OPTION;

#define SIZEOF_OPTION_LIST(L) (sizeof(L) / sizeof(MENU_OPTION))

sem_t screen_mutex;
int app_lines, app_cols;
WINDOW *win_state, *win_menu, *win_log;

MENU_OPTION main_menu_opts[] = {
    {.key = 1, .label = "Ligar/Desligar Controlador"},
    {.key = 2, .label = "Redefinir KP(PID)"},
    {.key = 3, .label = "Redefinir KI(PID)"},
    {.key = 4, .label = "Redefinir KD(PID)"},
    {.key = 5, .label = "Redefinir Histerese(On-Off)"},
    {.key = 6, .label = "Redefinir Estrategia de Controle(EC)"},
    {.key = 7, .label = "Redefinir Temperatura de Referencia(TR)"},
    {.key = 8, .label = "Redefinir Modo de Aquisicao da EC"},
    {.key = 9, .label = "Redefinir Modo de Aquisicao da TR"},
    {.key = 10, .label = "Encerrar Programa (CTRL+C)"},
};

MENU_OPTION control_strategy_opts[] = {
    {.key = PID_CODE, .label = "PID"},
    {.key = ON_OFF_CODE, .label = "On-Off"},
};

MENU_OPTION tr_acquisition_mode_opts[] = {
    {.key = TR_TERMINAL, .label = "Terminal"},
    {.key = TR_POTENTIOMETER, .label = "Potenciometro"},
};

MENU_OPTION cs_acquisition_mode_opts[] = {
    {.key = CONTROL_STRATEGY_TERMINAL, .label = "Terminal"},
    {.key = CONTROL_STRATEGY_SWITCH, .label = "Switch"},
};

void refresh_window(WINDOW *_wdw) {
    sem_wait(&screen_mutex);
    wrefresh(_wdw);
    sem_post(&screen_mutex);
}

void delete_window(WINDOW *_wdw) {
    sem_wait(&screen_mutex);
    werase(_wdw);
    wrefresh(_wdw);
    delwin(_wdw);
    sem_post(&screen_mutex);
}

void update_dimensions_data() {
    app_lines = LINES > MIN_LINES ? LINES : MIN_LINES;
    app_cols = COLS > MIN_COLS ? COLS : MIN_COLS;
}

void draw_input_box(int _lns, int _cls, int _by, int _bx, char *title, char *label, char *value) {
    const int margin_horz = 2;
    const int margin_vert = 3;

    WINDOW *subw = newwin(_lns, _cls, _by, _bx);
    // box(subw, 0, 0);

    echo();
    curs_set(1);

    wattron(subw, A_BOLD);
    mvwprintw(subw, 1, margin_horz, "%s", title);
    wattroff(subw, A_BOLD);

    mvwprintw(subw, margin_vert, margin_horz, "%s", label);

    refresh_window(subw);

    wscanw(subw, "%s", value);
    // mvwscanw(subw, margin_vert, strlen(label) + 2, "%s", value);

    delete_window(subw);
}

void draw_selection_box(int _lns, int _cls, int _by, int _bx, char *title, MENU_OPTION lst[], size_t sz_lst, int init_k, int *opt) {
    int pos, ch;
    char item[1000];
    char format[10];
    const int margin_horz = 2;
    const int margin_vert = 3;
    sprintf(format, "%%-%ds", _cls - 2 * margin_horz);

    WINDOW *subw = newwin(_lns, _cls, _by, _bx);
    // box(subw, 0, 0);

    wattron(subw, A_BOLD);
    mvwprintw(subw, 1, margin_horz, "%s", title);
    wattroff(subw, A_BOLD);

    for (int i = 0; i < sz_lst; i++) {
        sprintf(item, format, lst[i].label);
        if (lst[i].key == init_k) {
            pos = i;
            wattron(subw, A_STANDOUT);
            mvwprintw(subw, i + margin_vert, margin_horz, "%s", item);
            wattroff(subw, A_STANDOUT);
        } else {
            mvwprintw(subw, i + margin_vert, margin_horz, "%s", item);
        }
    }

    refresh_window(subw);

    noecho();
    keypad(subw, TRUE);
    curs_set(0);

    while ((ch = wgetch(subw)) != KEY_SELECT_OPT) {
        // right pad with spaces to make the items appear with even width.
        sprintf(item, format, lst[pos].label);
        mvwprintw(subw, pos + margin_vert, margin_horz, "%s", item);

        switch (ch) {
            case KEY_UP:
                pos--;
                pos = (pos < 0) ? sz_lst - 1 : pos;
                break;
            case KEY_DOWN:
                pos++;
                pos = (pos >= sz_lst) ? 0 : pos;
                break;
            default:
                break;
        }

        // now highlight the next item in the list.
        wattron(subw, A_STANDOUT);
        sprintf(item, format, lst[pos].label);
        mvwprintw(subw, pos + margin_vert, margin_horz, "%s", item);
        wattroff(subw, A_STANDOUT);
    }

    *opt = lst[pos].key;

    delete_window(subw);
}

void draw_system_state(WINDOW *_wdw) {
    char tmp[500];
    int px, py;
    int box_cols = app_cols * STATE_WD_WIDTH;
    int box_lines = app_lines * STATE_WD_HEIGHT;
    int start_y = (box_lines - 7) / 2;
    SYSTEM_STATE *sys_st = get_sys_state();

    delete_window(_wdw);

    curs_set(0);

    _wdw = newwin(box_lines, box_cols, 0, 0);
    box(_wdw, 0, 0);

    wattron(_wdw, A_BOLD);
    mvwprintw(_wdw, 0, 1, "Estado-do-Sistema");
    wattroff(_wdw, A_BOLD);

    // LINE 01
    sprintf(tmp, "Estrategia de Controle Atual: %s", sys_st->control_strategy == PID_CODE ? "PID" : "On-Off");
    px = start_y;
    py = ((box_cols / 2) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "Situacao do Controlador: %s", sys_st->is_on ? "Ligado" : "Desligado");
    px = start_y;
    py = (box_cols / 2) + ((box_cols / 2) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    // LINE 02
    sprintf(tmp, "Aquisicao da Estrategia de Controle: %s",
            sys_st->cs_acquisition_mode == CONTROL_STRATEGY_TERMINAL ? "Terminal" : "Switch");
    px = start_y + 2;
    py = ((box_cols / 2) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "Aquisicao da Temperatura de Referencia: %s",
            sys_st->tr_acquisition_mode == TR_TERMINAL ? "Terminal" : "Potenciometro");
    px = start_y + 2;
    py = (box_cols / 2) + ((box_cols / 2) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    // LINE 03
    sprintf(tmp, "TI: %.2lf", sys_st->ti);
    px = start_y + 4;
    py = ((box_cols / 3) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "TE: %.2lf", sys_st->te);
    px = start_y + 4;
    py = (box_cols / 3) * 1 + ((box_cols / 3) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "TR: %.2lf", sys_st->tr);
    px = start_y + 4;
    py = (box_cols / 3) * 2 + ((box_cols / 3) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    // LINE 04
    sprintf(tmp, "Histerese: %.2lf", sys_st->hysteresis);
    px = start_y + 6;
    py = ((box_cols / 4) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "KP: %.2lf", sys_st->kp);
    px = start_y + 6;
    py = (box_cols / 4) * 1 + ((box_cols / 4) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "KI: %.2lf", sys_st->ki);
    px = start_y + 6;
    py = (box_cols / 4) * 2 + ((box_cols / 4) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    sprintf(tmp, "KD: %.2lf", sys_st->kd);
    px = start_y + 6;
    py = (box_cols / 4) * 3 + ((box_cols / 4) / 2) - (strlen(tmp) / 2);
    mvwprintw(_wdw, px, py, tmp);

    refresh_window(_wdw);
    free(sys_st);
}

void draw_menu(WINDOW *_wdw) {
    int x_offset = 0;
    int y_offset = app_lines * STATE_WD_HEIGHT;
    int box_cols = app_cols * MENU_WD_WIDTH;
    int box_lines = app_lines * MENU_WD_HEIGHT;

    delete_window(_wdw);

    _wdw = newwin(box_lines, box_cols, y_offset, x_offset);
    box(_wdw, 0, 0);

    wattron(_wdw, A_BOLD);
    mvwprintw(_wdw, 0, 1, "Menu");
    wattroff(_wdw, A_BOLD);
    refresh_window(_wdw);

    int opt, res;
    char buff[1000];
    while (1) {
        draw_selection_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                           "Menu Principal", main_menu_opts, SIZEOF_OPTION_LIST(main_menu_opts), 1, &opt);

        switch (opt) {
            case 1:
                set_is_on(get_is_on() ? 0 : 1);
                break;
            case 2:
                draw_input_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                               "Redefinir KP", "Insira um novo valor:", buff);
                set_kp(atof(buff));
                break;
            case 3:
                draw_input_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                               "Redefinir KI", "Insira um novo valor:", buff);
                set_ki(atof(buff));
                break;
            case 4:
                draw_input_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                               "Redefinir KD", "Insira um novo valor:", buff);
                set_kd(atof(buff));
                break;
            case 5:
                draw_input_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                               "Redefinir Histerese", "Insira um novo valor:", buff);
                set_hysteresis(atof(buff));
                break;
            case 6:
                draw_selection_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                                   "Redefinir Estrategia de Controle", control_strategy_opts,
                                   SIZEOF_OPTION_LIST(control_strategy_opts), get_control_strategy(), &res);

                set_control_strategy(res);
                break;
            case 7:
                draw_input_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                               "Redefinir Temperatura de Referencia", "Insira um novo valor:", buff);
                set_tr(atof(buff));
                break;
            case 8:
                draw_selection_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                                   "Redefinir Modo de Aquisicao da EC", cs_acquisition_mode_opts,
                                   SIZEOF_OPTION_LIST(cs_acquisition_mode_opts), get_cs_acquisition_mode(), &res);
                set_cs_acquisition_mode(res);
                break;
            case 9:
                draw_selection_box(box_lines - 2, box_cols - 2, y_offset + 1, x_offset + 1,
                                   "Redefinir Modo de Aquisicao da TR", tr_acquisition_mode_opts,
                                   SIZEOF_OPTION_LIST(tr_acquisition_mode_opts), get_tr_acquisition_mode(), &res);
                set_tr_acquisition_mode(res);
                break;
            case 10:
                kill(getpid(), SIGINT);
                break;
        }
    }
}

void draw_system_logs(WINDOW *_wdw) {
    werase(_wdw);
    wrefresh(_wdw);
    delwin(_wdw);

    int x_offset = app_cols * MENU_WD_WIDTH + (app_cols % 2 != 0 ? 1 : 0);
    int y_offset = app_lines * STATE_WD_HEIGHT;
    int box_cols = app_cols * LOGS_WD_WIDTH;
    int box_lines = app_lines * LOGS_WD_HEIGHT;

    _wdw = newwin(box_lines, box_cols, y_offset, x_offset);
    box(_wdw, 0, 0);

    mvwprintw(_wdw, 0, 1, "Logs");

    wrefresh(_wdw);
}

void handle_resize(int sig) {
    signal(sig, handle_resize);

    update_dimensions_data();
}

void call_draw_menu() {
    draw_menu(win_menu);
}

void call_draw_system_state() {
    draw_system_state(win_state);
}

void init_screen() {
    initscr();
    refresh();

    sem_init(&screen_mutex, 0, 1);

    update_dimensions_data();
}

void close_screen() {
    if (win_state != NULL) delwin(win_state);
    if (win_menu != NULL) delwin(win_menu);
    if (win_log != NULL) delwin(win_log);

    endwin();
}