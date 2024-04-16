#ifndef _MAIN_H
#define _MAIN_H

#define DEBUG

#define MIN_ARGS_TO_PARSE 1

#define DYN_INPUT_STR_LEN_MIN 32
#define STR_MAX 256
#define USER_INT_PROMPT_LEN 128

#define MIN_ALLOC_LINE_CNT 8

// CSV and CSV reading macros
// Product file fields. Index of first field is 1
#define CSV_PRO_FIELD_CODE 1
#define CSV_PRO_FIELD_NAME 2
#define CSV_PRO_FIELD_RAM 3
#define CSV_PRO_FIELD_SCRN 4
#define CSV_PRO_FIELD_OS 5

// Quote file fields. Index of first field is 1
#define CSV_QTE_FIELD_ID 1
#define CSV_QTE_FIELD_CODE 2
#define CSV_QTE_FIELD_RTLR 3
#define CSV_QTE_FIELD_PRICE 4
#define CSV_QTE_FIELD_STOCK 5

// Read error severity
#define READ_ERR_NOT_FATAL 0
#define READ_ERR_FATAL 1

// Currency: cents to euros
#define CNTS_TO_EUR(cnts) (cnts / 100.0f)

// Menu options
enum menu_options {MENU_OPT_EXIT, MENU_OPT_DISP_DATA, MENU_OPT_EDIT_RAM,
                  MENU_OPT_EDIT_RTLR, MENU_OPT_SRCH_PRO, MENU_OPT_CNT};

// Read errors
enum read_errors {READ_OK, READ_ERR_MSNG_DATA, READ_ERR_STR_MALLOC,
                  READ_ERR_RAM_NINT, READ_ERR_SCRNS_NFLOAT, READ_ERR_PRICE_NINT,
                  READ_ERR_PRICE_NEG, READ_ERR_STOCK_NINT, READ_ERR_STOCK_NEG};


/*
    Struct that holds all the available information about one product, from the
    products input file.
*/
struct product_info
{
    char *p_code;       // Product code
    char *p_name;       // Product name
    char *p_os;         // Operating system
    int ram;            // RAM in MB
    float screen_size;  // Screen size in inches
};


/*
    Wrapper for struct product_info. Has information about the structs size
    in bytes, how many entries (lines) exist and a pointer to the data array.
*/
struct product_data_wrapper
{
    struct product_info *data;
    int lines;
    size_t data_struct_size;
};


/*
    Struct that holds all the available information about one quote of a product,
    from the quotes input file.
*/
struct quote_info
{
    char *p_id;         // Quote id
    char *p_code;       // Product code
    char *p_retailer;   // Retailer
    int price;          // Price in cents
    int stock;          // Stock status and count
};


/*
    Wrapper for struct quote_info. Has information about the structs size
    in bytes, how many entries (lines) exist and a pointer to the data array.
*/
struct quote_data_wrapper
{
    struct quote_info *data;
    int lines;
    size_t data_struct_size;
};

/*
Description:    A helper function for opening file with name f_name and in mode
                mode. Mode is a string similar to the ones passed to function
                fopen. Checks if file was opened successfully. Also handles log
                writing and error printing for associated actions.
                
Parameters:     *f_name - Pointer to string containing file name
                *mode - Pointer to string containing info about how the file
                        should be opened
                
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
Description:    Creates a dynamically allocated string, with the exact length
                needed for containing an origin string *orgn_str. If allocation
                is successful, copies the origin string over to the new string.
                
Parameters:     *orgn_str - Pointer to the origin string.
                
Return:         Pointer to the new dynamic string, if allocation was successful.
                NULL if allocation failed.
*/
char *dynamic_string(char *orgn_str);


/*
Description:    Frees all dynamically allocated memory, that is used for storing
                the names, codes, and OS names of all the products. The number
                of data entries to free and the pointer to the data array are
                stored in the wrapper *pwd.
                
Parameters:     *pdw - Wrapper for the product data array.
                
Return:         -
*/
void free_product_info(struct product_data_wrapper *pdw);


/*
Description:    Frees all dynamically allocated memory, that is used for storing
                the IDs, codes, and retailer names of all the quotes. The number
                of data entries to free and the pointer to the data array are
                stored in the wrapper *qdw.
                
Parameters:     *qdw - Wrapper for the quotes data array.
                
Return:         -
*/
void free_quote_info(struct quote_data_wrapper *qdw);


/*
Description:    Calls get_int function until a value of at least min and at most
                max, is returned. Prints error if value is not in expected
                range.
                
Parameters:     min - Minimal expected value.
                max - Maximum expected value.
                
Return:         Value in range min-max.
*/
int get_int_in_range(int min, int max);


/*
Description:    Reads text from standard input into a string. Scans the string
                for an integer value. If value is not an integer, an error is
                printed and a warning logged.
                
Parameters:     -
                
Return:         User entered integer.
*/
int get_int(void);


/*
Description:    For every product in product data array, searches through all of
                quote data array for matching product codes. If codes match
                calls a function that prints the quotes info. In case of first
                match calls function to print a header for quotes table. Also
                adds and prints an entry for all quotes. If no quotes are
                available prints no table and an appropriate message.
                Before quotes also prints the products info and separates
                different products with a line (hopefully easier to follow).
                
Parameters:     pdw - Wrapper containing a pointer to product data array and its
                      length.
                qdw - Wrapper containing a pointer to quote data array and its
                      length.
                
Return:         -
*/
void display_quotes_by_product(struct product_data_wrapper pdw,
                               struct quote_data_wrapper qdw);


/*
Description:    Prompts the user for a product code. Product code is used to
                search for the product. If matching product is found, user is
                prompted to enter a new RAM amount (value must be [0; INT_MAX]).
                Old RAM amount is overwritten. Function also logs/prints
                appropriate messages/errors.
                
Parameters:     pdw - Wrapper containing a pointer to product data array and its
                      length.
                
Return:         EXIT_SUCCESS if RAM amount was successfully changed.
                EXIT_FAILURE otherwise.
*/
int edit_product_ram(struct product_data_wrapper pdw);


/*
Description:    Prompts the user for a quote ID. Quote ID is used to search for
                the quote. If matching quote is found, user is prompted to enter
                the new name for the retailer. The new name is also a dynamic
                char array. The old dynamically allocated string for the
                retailer is freed and then replaced by the new one. Function
                also logs/prints appropriate messages/errors.
                
Parameters:     qdw - Wrapper containing a pointer to quote data array and its
                      length.
                
Return:         EXIT_SUCCESS if retailer name was successfully changed.
                EXIT_FAILURE otherwise.
*/
int edit_quote_retailer(struct quote_data_wrapper qdw);


/*
Description:    Reads provided input stream char-by-char until a newline symbol
                '\n' is encountered. Read chars are stored into a dynamically
                allocated string. The string is lengthened after every multiple
                of DYN_INPUT_STR_LEN_MIN chars read. Input stream must be
                flushed beforehand! If EOF flag is raised for the stream being
                read or memory allocation fails the string is freed. Excess
                allocated memory is freed, so returned string uses the exact
                amount of memory needed. Function also logs/prints appropriate
                messages/errors.
                
Parameters:     stream - Pointer to input stream being read.
                
Return:         On success a pointer to a dynamically allocated string.
                On failure a NULL pointer.
*/
char *get_dynamic_input_string(FILE *stream);

#endif
