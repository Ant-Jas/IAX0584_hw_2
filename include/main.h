#ifndef _MAIN_H
#define _MAIN_H

#define DEBUG

#define MIN_ARGS_TO_PARSE 1
#define STR_MAX 256

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

// Read errors
#define READ_ERR_NOT_FATAL 0
#define READ_ERR_FATAL 1

enum read_errors {READ_OK, READ_ERR_MSNG_DATA, READ_ERR_STR_MALLOC,
                  READ_ERR_RAM_NINT, READ_ERR_SCRNS_NFLOAT, READ_ERR_PRICE_NINT,
                  READ_ERR_PRICE_NEG, READ_ERR_STOCK_NINT, READ_ERR_STOCK_NEG};

struct product_info
{
    char *p_code;       // Product code
    char *p_name;       // Product name
    char *p_os;         // Operating system
    int ram;            // RAM in MB
    float screen_size;  // Screen size in inches
};

struct product_data_wrapper
{
    struct product_info *data;
    int lines;
    size_t data_struct_size;
};

struct quote_info
{
    char *p_id;         // Quote id
    char *p_code;       // Product code
    char *p_retailer;   // Retailer
    int price;          // Price in cents
    int stock;          // Stock status and count
};

struct quote_data_wrapper
{
    struct quote_info *data;
    int lines;
    size_t data_struct_size;
};

FILE *open_file(char *f_name, char *mode);
int read_data_products(char *f_name, struct product_data_wrapper *pdw);
int get_product_info(struct product_info *pi, char *buf);
int read_data_quotes(char *f_name, struct quote_data_wrapper *qdw);
int get_quote_info(struct quote_info *qi, char *buf);
int print_read_error(enum read_errors err, char *f_name, int line);
char *dynamic_string(char *orgn_str);
void free_product_info(struct product_data_wrapper *pwd);
void free_quote_info(struct quote_data_wrapper *qdw);

#endif
