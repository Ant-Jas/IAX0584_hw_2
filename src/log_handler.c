/*
File:         log_handler.c
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Functions to help with logging.
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <log_handler.h>

static enum log_levels global_log_level = INFO;
static char global_log_file_name[MAX_LOG_FILE_NAME_LEN] = "log.txt";

void get_log_time(char *str)
{
    time_t current_time;
    struct tm *time_struct;
    
    time(&current_time);
    time_struct = localtime(&current_time);
    strftime(str, MAX_TIME_STR_LEN, "%Y.%m.%d %T", time_struct);
}


void write_log(enum log_levels msg_lvl, char *msg)
{
    if (global_log_level < msg_lvl)
    {
        return;
    }
    
    FILE *p_log_file;
    p_log_file = fopen(global_log_file_name, "a");
    if (p_log_file == NULL)
    {
        fprintf(stderr, "%s\n", WARNING_BAR_STR);
        fprintf(stderr, "\t\t!!! WARNING !!!\n");
        fprintf(stderr, "Unable to open logfile \"%s\". Current actions are not"
                " logged\n", global_log_file_name);
        fprintf(stderr, "\t\t!!! WARNING !!!\n");
        fprintf(stderr, "%s\n", WARNING_BAR_STR);
        return;
    }

    char s_time[MAX_TIME_STR_LEN];
    get_log_time(s_time);
    fprintf(p_log_file, "%s %s: %s\n", s_time, print_log_level(msg_lvl), msg);
    
    fclose(p_log_file);
}


void set_logging_level(enum log_levels lvl)
{
    global_log_level = lvl;
}


void set_logging_file_name(char *f_name)
{
    int i = 0;
    while (1)
    {
        if (i >= MAX_LOG_FILE_NAME_LEN)
        {
            char log_msg[MAX_LOG_MSG_STR_LEN];
            sprintf(log_msg, "Log file name to long. Must be under %d chars long."
                    " Using default file name \"%s\"", MAX_LOG_MSG_STR_LEN,
                    global_log_file_name);
            write_log(ERROR, log_msg);
        return;
        }
        
        *(global_log_file_name + i) = *(f_name + i);
        
        if (*(f_name + i) == '\0')
        {
            printf("%s\n", global_log_file_name);
            return;
        }
        i++;
    }
}


char *print_log_level(enum log_levels lvl)
{
    switch (lvl)
    {
        case OFF:
            return "OFF";
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARNING";
        case INFO:
            return "INFO";
        default:
            return "UNKNOWN";
    }
}
