SRCS = classes.cpp main.cpp tools.cpp
OBJS = *.o
INCLUDE = *.hpp
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98  -fsanitize=address 
NAME = webserv

all: ${NAME}

${NAME}: ${SRCS} ${INCLUDE}
	rm -rf .vscode
	${CC} ${CFLAGS} -c ${SRCS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}
clean:
	rm -f ${OBJS}
fclean: clean
	rm -f ${NAME}
re : fclean all


