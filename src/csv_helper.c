/*
File:         csv_helper.c
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Code for reading a simplified CSV file (no column descriptions and
              no quotations). Dynamic buffers are used for reading, to not limit
              data line length.
*/


#include <stdio.h>
#include <stdlib.h>
#include <log_handler.h>
#include <csv_helper.h>

static char *p_line_buffer = NULL;

int read_line(FILE *p_file, char **str)
{    
    int chars_read = 0;
    static int buffer_len = 0;
    char *temp = NULL;
    char *current = NULL;
    while (1)
    {
        // Allocate more memory, if line buffer is full
        if (buffer_len <= chars_read)
        {
            buffer_len += DYN_BUF_STEP;
            temp = realloc(p_line_buffer, (size_t)buffer_len);
            
            if (temp == NULL)
            {
                char *err = "Failed to allocate memory for dynamic string "
                            "while reading data file.";
                free(p_line_buffer);
                write_log(ERROR, err);
                fprintf(stderr, "%s\n", err);
                *str = NULL;
                return CSV_MALLOC_ERR;
            }
            p_line_buffer = temp;
        }
        
        current = (p_line_buffer + chars_read);
        *current = (char)fgetc(p_file);
        
        if (!chars_read)
        {
            if (feof(p_file)) // EOF, no chars read
            {
                // Buffer reset after finishing every file
                free(p_line_buffer);
                p_line_buffer = NULL;
                buffer_len = 0;
                *str = NULL;
                return EOF;
            }
            if (*current == '\n') // Empty line skip
            {
                continue;
            }
        }
        chars_read++;
        
        if (*current == '\n')
        {
            *current = '\0';  // Get rid of newline and end string
            break;
        }
        if (feof(p_file)) // File ended without newline
        {
            if (buffer_len > chars_read)
            {
                *(current + 1) = '\0';
                break;
            }
            else
            {
                buffer_len += DYN_BUF_STEP;
                temp = realloc(p_line_buffer, (size_t)buffer_len);
                
                if (temp == NULL)
                {
                    char *err = "Failed to allocate memory for dynamic string "
                                "while reading data file.";
                    free(p_line_buffer);
                    write_log(ERROR, err);
                    fprintf(stderr, "%s\n", err);
                    *str = NULL;
                    return CSV_MALLOC_ERR;
                }
                
                // Cant use current because of realloc
                *(p_line_buffer + chars_read) = '\0';
                p_line_buffer = temp;
                break;
            }
        }
    }
    
    *str = p_line_buffer;
    
    return chars_read;
}


char *get_field(char *src, int field_num)
{
    #ifdef DEBUG
    printf("Src str: %s\n", src);
    #endif
    
    int i = 0;
    int field_cnt = 1;
    
    while (field_cnt < field_num)
    {   
        if (*(src + i) == CSV_DELIMITER)
        {
            field_cnt++;
        }
        else if (*(src + i) == '\0')
        {
            return NULL;
        }
        i++;
    }
    while (*(src + i) == ' ') // Removes leading spaces from a field
    {
        i++;
    }
    int j;
    for (j = i; *(src + j) != CSV_DELIMITER && *(src + j) != '\0'; j++);
    *(src + j) = '\0';
    return src + i;
}


void free_buffer_manually(void)
{
    free(p_line_buffer);
    p_line_buffer = NULL;
}
