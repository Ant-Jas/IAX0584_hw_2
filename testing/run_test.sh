#!/bin/bash

# Uncomment &> /dev/null to see tests output

# Make sure the following files in test_data/ have the following permissions:
#	no_rw_products.csv	---
#	no_rw_quotes.csv	---
#	no_w_products.csv	r--
#	no_w_quotes.csv		r--
#	all others			rw-

# Make sure the following files in ./ have the following permissions
#	rm_last_endline.out	rwx

TEST_NUM=0

print_success()
{
	RETURN_VAL=$1
	TEST_NAME=$2
	
	TEST_NUM=$((TEST_NUM+1))
	
	echo -n "Test $TEST_NUM: "
	if [ $RETURN_VAL == $VALGRIND_ERR_CODE ]; then
		echo -n "Fail"
	else
		echo -n "Pass"
	fi
	echo " $2"
}

BIN_DIR="../"
BIN_NAME="price_watch.out"
TEST_FILE_DIR="test_data/"

VALGRIND_ERR_CODE=66

# Tests 1-10 use regular build
cd $BIN_DIR
make fclean all &> /dev/null
cd testing/


# Test 1 - Unknown CLI argument
FILE_USER_INPUT="$TEST_FILE_DIR""print_all_data_user_input"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--random_unknwown_arg \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Unknwon CLI argument)"


# Test 2 - No RW permissions for products file
FILE_PRO="$TEST_FILE_DIR""no_rw_products.csv"
FILE_QTE="$TEST_FILE_DIR""quotes.csv"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
&> /dev/null
print_success $? "(No RW permissions for products file)"


# Test 3 - No RW permissions for quotes file
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""no_rw_quotes.csv"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
&> /dev/null
print_success $? "(No RW permissions for quotes file)"


# Test 4 - No W permissions for products file
FILE_PRO="$TEST_FILE_DIR""no_w_products.csv"
FILE_QTE="$TEST_FILE_DIR""quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""change_product_ram"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(No W permissions for products file)"


# Test 5 - No W permissions for quotes file
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""no_w_quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""change_retailer_name"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(No W permissions for quotes file)"


# Test 6 - Invalid data values
FILE_PRO="$TEST_FILE_DIR""invalid_data_products.csv"
FILE_QTE="$TEST_FILE_DIR""invalid_data_quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""print_all_data_user_input"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Invalid data values)"


# Test 7 - Invalid search values / no search result
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""invalid_search_values"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Invalid search values / no search result)"


# Test 8 - No '\n' at end of data files
FILE_PRO="$TEST_FILE_DIR""rm_endl_products.csv"
FILE_QTE="$TEST_FILE_DIR""rm_endl_quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""print_all_data_user_input"

./rm_last_endline.out $FILE_PRO &> /dev/null
./rm_last_endline.out $FILE_QTE &> /dev/null

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Missing '\n' at end of file)"


# Test 9 - missing product data
FILE_PRO="$TEST_FILE_DIR""missing_data_products.csv"
FILE_QTE="$TEST_FILE_DIR""quotes.csv"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Missing product data)"


# Test 10 - missing quotes data
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""missing_data_quotes.csv"

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Missing quote data)"


# Test 11 - Dynamic string function realloc test
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""quotes.csv"
FILE_USER_INPUT="$TEST_FILE_DIR""dyn_str_test_input"

cd $BIN_DIR
make testing TEST_MACRO=FUNC_GET_DYNAMIC_INPUT_STRING_TEST &> /dev/null
cd testing/

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
< $FILE_USER_INPUT &> /dev/null
print_success $? "(Dynamic string function realloc error)"


# Test 12 - Product file realloc error
FILE_PRO="$TEST_FILE_DIR""more_products.csv"

cd $BIN_DIR
make testing TEST_MACRO=FUNC_READ_DATA_PRODUCTS_TEST &> /dev/null
cd testing/

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
&> /dev/null
print_success $? "(Product file realloc error)"


# Test 13 - Quote file realloc error
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""more_quotes.csv"

cd $BIN_DIR
make testing TEST_MACRO=FUNC_READ_DATA_QUOTES_TEST &> /dev/null
cd testing/

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
&> /dev/null
print_success $? "(Quote file realloc error)"


# Test 14 - Read line realloc error
FILE_PRO="$TEST_FILE_DIR""products.csv"
FILE_QTE="$TEST_FILE_DIR""extra_long_line_quotes.csv"

cd $BIN_DIR
make testing TEST_MACRO=FUNC_READ_LINE_TEST &> /dev/null
cd testing/

valgrind --error-exitcode=$VALGRIND_ERR_CODE ./"$BIN_DIR""$BIN_NAME" \
--file_products $FILE_PRO --file_quotes $FILE_QTE \
&> /dev/null
print_success $? "(Read line realloc error)"

# Restore regular build
cd $BIN_DIR
make fclean all &> /dev/null
cd testing/
