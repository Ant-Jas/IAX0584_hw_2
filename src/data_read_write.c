/*
File:         data_read_write.c
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Functions that handle errors and the logic of reading from and
              writing to of Homework II-s data files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log_handler.h>
#include <csv_helper.h>
#include <main.h>
#include <data_printing.h>
#include <data_read_write.h>

FILE *open_file(char *f_name, char *mode)
{
    char msg[MAX_ERR_MSG_LEN];
    FILE *fp;
    fp = fopen(f_name, mode);
    if (fp == NULL)
    {
        snprintf(msg, MAX_ERR_MSG_LEN, "Unable to open file \"%s\" in mode "
                 "\"%s\".", f_name, mode);
        fprintf(stderr, "%s\n", msg);
        write_log(ERROR, msg);
        return NULL;
    }
    snprintf(msg, MAX_ERR_MSG_LEN, "Opened file \"%s\" in mode \"%s\".",
             f_name, mode);
    write_log(INFO, msg);
    return fp;
}


int read_data_products(char *f_name, struct product_data_wrapper *pdw)
{
    char msg[MAX_ERR_MSG_LEN];
    char *line_buffer;
    FILE *p_file = open_file(f_name, "r");
    if (p_file == NULL)
    {
        pdw->lines = 0;
        return EXIT_FAILURE;
    }
    
    // Dynamic allocation variables
    struct product_info *p_arr = NULL;
    struct product_info *p_temp = NULL;
    struct product_info pro_buf;
    int count = 0;
    int return_val;
    int alloc_limit = MIN_ALLOC_LINE_CNT;
    
    enum read_errors err_code;
    while (1)
    {
        return_val = read_line(p_file, &line_buffer);
        if (return_val == EOF)
        {
            break;
        }
        else if (return_val == CSV_MALLOC_ERR)
        {
            pdw->data = p_arr;
            pdw->lines = count;
            fclose(p_file);
            return EXIT_FAILURE;
        }
        
        // Allocate memory if necessary
        if (count >= alloc_limit || count == 0)
        {
            alloc_limit *= 2;
            p_temp = realloc(p_arr, pdw->data_struct_size * (size_t)(alloc_limit));
                        
            // Have same data read before simulating realloc fail
            #ifdef FUNC_READ_DATA_PRODUCTS_TEST
            if (count > MIN_ALLOC_LINE_CNT * 2)
            {
                printf("Simulating realloc fail. (Products reading)\n");
                if (p_temp != NULL)
                {
                    p_arr = p_temp;
                    p_temp = NULL;
                }
            }
            #endif
            
            if (p_temp == NULL)
            {
                snprintf(msg, MAX_ERR_MSG_LEN, "Unable to expand data array from"
                         " length %d to %d", count, alloc_limit);
                write_log(ERROR, msg);
                fprintf(stderr, "%s\n", msg);
                pdw->data = p_arr;
                pdw->lines = count;
                fclose(p_file);
                free_buffer_manually();
                return EXIT_FAILURE;
            }
            p_arr = p_temp;
        }
        
        err_code = get_product_info(&pro_buf, line_buffer);
        
        // Copy buffer to data array
        *(p_arr + count) = pro_buf;
        count++;
        
        if (err_code != READ_OK)
        {
            if (print_read_error(err_code, f_name, count) == READ_ERR_FATAL)
            {
                pdw->data = p_arr;
                pdw->lines = count;
                fclose(p_file);
                free_buffer_manually();
                return EXIT_FAILURE;
            }
        }
    }
    fclose(p_file);
    snprintf(msg, MAX_ERR_MSG_LEN, "Closed file \"%s\".", f_name);
    write_log(INFO, msg);
    
    // Free excess allocated memory
    p_temp = realloc(p_arr, pdw->data_struct_size * (size_t)(count));
    if (p_temp == NULL)
    {
        snprintf(msg, MAX_ERR_MSG_LEN, "Unable to free excess memory");
        write_log(ERROR, msg);
        fprintf(stderr, "%s\n", msg);
        pdw->data = p_arr;
        pdw->lines = count;
        return EXIT_FAILURE;
    }
    
    // Save to wrapper
    pdw->data = p_temp;
    pdw->lines = count;
    snprintf(msg, MAX_ERR_MSG_LEN, "Product data read successfully.");
    write_log(INFO, msg);
    return EXIT_SUCCESS;
}


int get_product_info(struct product_info *pi, char *buf)
{
    char field[STR_MAX];
    char *p_field;
    int error_status = READ_OK; // For non fatal errors
    
    // Initialize in case an error might occur
    pi->p_code = NULL;
    pi->p_name = NULL;
    pi->p_os = NULL;
    
    // Getting product code
    strcpy(field, buf);
    p_field = get_field(field, CSV_PRO_FIELD_CODE);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    pi->p_code = dynamic_string(p_field);
    if (pi->p_code == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting product name
    strcpy(field, buf);
    p_field = get_field(field, CSV_PRO_FIELD_NAME);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    pi->p_name = dynamic_string(p_field);
    if (pi->p_name == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting product OS
    strcpy(field, buf);
    p_field = get_field(field, CSV_PRO_FIELD_OS);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    pi->p_os = dynamic_string(p_field);
    if (pi->p_os == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting product RAM
    strcpy(field, buf);
    p_field = get_field(field, CSV_PRO_FIELD_RAM);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    if (sscanf(p_field, "%d", &pi->ram) != 1)
    {
       pi->ram = 0;
       error_status = READ_ERR_RAM_NINT;
    }
    if (pi->ram < 0)
    {
        pi->ram = 0;
        error_status = READ_ERR_RAM_NEG;
    }
    
    // Getting product screen size
    strcpy(field, buf);
    p_field = get_field(field, CSV_PRO_FIELD_SCRN);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    if (sscanf(p_field, "%f", &pi->screen_size) != 1)
    {
        pi->screen_size = 0.0f;
        error_status = READ_ERR_SCRNS_NFLOAT;
    }
    if (pi->screen_size < 0.0f)
    {
        pi->screen_size = 0.0f;
        error_status = READ_ERR_SCRNS_NEG;
    }
    
    return error_status;
}


int read_data_quotes(char *f_name, struct quote_data_wrapper *qdw)
{
    char msg[MAX_ERR_MSG_LEN];
    char *line_buffer;
    FILE *p_file = open_file(f_name, "r");
    if (p_file == NULL)
    {
        qdw->lines = 0;
        return EXIT_FAILURE;
    }
    
    // Dynamic allocation variables
    struct quote_info *p_arr = NULL;
    struct quote_info *p_temp = NULL;
    struct quote_info qte_buf;
    int count = 0;
    int return_val;
    int alloc_limit = MIN_ALLOC_LINE_CNT;
    
    enum read_errors err_code;
    while (1)
    {
        return_val = read_line(p_file, &line_buffer);
        if (return_val == EOF)
        {
            break;
        }
        else if (return_val == CSV_MALLOC_ERR)
        {
            qdw->data = p_arr;
            qdw->lines = count;
            fclose(p_file);
            return EXIT_FAILURE;
        }
        
        // Allocate memory if necessary
        if (count >= alloc_limit || count == 0)
        {
            alloc_limit *= 2;
            p_temp = realloc(p_arr, qdw->data_struct_size * (size_t)(alloc_limit));
            
            // Have same data read before simulating realloc fail
            #ifdef FUNC_READ_DATA_QUOTES_TEST
            if (count > MIN_ALLOC_LINE_CNT * 2)
            {
                printf("Simulating realloc fail. (Quotes reading)\n");
                if (p_temp != NULL)
                {
                    p_arr = p_temp;
                    p_temp = NULL;
                }
            }
            #endif
            
            if (p_temp == NULL)
            {
                snprintf(msg, MAX_ERR_MSG_LEN, "Unable to expand data array from"
                         " length %d to %d", count, count + 1);
                write_log(ERROR, msg);
                fprintf(stderr, "%s\n", msg);
                qdw->data = p_arr;
                qdw->lines = count;
                fclose(p_file);
                free_buffer_manually();
                return EXIT_FAILURE;
            }
            p_arr = p_temp;
        }
        
        err_code = get_quote_info(&qte_buf, line_buffer);
        
        // Copy buffer to data array
        *(p_arr + count) = qte_buf;
        count++;
        
        if (err_code != READ_OK)
        {
            if (print_read_error(err_code, f_name, count) == READ_ERR_FATAL)
            {
                qdw->data = p_arr;
                qdw->lines = count;
                fclose(p_file);
                free_buffer_manually();
                return EXIT_FAILURE;
            }
        }
    }
    fclose(p_file);
    snprintf(msg, MAX_ERR_MSG_LEN, "Closed file \"%s\".", f_name);
    write_log(INFO, msg);
    
    // Free excess allocated memory
    // Fatal ?
    p_temp = realloc(p_arr, qdw->data_struct_size * (size_t)(count));
    if (p_temp == NULL)
    {
        snprintf(msg, MAX_ERR_MSG_LEN, "Unable to free excess memory");
        write_log(ERROR, msg);
        fprintf(stderr, "%s\n", msg);
        qdw->data = p_arr;
        qdw->lines = count;
        return EXIT_FAILURE;
    }
    
    // Save to wrapper
    qdw->data = p_temp;
    qdw->lines = count;
    snprintf(msg, MAX_ERR_MSG_LEN, "Quote data read successfully.");
    write_log(INFO, msg);
    return EXIT_SUCCESS;
}


int get_quote_info(struct quote_info *qi, char *buf)
{
    char field[STR_MAX];
    char *p_field;
    int error_status = READ_OK; // For non fatal errors
    
    // Initialize in case an error might occur
    qi->p_id = NULL;
    qi->p_code = NULL;
    qi->p_retailer = NULL;
    
    // Getting quote id
    strcpy(field, buf);
    p_field = get_field(field, CSV_QTE_FIELD_ID);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    qi->p_id = dynamic_string(p_field);
    if (qi->p_id == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting product code (FK)
    strcpy(field, buf);
    p_field = get_field(field, CSV_QTE_FIELD_CODE);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    qi->p_code = dynamic_string(p_field);
    if (qi->p_code == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting quote retailer
    strcpy(field, buf);
    p_field = get_field(field, CSV_QTE_FIELD_RTLR);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    qi->p_retailer = dynamic_string(p_field);
    if (qi->p_retailer == NULL)
    {
        return READ_ERR_STR_MALLOC;
    }
    
    // Getting quote price
    strcpy(field, buf);
    p_field = get_field(field, CSV_QTE_FIELD_PRICE);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    if (sscanf(p_field, "%d", &qi->price) != 1)
    {
       qi->price = 0;
       error_status = READ_ERR_PRICE_NINT;
    }
    else if (qi->price < 0)
    {
        error_status = READ_ERR_PRICE_NEG;
    }
    
    // Getting quoted items stock
    strcpy(field, buf);
    p_field = get_field(field, CSV_QTE_FIELD_STOCK);
    if (p_field == NULL)
    {
        return READ_ERR_MSNG_DATA;
    }
    if (sscanf(p_field, "%d", &qi->stock) != 1)
    {
       qi->stock = 0;
       error_status = READ_ERR_STOCK_NINT;
    }
    else if (qi->stock < 0)
    {
        error_status = READ_ERR_STOCK_NEG;
    }
    
    return error_status;
}


int print_read_error(enum read_errors err, char *f_name, int line)
{
    char err_msg[MAX_ERR_MSG_LEN];
    switch (err)
    {
        case READ_ERR_MSNG_DATA:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Line: %d from file \"%s\" is "
                     "missing data fields.", line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_FATAL;
        
        case READ_ERR_STR_MALLOC:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Could not allocate memory for "
                     "string type date field at line: %d from file \"%s\".",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_FATAL;
        
        case READ_ERR_RAM_NINT:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Product RAM value at line: %d "
                     "in file \"%s\" is not an integer. It will be set to 0",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_RAM_NEG:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Product RAM value at line: %d "
                     "in file \"%s\" is negative. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_SCRNS_NFLOAT:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Product screen size at line: %d"
                     " in file \"%s\" is not a float. It will be set to 0",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_SCRNS_NEG:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Product screen size at line: %d"
                     " in file \"%s\" is negative. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_PRICE_NINT:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Quote price value at line: %d"
                     " in file \"%s\" is not an integer. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_PRICE_NEG:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Quote price value at line: %d"
                     " in file \"%s\" is negative. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_STOCK_NINT:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Quote stock value at line: %d"
                     " in file \"%s\" is not an integer. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
        
        case READ_ERR_STOCK_NEG:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Quote stock value at line: %d"
                     " in file \"%s\" is negative. It will be set to 0.",
                     line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
            
        default:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Unknown error with value %d "
                     " called by read error at line: %d in file \"%s\".",
                     err, line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_FATAL;
    }
}


int save_product_file_changes(char *f_name, struct product_data_wrapper pdw)
{
    FILE *p_file = open_file(f_name, "w");
    if (p_file == NULL)
    {
        return CSV_WRITE_FOPEN_ERR;
    }
    
    for (int i = 0; i < pdw.lines; i++)
    {
        print_product_csv_line(p_file, *(pdw.data + i));
    }
    
    fclose(p_file);
    char msg[STR_MAX];
    snprintf(msg, STR_MAX, "Closed file \"%s\".", f_name);
    write_log(INFO, msg);
    
    return CSV_WRITE_OK;
}


int save_quote_file_changes(char *f_name, struct quote_data_wrapper qdw)
{
    FILE *p_file = open_file(f_name, "w");
    if (p_file == NULL)
    {
        return CSV_WRITE_FOPEN_ERR;
    }
    
    for (int i = 0; i < qdw.lines; i++)
    {
        print_quote_csv_line(p_file, *(qdw.data + i));
    }
    
    fclose(p_file);
    char msg[STR_MAX];
    snprintf(msg, STR_MAX, "Closed file \"%s\".", f_name);
    write_log(INFO, msg);
    
    return CSV_WRITE_OK;
}
