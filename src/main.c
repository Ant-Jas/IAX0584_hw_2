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
        free_product_info(&products_wrapper);
        free_quote_info(&quotes_wrapper);
        return EXIT_FAILURE;
    }
    
    char msg[STR_MAX];
    int menu_action;
    do
    {
        print_menu();
        menu_action = get_int_in_range(MENU_OPT_EXIT, MENU_OPT_CNT - 1);
        switch (menu_action)
        {
            case MENU_OPT_EXIT:
                break;
            
            case MENU_OPT_DISP_DATA:
                display_quotes_by_product(products_wrapper, quotes_wrapper);
                break;
            
            case MENU_OPT_EDIT_RAM:
                break;
            
            case MENU_OPT_EDIT_RTLR:
                break;
            
            case MENU_OPT_SRCH_PRO:
                break;
            default:
                snprintf(msg, STR_MAX, "An unknown menu option with value: %d "
                         "received.", menu_action);
                write_log(WARNING, msg);
                fprintf(stderr, "%s\n", msg);
                break;
        }
    }
    while (menu_action != MENU_OPT_EXIT);
    
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
        fprintf(stderr, "%s\n", msg);
        write_log(ERROR, msg);
        return NULL;
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
                pdw->lines = count;
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
                pdw->lines = count;
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
                qdw->lines = count;
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
                qdw->lines = count;
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


int get_int_in_range(int min, int max)
{
    int val;
    while (1)
    {
        val = get_int();
        if (val >= min && val <= max)
        {
                return val;
        }
        fprintf(stderr, "Value must be in range [%d - %d]\n", min, max);
    }
}


int get_int(void)
{
    int val;
    char buf[USER_INT_PROMPT_LEN];
    while (1)
    {
        printf("> ");
        scanf("%s", buf);
        if (sscanf(buf, "%d", &val) == 1)
        {
            return val;
        }
        else
        {
            fprintf(stderr, "\"%s\" is not a number\n", buf);
            strcat(buf, " - non integer value entered by user.");
            write_log(WARNING, buf);
        }
    }
}


void print_menu(void)
{
    putchar('\n');
    printf("%d - Print all data\n", MENU_OPT_DISP_DATA);
    printf("%d - Edit product RAM\n", MENU_OPT_EDIT_RAM);
    printf("%d - Edit quote retailer\n", MENU_OPT_EDIT_RTLR);
    printf("%d - Search for product\n", MENU_OPT_SRCH_PRO);
    printf("%d - EXIT\n", MENU_OPT_EXIT);
    putchar('\n');
}


void display_quotes_by_product(struct product_data_wrapper pdw,
                               struct quote_data_wrapper qdw)
{
    for (int i = 0; i < pdw.lines; i++)
    {
        int nr = 0;
        print_product_specs(*(pdw.data + i));
        
        for (int j = 0; j < qdw.lines; j++)
        {
            if (strcmp((*(pdw.data + i)).p_code, (*(qdw.data + j)).p_code) == 0)
            {
                if (nr == 0)
                {
                    printf("\nQuotes:\n");
                    print_quote_table_head();
                }
                nr++;
                printf("\t%3d ", nr);
                print_product_quote(*(qdw.data + j));
            }
        }
        if (nr <= 0)
        {
            printf("\nNo quotes for %s available.\n", (pdw.data + i)->p_name);
        }
        putchar('\n');
        if (i != (pdw.lines - 1))
        {
            print_separator_line();
        }
    }
}


void print_product_specs(struct product_info pi)
{
    printf("\nProduct: %s\n", pi.p_name);
    printf("\t%-13s %d MB\n", "RAM:", pi.ram);
    printf("\t%-13s %.1f \"\n", "Screen size:", pi.screen_size);
    printf("\t%-13s %s\n", "OS:", pi.p_os);
    printf("\t%-13s %s\n", "Product code:", pi.p_code);
}


void print_product_quote(struct quote_info qi)
{
    printf("| %-16s ", qi.p_retailer);
    printf("| %8.2f EUR ", CNTS_TO_EUR((float)qi.price));
    if (qi.stock > 0)
    {
        printf("| %3d %-8s ", qi.stock, "In Stock");
    }
    else
    {
        printf("| %3s %-8s ", "", "Order");
    }
    printf("| %s", qi.p_id);
    putchar('\n');
}


void print_quote_table_head(void)
{
    putchar('\t');
    printf("%3s ", "Nr.");
    printf("| %16s ", "Retailer");
    printf("| %12s ", "Price");
    printf("| %12s ", "Stock status");
    printf("| %s", "Quote ID");
    putchar('\n');
}


void print_separator_line(void)
{
    for (int i = 0; i < SEP_LINE_LEN; i++)
    {
        putchar('-');
    }
    putchar('\n');
}

// Non fatal errors overwrite each other
// Add dynamic read line buffer (lib global pointer, allocate when called first,
// extend when needed, free if reading return NULL or other error - return NULL)
