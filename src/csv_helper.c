#include <stdio.h>
#include <csv_helper.h>
#include <log_handler.h>

int read_line(FILE *p_file, char *str)
{    
    int chars_read = 0;
    while (1)
    {
        if (fgets(str, STR_MAX, p_file) == NULL)  // NULL if EOF and no chars read
        { 
            return EOF;
        }
        chars_read = (int)strlen(str);
        if (chars_read == STR_MAX - 1)
        {
            fprintf(stderr, "While reading a line, buffer was completely filled"
                    ". Loss of data may be possible. Check if any input line is"
                    " longer then buffer length: %d.\n", STR_MAX - 1);
            
            // Get rid of lost data, to not break reading
            while ((int)fgetc(p_file) != '\n' && !feof(p_file));
        }
        if (*str == '\n')
        {
            continue; // Empty line skip
        }
        else if (chars_read > 0 && !feof(p_file))
        {
            *(str + chars_read - 1) = '\0'; // Get rid of newline
            break;
        }
        else if (chars_read > 0 && feof(p_file))
        {
            break;
        }
    }
    #ifdef DEBUG
    printf("Returning line: %s\n", str);
    #endif
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
