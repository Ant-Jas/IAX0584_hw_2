#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <stdio.h>
#include <time.h>
#include <string.h>

#define WARNING_BAR_STR "=========================================================="

#define MAX_TIME_STR_LEN 64
#define MAX_LOG_MSG_STR_LEN 256
#define MAX_LOG_FILE_NAME_LEN 64

enum log_levels {OFF, ERROR, WARNING, INFO};

void get_log_time(char *str);
void write_log(enum log_levels msg_lvl, char *msg);
void set_logging_level(enum log_levels lvl);
void set_logging_file_name(char *f_name);
char *print_log_level(enum log_levels lvl);

#endif
