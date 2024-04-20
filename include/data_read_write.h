/*
File:         data_read_write.h
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Header file for data_read_write.c. Data struct definitions, macros
              etc.
*/

#ifndef _DATA_READ_WRITE_H
#define _DATA_READ_WRITE_H

#include <main.h>

// CSV and CSV reading macros
// Product file fields. Index of first field is 1
#define CSV_PRO_FIELD_CODE  1
#define CSV_PRO_FIELD_NAME  2
#define CSV_PRO_FIELD_RAM   3
#define CSV_PRO_FIELD_SCRN  4
#define CSV_PRO_FIELD_OS    5

// Quote file fields. Index of first field is 1
#define CSV_QTE_FIELD_ID    1
#define CSV_QTE_FIELD_CODE  2
#define CSV_QTE_FIELD_RTLR  3
#define CSV_QTE_FIELD_PRICE 4
#define CSV_QTE_FIELD_STOCK 5

// Read error severity
#define READ_ERR_NOT_FATAL  0
#define READ_ERR_FATAL      1

// Read errors
enum read_errors {READ_OK, READ_ERR_MSNG_DATA, READ_ERR_STR_MALLOC,
                  READ_ERR_RAM_NINT, READ_ERR_SCRNS_NFLOAT, READ_ERR_PRICE_NINT,
                  READ_ERR_PRICE_NEG, READ_ERR_STOCK_NINT, READ_ERR_STOCK_NEG};

/*
Description:    A helper function for opening file with name f_name and in mode
                mode. Mode is a string similar to the ones passed to function
                fopen. Checks if file was opened successfully. Also handles log
                writing and error printing for associated actions.
                
Parameters:     *f_name - Pointer to string containing file name.
                *mode - Pointer to string containing info about how the file
                        should be opened.
                
Return:         Pointer to opened file. NULL if unsuccessful.
*/
FILE *open_file(char *f_name, char *mode);


/*
Description:    First calls a function to read a line from a csv file with name
                f_name. Then calls another function to interpret the line into
                buffer variables. The function itself creates a dynamic array to
                which it saves the buffered values to. If needed, the dynamic
                array is lengthened according to 2*n principle. If reading is
                done, excess allocated memory is freed. The function contains
                error printing and logging.
                
Parameters:     *f_name - Pointer to string containing file name.
                *pdw - Pointer to a wrapper for product info array.
                
Return:         EXIT_SUCCESS (0) if all data was read successfully. Otherwise
                EXIT_FAILURE.
*/
int read_data_products(char *f_name, struct product_data_wrapper *pdw);


/*
Description:    A pointer to a struct *pi of buffer variables is passed to this
                function together with *buf, that points to a data line read
                from the products info data file. Using the get_field function,
                data from *buf is interpreted into buffer variables. Necessary
                checks are conducted.
                
Parameters:     *pi - Pointer to a struct of buffer variables.
                *buf - Pointer to a string of read csv data.
                
Return:         READ_OK (0 - enum value) if all data was read successfully.
                Otherwise a value corresponding to the first encountered error.
*/
int get_product_info(struct product_info *pi, char *buf);


/*
Description:    First calls a function to read a line from a csv file with name
                f_name. Then calls another function to interpret the line into
                buffer variables. The function itself creates a dynamic array to
                which it saves the buffered values to. If needed, the dynamic
                array is lengthened according to 2*n principle. If reading is
                done, excess allocated memory is freed. The function contains
                error printing and logging.
                
Parameters:     *f_name - Pointer to string containing file name.
                *qdw - Pointer to a wrapper for product info array.
                
Return:         EXIT_SUCCESS (0) if all data was read successfully. Otherwise
                EXIT_FAILURE.
*/
int read_data_quotes(char *f_name, struct quote_data_wrapper *qdw);


/*
Description:    A pointer to a struct *qi of buffer variables is passed to this
                function together with *buf, that points to a data line read
                from the quotes info data file. Using the get_field function,
                data from *buf is interpreted into buffer variables. Necessary
                checks are conducted.
                
Parameters:     *qi - Pointer to a struct of buffer variables.
                *buf - Pointer to a string of read csv data.
                
Return:         READ_OK (0 - enum value) if all data was read successfully.
                Otherwise a value corresponding to the first encountered error.
*/
int get_quote_info(struct quote_info *qi, char *buf);


/*
Description:    Prints and logs an error message according to an enum value err
                returned from a read function. If necessary specifies a file
                name f_name and the line number line, where the error occurred,
                for easier troubleshooting.
                
Parameters:     err - enum value of error message.
                *f_name - Pointer to a string containing the file name where the
                          error occurred.
                line - Line number where the error occurred.
                
Return:         READ_OK (0 - enum value) if all data was read successfully.
                Otherwise a value corresponding to the first encountered error.
*/
int print_read_error(enum read_errors err, char *f_name, int line);


/*
Description:    Writes all data from products data array into a CSV file.
                
Parameters:     f_name - File name (path) of output file.
                pdw - Wrapper containing a pointer to product data array and its
                      length.
                
Return:         CSV_WRITE_FOPEN_ERR - If an error occurs while opening the file.
                CSV_WRITE_OK - Data was successfully written to the file.
*/
int save_product_file_changes(char *f_name, struct product_data_wrapper pdw);


/*
Description:    Writes all data from quotes data array into a CSV file.
                
Parameters:     f_name - File name (path) of output file.
                qdw - Wrapper containing a pointer to quote data array and its
                      length.
                
Return:         CSV_WRITE_FOPEN_ERR - If an error occurs while opening the file.
                CSV_WRITE_OK - Data was successfully written to the file.
*/
int save_quote_file_changes(char *f_name, struct quote_data_wrapper qdw);

#endif
