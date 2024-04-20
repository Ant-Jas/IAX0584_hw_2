/*
File:         csv_helper.h
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Header file for csv_helper.c. Data struct definitions, macros
              etc.
*/

#ifndef _CSV_HELPER_H
#define _CSV_HELPER_H

#define CSV_DELIMITER ';'
#define DYN_BUF_STEP 32

// Errors
#define CSV_MALLOC_ERR 1

/*
Description:    Reads a line from file pointed to by *p_file and saves line to
                a dynamic string buffer. While reading skips empty lines
                (contain only '\n'). After file is over dynamic line buffer is
                freed. If whole file is not read, buffer should be manually
                freed!
                
Parameters:     *p_file - Pointer to file.
                **str - Double pointer that will be pointed to the buffer string.
                
Return:         EOF - if file is over and no data has been read.
                Number of chars read, if any data has been read.
*/
int read_line(FILE *p_file, char **str);


/*
Description:    Reads data line *src char-by-char until an amout (field_num - 1)
                of CSV_DELIMITER is encountered. Removes empty spaces after
                CSV_DELIMITER and before data to get the first chars position in
                the desirable data field. Then goes again char-by-char until end
                of data field (marked by CSV_DELIMITER or '\0') and adds '\0' to
                complete the string for desirable data field.
                
Parameters:     *src - Pointer to string that contains data line.
                field_num - A number that represents the desirable data fields
                position in the sequence of a data line. Counting starts from 1
                and goes left-to-right.
                
Return:         NULL - If data line does not contain desirable field
                Pointer to the start of the data field.
*/
char *get_field(char *src, int field_num);


/*
Description:    Allows user to manually free the read buffer.
                
Parameters:     -
                
Return:         -
*/
void free_buffer_manually(void);

#endif
