/*
File:         main.c
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Main code file for homework assignment II - Price Watch. General
              code for setting up, running and closing the program. Also
              functions that manipulate the data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <log_handler.h>
#include <arg_parse.h>
#include <data_read_write.h>
#include <csv_helper.h>
#include <data_printing.h>
#include <main.h>

int main(int argc, char **argv)
{
    // Default logging level: INFO & file name: "log.txt" in log lib
    // Uncomment and change values to change defaults
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
    
    // Parse arguments if needed
    if (argc > MIN_ARGS_TO_PARSE)
    {
        parse_arguments(options, &arguments, argc, argv);
        write_log(INFO, "Parsed command line arguments successfully.");
    }
    else
    {
        write_log(INFO, "Using default arguments.");
    }
    
    // Setup products wrapper and read products data
    struct product_data_wrapper products_wrapper =
    {
        .data = NULL,
        .lines = 0,
        .data_struct_size = sizeof(struct product_info)
    };
    
    if (read_data_products(arguments.f_pro, &products_wrapper) == EXIT_FAILURE)
    {
        free_product_info(&products_wrapper);
        write_log(INFO, "Closing program after encountering an error.");
        return EXIT_FAILURE;
    }
    
    // Setup quotes wrapper and read quotes data
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
        write_log(INFO, "Closing program after encountering an error.");
        return EXIT_FAILURE;
    }
    
    // Menu
    bool products_modified = false;
    bool quotes_modified = false;
    char msg[STR_MAX];
    int menu_action;
    int return_val;
    do
    {
        print_menu();
        menu_action = get_int_in_range(MENU_OPT_EXIT, MENU_OPT_CNT - 1);
        putchar('\n');
        switch (menu_action)
        {
            case MENU_OPT_EXIT:
                break;
            
            case MENU_OPT_DISP_DATA:
                display_quotes_by_product(products_wrapper, quotes_wrapper);
                break;
            
            case MENU_OPT_EDIT_RAM:
                return_val = edit_product_ram(products_wrapper);
                if (return_val == EDIT_OK)
                {
                    products_modified = true;
                }
                else if (return_val == EDIT_MALLOC)
                {
                    free_product_info(&products_wrapper);
                    free_quote_info(&quotes_wrapper);
                    write_log(INFO, "Closing program after encountering an error.");
                    return EXIT_FAILURE;
                }
                break;
            
            case MENU_OPT_EDIT_RTLR:
                return_val = edit_quote_retailer(quotes_wrapper);
                if (return_val == EDIT_OK)
                {
                    quotes_modified = true;
                }
                else if (return_val == EDIT_MALLOC)
                {
                    free_product_info(&products_wrapper);
                    free_quote_info(&quotes_wrapper);
                    write_log(INFO, "Closing program after encountering an error.");
                    return EXIT_FAILURE;
                }
                break;
            
            case MENU_OPT_SRCH_PRO:
                return_val = search_best_price(products_wrapper, quotes_wrapper);
                if (return_val == SRCH_RES_INPUT_ERR)
                {
                    free_product_info(&products_wrapper);
                    free_quote_info(&quotes_wrapper);
                    write_log(INFO, "Closing program after encountering an error.");
                    return EXIT_FAILURE;
                }
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
    
    // Write changes to file if needed
    if (products_modified)
    {
        if (!save_product_file_changes(arguments.f_pro, products_wrapper))
        {
            fprintf(stderr, "Changes made will not be saved.\n");
        }
    }
    if (quotes_modified)
    {
        if(!save_quote_file_changes(arguments.f_qte, quotes_wrapper))
        {
            fprintf(stderr, "Changes made will not be saved.\n");
        }
    }
    
    // Free dynamically allocated memory
    free_product_info(&products_wrapper);
    free_quote_info(&quotes_wrapper);
    
    write_log(INFO, "Closing program successfully.");
    return EXIT_SUCCESS;
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
        fgets(buf, USER_INT_PROMPT_LEN, stdin);
        *(buf + strlen(buf) - 1) = '\0';
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
                    /*
                        This column is not part of quote data printing.
                        Therefore, to have the printing align when using both
                        functions elsewhere, it is also not included in table
                        head printing.
                    */
                    printf("\t%3s ", "Nr.");
                    print_quote_table_head();
                }
                nr++;
                printf("\t%3d ", nr);
                print_product_quote(*(qdw.data + j));
            }
        }
        
        if (nr <= 0)
        {
            printf("\nNo quotes for %s available.\n\n", (pdw.data + i)->p_name);
        }
        putchar('\n');
        
        if (i != (pdw.lines - 1))
        {
            print_separator_line();
        }
    }
    write_log(INFO, "Displayed all product and quote info to user.");
}


int edit_product_ram(struct product_data_wrapper pdw)
{
    printf("Enter phones product code to modify the amount of RAM.\n> ");
    
    char *search_str = get_dynamic_input_string(stdin);
    if (search_str == NULL)
    {
        return EDIT_MALLOC;
    }
    
    char msg[STR_MAX];
    
    for (int i = 0; i < pdw.lines; i++)
    {
        if (strcmp(search_str, (pdw.data + i)->p_code) == 0)
        {
            printf("\nEnter new RAM amount.\n");
            int new_ram = get_int_in_range(0, INT_MAX);
            snprintf(msg, STR_MAX, "Updating products %s RAM: %d -> %d",
                     (pdw.data + i)->p_name, (pdw.data + i)->ram, new_ram);
            (pdw.data + i)->ram = new_ram;
            write_log(INFO, msg);
            printf("%s\n", msg);
            break;
        }
        if (i == (pdw.lines - 1))
        {
            snprintf(msg, STR_MAX, "Search for product with product code: %s, "
                     "returned no results.", search_str);
            write_log(INFO, msg);
            printf("%s Search is case sensitive!\n\n", msg);
            free(search_str);
            return EDIT_NO_MATCH;
        }
    }
    free(search_str);
    return EDIT_OK;
}


int edit_quote_retailer(struct quote_data_wrapper qdw)
{
    printf("Enter quote ID to change the retailers name.\n> ");
    
    char *search_str = get_dynamic_input_string(stdin);
    if (search_str == NULL)
    {
        return EDIT_MALLOC;
    }
    
    char msg[STR_MAX];
    
    for (int i = 0; i < qdw.lines; i++)
    {
        if (strcmp(search_str, (qdw.data + i)->p_id) == 0)
        {
            printf("\nEnter new retailer name.\n> ");
            char *new_retailer = get_dynamic_input_string(stdin);
            if (new_retailer == NULL)
            {
                free(search_str);
                return EDIT_MALLOC;
            }
            
            snprintf(msg, STR_MAX, "Updating quote's %s retailer: %s -> %s",
                     (qdw.data + i)->p_id, (qdw.data + i)->p_retailer,
                     new_retailer);
            free((qdw.data + i)->p_retailer);
            (qdw.data + i)->p_retailer = new_retailer;
            write_log(INFO, msg);
            printf("%s\n\n", msg);
            break;
        }
        if (i == (qdw.lines - 1))
        {
            snprintf(msg, STR_MAX, "Search for quote with id: %s, "
                     "returned no results.", search_str);
            write_log(INFO, msg);
            printf("%s Search is case sensitive!\n\n", msg);
            free(search_str);
            return EDIT_NO_MATCH;
        }
    }
    free(search_str);
    return EDIT_OK;
}


char *get_dynamic_input_string(FILE *stream)
{
    char *str = NULL;
    char *temp = NULL;
    int chars_read = 0;
    int cur_str_len = 0;
    
    // INPUT STREAM MUST BE FLUSHED BEFOREHAND !!!
    while (1)
    {
        // Allocate more memory, if string is full
        if (cur_str_len <= chars_read)
        {
            cur_str_len += DYN_INPUT_STR_LEN_MIN;
            temp = realloc(str, (size_t)cur_str_len);
            
            // Simulates realloc fail
            #ifdef FUNC_GET_DYNAMIC_INPUT_STRING_TEST
            free(temp);
            temp = NULL;
            #endif
            
            if (temp == NULL)
            {
                char *err = "Failed to allocate memory for dynamic string "
                            "while reading input.";
                free(str);
                write_log(ERROR, err);
                fprintf(stderr, "%s\n", err);
                return NULL;
            }
            str = temp;
        }
        
        *(str + chars_read) = (char)fgetc(stream);
        if (feof(stream))
        {
            free(str);
            char *err = "Unexpected EOF occured while reading input into "
                        "dynamic string.";
            write_log(ERROR, err);
            fprintf(stderr, "%s\n", err);
            return NULL;
        }
        if (*(str + chars_read) == '\n')
        {
            *(str + chars_read) = '\0';
            break;
        }
        chars_read++;
    }
    
    // Free unused allocated memory
    temp = dynamic_string(str);
    if (temp == NULL)
    {
        char *err = "Failed to reallocate input string to shorten it.";
        write_log(ERROR, err);
        fprintf(stderr, "%s\n", err);
    }
    free(str);
    str = temp;
    
    return str;
}


int search_best_price(struct product_data_wrapper pdw,
                       struct quote_data_wrapper qdw)
{
    printf("Enter product name to search for.\n> ");
    
    char *search_str = get_dynamic_input_string(stdin);
    if (search_str == NULL)
    {
        return SRCH_RES_INPUT_ERR;
    }
    
    char msg[STR_MAX];
    
    // Find if product exists
    struct product_info *search_res = NULL;
    for (int i = 0; i < pdw.lines; i++)
    {
        if (strcmp(search_str, (pdw.data + i)->p_name) == 0)
        {
            search_res = (pdw.data + i);
            break;
        }
    }
    if (!search_res)
    {
        snprintf(msg, STR_MAX, "Search for product with name \"%s\", "
                 "returned no results.", search_str);
        write_log(INFO, msg);
        printf("%s Search is case sensitive!\n\n", msg);
        free(search_str);
        return SRCH_RES_NEG;
    }
    
    // Find product with matching product code
    struct quote_info *min_price = NULL;
    int j = 0;
    while (j < qdw.lines)
    {
        if (strcmp(search_res->p_code, (qdw.data + j)->p_code) == 0)
        {
            if ((qdw.data + j)->stock)
            {
                min_price = (qdw.data + j);
                break;
            }
        }
        j++;
    }
    if (!min_price)
    {
        // No quote with stock found msg
        snprintf(msg, STR_MAX, "\nNo quotes for product \"%s\" with available "
                 "stock exist.", search_str);
        write_log(INFO, msg);
        printf("%s\n\n", msg);
        free(search_str);
        return SRCH_RES_NO_STOCK;
    }
    j++;
    
    // Find if there are cheaper options
    while (j < qdw.lines)
    {
        if (strcmp(search_res->p_code, (qdw.data + j)->p_code) == 0)
        {
            if (min_price->price > (qdw.data + j)->price && (qdw.data + j)->stock)
            {
                min_price = (qdw.data + j);
            }
        }
        j++;
    }
    
    // Print retailer with best price
    printf("\nCheapest offer for %s:\n", search_str);
    printf("\t%12s: %.2f\n", "Price", CNTS_TO_EUR((float)min_price->price));
    printf("\t%12s: %s\n", "Retailer", min_price->p_retailer);
    printf("\t%12s: %d\n", "Stock", min_price->stock);
    putchar('\n');
    
    free(search_str);
    
    return SRCH_RES_POS;
}
