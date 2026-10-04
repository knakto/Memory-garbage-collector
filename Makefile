NAME	:= mem_gbc
CC		:= cc
CFLAGS	:= -Wall -Wextra -Werror
INC		:= -Iinc

TEST_BIN	:= test_runner
TEST_SRCS	:= test/test_main.c

VALGRIND	:= valgrind
VFLAGS	:= --leak-check=full --error-exitcode=1

all: $(TEST_BIN)

$(TEST_BIN) test:
	$(CC) $(CFLAGS) $(TEST_SRCS) $(INC) -o $@
	./$(TEST_BIN)

mem-test:
	$(CC) $(CFLAGS) $(TEST_SRCS) $(INC) -o $(TEST_BIN)
	$(VALGRIND) $(VFLAGS) ./$(TEST_BIN)

fclean:
	rm -rf $(TEST_BIN)

re: fclean all

.PHONY: all fclean re test mem-test
