/*
File:         log_handler.h
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Header file for log_handler.c. Data struct definitions, macros
              etc.
*/

#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#define WARNING_BAR_STR "=========================================================="

#define MAX_TIME_STR_LEN 64
#define MAX_LOG_MSG_STR_LEN 256
#define MAX_LOG_FILE_NAME_LEN 64

enum log_levels {OFF, ERROR, WARNING, INFO};


/*
Description:    Prints the time, for this function call, to the provided string.
                Time format: YYYY.MM.DD HH:MM:SS.
                
Parameters:     *str - Pointer to string, where time is printed.
                
Return:         -
*/
void get_log_time(char *str);


/*
Description:    Writes a log message *msg to the log file, if the provided log
                level is as or more important, then the set logging level. Log
                level and time stamp are automatically added.
                
Parameters:     *msg - Pointer to string, containing log level.
                msg_lvl - Messages importance (log level) value.
                
Return:         -
*/
void write_log(enum log_levels msg_lvl, char *msg);


/*
Description:    Changes the global logging level.
                
Parameters:     lvl - New logging level value.
                
Return:         -
*/
void set_logging_level(enum log_levels lvl);


/*
Description:    Changes the global logging file name (path).
                
Parameters:     *f_name - Pointer to string, containing new file name (path).
                
Return:         -
*/
void set_logging_file_name(char *f_name);


/*
Description:    According to provided log level lvl, returns a pointer to
                string. The string contains the log level in text form.
                
Parameters:     lvl - Log level, for which to get a printable string for.
                
Return:         A pointer to a string, containing log level in printable form.
*/
char *print_log_level(enum log_levels lvl);

#endif
