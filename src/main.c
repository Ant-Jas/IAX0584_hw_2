#include <stdlib.h>
#include <log_handler.h>
#include <arg_parse.h>
#include <csv_helper.h>
#include <main.h>

int main(int argc, char **argv)
{
    // Default logging level: INFO & file name: "log.txt" in log lib
    //set_logging_level(INFO);
    //set_logging_file_name("log.txt");
    write_log(INFO, "Starting program.");
    
    // Supported command line arguments
    struct argument_description options[] =
    {
        {ARG_FILE_PRO, "--file_products", 2},
        {ARG_FILE_QTE, "--file_quotes", 2},
        {LOG_FILE, "--file_log", 2},
        {LOG_LEVEL, "--log_level", 2}
    };
    
    // Default argument values
    struct argument arguments =
    {
        .f_pro = "data/products.csv",
        .f_qte = "data/quotes.csv",
    };
    
    if (argc > MIN_ARGS_TO_PARSE)
    {
        parse_arguments(options, &arguments, argc, argv);
        write_log(INFO, "Parsed command line arguments successfully.");
    }
    else
    {
        write_log(INFO, "Using default arguments.");
    }
    
    struct product_data_wrapper products_wrapper =
    {
        .data = NULL,
        .lines = 0,
        .data_struct_size = sizeof(struct product_info)
    };
    
    if (read_data_products(arguments.f_pro, &products_wrapper) == EXIT_FAILURE)
    {
        free_product_info(&products_wrapper);
        return EXIT_FAILURE;
    }
    
    struct quote_data_wrapper quotes_wrapper =
    {
        .data = NULL,
        .lines = 0,
        .data_struct_size = sizeof(struct quote_info)
    };
    
    if (read_data_quotes(arguments.f_qte, &quotes_wrapper) == EXIT_FAILURE)
    {
        free_quote_info(&quotes_wrapper);
        return EXIT_FAILURE;
    }
    
    free_product_info(&products_wrapper);
    free_quote_info(&quotes_wrapper);
    
    write_log(INFO, "Closing program successfully.");
    return EXIT_SUCCESS;
}


FILE *open_file(char *f_name, char *mode)
{
    char msg[MAX_ERR_MSG_LEN];
    FILE *fp;
    fp = fopen(f_name, mode);
    if (fp == NULL)
    {
        snprintf(msg, MAX_ERR_MSG_LEN, "Unable to open file \"%s\".", f_name);
        write_log(ERROR, msg);
        exit_with_error(msg);
    }
    snprintf(msg, MAX_ERR_MSG_LEN, "Opened file \"%s\".", f_name);
    write_log(INFO, msg);
    return fp;
}


int read_data_products(char *f_name, struct product_data_wrapper *pdw)
{
    char msg[MAX_ERR_MSG_LEN];
    char line_buffer[STR_MAX];
    FILE *p_file = open_file(f_name, "r");
    
    // Dynamic allocation variables
    struct product_info *p_arr = NULL;
    struct product_info *p_temp = NULL;
    struct product_info pro_buf;
    int count = 0;
    int alloc_limit = MIN_ALLOC_LINE_CNT;
    
    enum read_errors err_code;
    while (read_line(p_file, line_buffer) != EOF)
    {
        err_code = get_product_info(&pro_buf, line_buffer);
        if (err_code != READ_OK)
        {
            if (print_read_error(err_code, f_name, count) == READ_ERR_FATAL)
            {
                pdw->data = p_arr;
                return EXIT_FAILURE;
            }
        }
        
        // Allocate memory if necessary
        if (count >= alloc_limit || count == 0)
        {
            alloc_limit *= 2;
            p_temp = realloc(p_arr, pdw->data_struct_size * (size_t)(alloc_limit));
            if (p_temp == NULL)
            {
                snprintf(msg, MAX_ERR_MSG_LEN, "Unable to expand data array from"
                         " length %d to %d", count, count + 1);
                write_log(ERROR, msg);
                fprintf(stderr, "%s\n", msg);
                pdw->data = p_arr;
                return EXIT_FAILURE;
            }
            p_arr = p_temp;
        }
        
        // Copy buffer to data array
        *(p_arr + count) = pro_buf;
        count++;
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
    
    return error_status;
}


int read_data_quotes(char *f_name, struct quote_data_wrapper *qdw)
{
    char msg[MAX_ERR_MSG_LEN];
    char line_buffer[STR_MAX];
    FILE *p_file = open_file(f_name, "r");
    
    // Dynamic allocation variables
    struct quote_info *p_arr = NULL;
    struct quote_info *p_temp = NULL;
    struct quote_info qte_buf;
    int count = 0;
    int alloc_limit = MIN_ALLOC_LINE_CNT;
    
    enum read_errors err_code;
    while (read_line(p_file, line_buffer) != EOF)
    {
        err_code = get_quote_info(&qte_buf, line_buffer);
        if (err_code != READ_OK)
        {
            if (print_read_error(err_code, f_name, count) == READ_ERR_FATAL)
            {
                qdw->data = p_arr;
                return EXIT_FAILURE;
            }
        }
        
        // Allocate memory if necessary
        if (count >= alloc_limit || count == 0)
        {
            alloc_limit *= 2;
            p_temp = realloc(p_arr, qdw->data_struct_size * (size_t)(alloc_limit));
            if (p_temp == NULL)
            {
                snprintf(msg, MAX_ERR_MSG_LEN, "Unable to expand data array from"
                         " length %d to %d", count, count + 1);
                write_log(ERROR, msg);
                fprintf(stderr, "%s\n", msg);
                qdw->data = p_arr;
                return EXIT_FAILURE;
            }
            p_arr = p_temp;
        }
        
        // Copy buffer to data array
        *(p_arr + count) = qte_buf;
        count++;
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
                     "in file \"%s\" is not an integer.", line, f_name);
            write_log(ERROR, err_msg);
            fprintf(stderr, "%s\n", err_msg);
            return READ_ERR_NOT_FATAL;
            
        case READ_ERR_SCRNS_NFLOAT:
            snprintf(err_msg, MAX_ERR_MSG_LEN, "Product screen size at line: %d"
                     " in file \"%s\" is not a float.", line, f_name);
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


char *dynamic_string(char *orgn_str)
{
    char *dest_str = malloc(sizeof(char) * (strlen(orgn_str) + 1));
    if (dest_str == NULL)
    {
        return NULL;
    }
    strcpy(dest_str, orgn_str);
    return dest_str;
}


void free_product_info(struct product_data_wrapper *pdw)
{
    for (int i = 0; i < pdw->lines; i++)
    {   
        free((pdw->data + i)->p_code);
        free((pdw->data + i)->p_name);
        free((pdw->data + i)->p_os);
        (pdw->data + i)->p_code = NULL;
        (pdw->data + i)->p_name = NULL;
        (pdw->data + i)->p_os = NULL;
    }
    free(pdw->data);
    pdw->data = NULL;
}


void free_quote_info(struct quote_data_wrapper *qdw)
{
    for (int i = 0; i < qdw->lines; i++)
    {   
        free((qdw->data + i)->p_id);
        free((qdw->data + i)->p_code);
        free((qdw->data + i)->p_retailer);
        (qdw->data + i)->p_id = NULL;
        (qdw->data + i)->p_code = NULL;
        (qdw->data + i)->p_retailer = NULL;
    }
    free(qdw->data);
    qdw->data = NULL;
}

// Non fatal errors overwrite each other
