#include <stdlib.h>
#include "../include/log_handler.h"
#include "../include/arg_parse.h"

#define MIN_ARGS_TO_PARSE 1

int main(int argc, char **argv)
{
    // Default logging level: INFO & file name: "log.txt" 
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
    
    /*#ifdef DEBUG
    printf("Args cnt: %d\n", argc);
    printf("Default args:\n");
    printf("\tprint_file = %s\n", arguments.print_file ? "true" : "false");
    printf("\tprint_screen = %s\n", arguments.print_screen ? "true" : "false");
    printf("\tfile_out = %s\n", arguments.file_out);
    printf("\tfile_mes = %s\n", arguments.file_mes);
    printf("\tfile_reg = %s\n", arguments.file_reg);
    #endif*/
    
    if (argc > MIN_ARGS_TO_PARSE)
    {
        parse_arguments(options, &arguments, argc, argv);
        write_log(INFO, "Parsed command line arguments successfully.");
    }
    else
    {
        write_log(INFO, "Using default arguments.");
    }
    
    write_log(INFO, "Closing program successfully");
    return EXIT_SUCCESS;
}
