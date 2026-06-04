#### SETUP ####
NAME = libasm.a
TEST = libasm_tests
BONUS_TEST = libasm_tests_bonus
CC = gcc
ASM = nasm
ASMFLAGS = -f elf64
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

#### SOURCES ####
SRCDIR = src
BONUSDIR = src

#### FILES ####
SRCS = $(SRCDIR)/ft_strlen.s \
	$(SRCDIR)/ft_strcpy.s \
	$(SRCDIR)/ft_strcmp.s \
	$(SRCDIR)/ft_write.s \
	$(SRCDIR)/ft_read.s \
	$(SRCDIR)/ft_strdup.s
OBJS = $(SRCS:.s=.o)

BONUS_SRC = $(BONUSDIR)/ft_atoi_base_bonus.s
BONUS_OBJ = $(BONUS_SRC:.s=.o)

#### RULES ####
all: $(NAME) $(TEST)

$(NAME): $(OBJS)
	$(RM) $@
	ar rcs $@ $^

$(TEST): main.c $(NAME)
	$(CC) $(CFLAGS) -o $@ main.c $(NAME)
	@echo "Main archive and main test executable built"

bonus: $(OBJS) $(BONUS_OBJ)
	$(RM) $(NAME)
	ar rcs $(NAME) $(OBJS) $(BONUS_OBJ)
	$(CC) $(CFLAGS) -o $(BONUS_TEST) main_bonus.c $(NAME)
	@echo "Bonus archive and bonus test executable built"

$(BONUS_TEST): main_bonus.c $(NAME)
	$(CC) $(CFLAGS) -o $@ main_bonus.c $(NAME)

$(BONUS_OBJ): $(BONUS_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	$(RM) $(OBJS) $(BONUS_OBJ)

fclean: clean
	$(RM) $(NAME) $(TEST) $(BONUS_TEST)

re: fclean all

.PHONY: all clean fclean re bonus
