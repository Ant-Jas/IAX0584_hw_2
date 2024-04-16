#include <data_printing.h>

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
