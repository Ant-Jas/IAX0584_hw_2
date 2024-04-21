/*
File:         arg_parse.c
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Code for parsing command line arguments.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log_handler.h>
#include <arg_parse.h>

void parse_arguments(struct argument_description *opts, struct argument *args,
                     int arg_cnt, char **arg_vec)
{
    char err_msg[ERR_MSG_LEN];
    
    for (int i = 1; i < arg_cnt; i++)
    {
        for (int j = 0; j < ARG_SUPPORTED_CNT; j++)
        {
            if (strcmp(*(arg_vec + i), (opts + j)->arg_name) == 0)
            {
                i += change_argument_value(args, j, arg_vec, arg_cnt, i,
                                           (opts + j)->arg_mems);
                break;
            }
            if (j == (ARG_SUPPORTED_CNT - 1))
            {
                snprintf(err_msg, ERR_MSG_LEN, "Unknown argument "
                         "\"%s\" encountered.", *(arg_vec + i));
                write_log(ERROR, err_msg);
                exit_with_error(err_msg);
            }
        }
    }
}


int change_argument_value(struct argument *args, enum argument_cases event,
                          char **arg_vec, int arg_vec_len, int cnt, int arg_mems)
{
    if (arg_mems + cnt > arg_vec_len)
    {
        exit_with_error("Not enough arguments provided. Check if every file name"
                        " argument specifies a name.");
        write_log(ERROR, "Arguments missing parameters.");
    }
    
    char buf[MSG_MAX_LEN];
    
    switch (event)
    {
        case ARG_FILE_PRO:
            strcpy(args->f_pro, *(arg_vec + cnt + 1));
            snprintf(buf, MSG_MAX_LEN, "Using \"%s\" as product file.",
                     *(arg_vec + cnt + 1));
            write_log(INFO, buf);
            break;
            
        case ARG_FILE_QTE:
            strcpy(args->f_qte, *(arg_vec + cnt + 1));
            snprintf(buf, MSG_MAX_LEN, "Using \"%s\" as quotes file.",
                     *(arg_vec + cnt + 1));
            write_log(INFO, buf);
            break;
            
        case LOG_FILE:
            set_logging_file_name(*(arg_vec + cnt + 1));
            snprintf(buf, MSG_MAX_LEN, "Log file changed to \"%s\".",
                     *(arg_vec + cnt + 1));
            write_log(WARNING, buf);
            break;
            
        case LOG_LEVEL:
            int temp;
            if (sscanf(*(arg_vec + cnt + 1), "%d", &temp) != 1)
            {
                snprintf(buf, ERR_MSG_LEN, "\"%s\" is not a supported log "
                         "level value.", *(arg_vec + cnt + 1));
                write_log(ERROR, buf);
                exit_with_error(buf);
            }
            set_logging_level(temp);
            snprintf(buf, MSG_MAX_LEN, "Log level changed to: %d.", temp);
            write_log(INFO, buf);
            break;
            
        default:
            exit_with_error("Error with argument handling setup, check argument"
                            " case values. This is not a user error!");
            write_log(ERROR, "Error with argument handling setup, check argument"
                      " case values. This is not a user error!");
            break;
    }
    return arg_mems - 1;
}


void exit_with_error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    write_log(ERROR, "Exiting program.");
    exit(EXIT_FAILURE);
};
