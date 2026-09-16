NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCS	= main.c \
		  parser.c \
		  coder.c \
		  coder_utils.c \
		  dongle.c \
		  init_utils.c \
		  log.c \
		  monitor.c \
		  scheduler.c \
		  scheduler_utils.c \
		  utils.c \

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
