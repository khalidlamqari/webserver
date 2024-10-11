FUNS =  SERVER/main.cpp SERVER/Server.cpp request/Request.cpp request/ParseHeader.cpp utils/split.cpp utils/trim.cpp

OBJS = ${FUNS:.cpp=.o}

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserver

RM = rm -f

%.o : %.cpp includes/main.hpp SERVER/Server.hpp request/Request.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re : fclean all
