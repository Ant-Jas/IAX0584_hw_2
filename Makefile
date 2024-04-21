NAME := price_watch.out
TEST_MACRO :=NO_MACRO

SRC_DIR	:= src
OBJ_DIR := obj
SRCS := 			\
	main.c			\
	log_handler.c		\
	arg_parse.c		\
	csv_helper.c		\
	data_printing.c		\
	data_read_write.c
SRCS := $(SRCS:%=$(SRC_DIR)/%)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC := gcc
CFLAGS := -Wall -Wextra -Wconversion -g -I include/

RM := rm -f
MAKEFLAGS += --no-print-directory
DIR_DUP = mkdir -p $(@D)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)
	$(info CREATED $(NAME))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(DIR_DUP)
	$(CC) $(CFLAGS) -c -o $@ $<
	$(info CREATED $@)

testing: fclean \
	$(OBJS)
	$(CC) $(OBJS) -o $(NAME) -D$(TEST_MACRO)
	$(info CREATED $(NAME))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(DIR_DUP)
	$(CC) $(CFLAGS) -D$(TEST_MACRO) -c -o $@ $<
	$(info CREATED $@)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

.PHONY: clean fclean
