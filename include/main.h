/*
File:         main.h
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Header file for main code file. Data struct definitions, macros
              etc.
*/

#ifndef _MAIN_H
#define _MAIN_H

#define DEBUG
#define MAX_ERR_MSG_LEN 256

#define MIN_ARGS_TO_PARSE 1

#define DYN_INPUT_STR_LEN_MIN 32
#define STR_MAX 256
#define USER_INT_PROMPT_LEN 128

#define MIN_ALLOC_LINE_CNT 8

// Product search return values
#define SRCH_RES_NEG        0
#define SRCH_RES_POS        1
#define SRCH_RES_NO_STOCK   2
#define SRCH_RES_INPUT_ERR -1

// Data edit errors
#define EDIT_OK             0
#define EDIT_NO_MATCH       1
#define EDIT_MALLOC         2

// CSV writing errors
#define CSV_WRITE_OK        0
#define CSV_WRITE_FOPEN_ERR 1

// Currency: cents to euros
#define CNTS_TO_EUR(cnts) (cnts / 100.0f)

// Menu options
enum menu_options {MENU_OPT_EXIT, MENU_OPT_DISP_DATA, MENU_OPT_EDIT_RAM,
                  MENU_OPT_EDIT_RTLR, MENU_OPT_SRCH_PRO, MENU_OPT_CNT};

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
                
Return:         EDIT_OK (0) if RAM amount was successfully changed.
                EDIT_NO_MATCH (1) if no matching product was found
                EDIT_MALLOC (2) if dynamic memory allocation for string(s)
                failed.
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
                
Return:         EDIT_OK (0) if RAM amount was successfully changed.
                EDIT_NO_MATCH (1) if no matching product was found
                EDIT_MALLOC (2) if dynamic memory allocation for string(s)
                failed.
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


/*
Description:    Checks if user entered string matches any product name. If
                product exists, checks if there are any quotes for it. If
                quote(s) exist, prints the cheapest option.
                
Parameters:     pdw - Wrapper containing a pointer to product data array and its
                      length.
                qdw - Wrapper containing a pointer to quote data array and its
                      length.
                
Return:         SRCH_RES_INPUT_ERR - If there was an error with input string
                memory allocation.
                SRCH_RES_NEG - If product searched for does not exist.
                SRCH_RES_NO_STOCK - If there is no stock for product.
                SRCH_RES_POS - If the product exists and is in stock.
*/
int search_best_price(struct product_data_wrapper pdw,
                       struct quote_data_wrapper qdw);

#endif
