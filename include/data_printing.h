/*
File:         data_printing.h
Author:       Anton Jaska
Created:      2024.12.00
Modified:     2024.04.20
Description:  Header file for data_printing.c. Data struct definitions, macros
              etc.
*/

#ifndef _DATA_PRINTING
#define _DATA_PRINTING

#define SEP_LINE_LEN 80

/*
Description:    Prints all the supported options of the program into standard
                output stream with corresponding menu option values.
                
Parameters:     -
                
Return:         -
*/
void print_menu(void);


/*
Description:    Prints products name, all the specs and product code. Everything
                is given a descriptive label and printed in a separate line. The
                product name is printed first, everything else that follows is
                indented by '\t'.
                
Parameters:     pi - Struct holding all the data necessary for printing.
                
Return:         -
*/
void print_product_specs(struct product_info pi);


/*
Description:    Prints quote info as one line with a "vertical a line ('|')"
                separating each field. Data is printed in a way that consecutive
                calls of this function have the pipe  symbols '|' aligned. If
                the retailer has the product in stock, "In Stock" with amount of
                stock is displayed. Otherwise "Order" is displayed.
                
Parameters:     qi - Struct holding all the data necessary for printing.
                
Return:         -
*/
void print_product_quote(struct quote_info qi);


/*
Description:    Prints the head of product quote table, with descriptive labels
                for every column. Labels are separated with the pipe symbol
                '|'. These symbols align with the ones from print_product_quote
                function.
                
Parameters:     -
                
Return:         -
*/
void print_quote_table_head(void);


/*
Description:    Prints a separator line of dashes '-' with length SEP_LINE_LEN.
                
Parameters:     -
                
Return:         -
*/
void print_separator_line(void);


/*
Description:    Prints product info from struct pi into file pointed by *fp.
                Data is separated by CSV_DELIMITER and in the following order:
                Product Code, Name, RAM, Screen Size, OS.
                
Parameters:     pi - Struct holding all the data necessary for printing.
                
Return:         -
*/
void print_product_csv_line(FILE *fp, struct product_info pi);


/*
Description:    Prints quote info from struct qi into file pointed by *fp.
                Data is separated by CSV_DELIMITER and in the following order:
                Quote ID, Product Code, Retailer, Price, Stock.
                
Parameters:     qi - Struct holding all the data necessary for printing.
                
Return:         -
*/
void print_quote_csv_line(FILE *fp, struct quote_info qi);

#endif
