CONFIG_PARSE_SRCS = config_file_parsing/config_parser.cpp config_file_parsing/HttpContext.cpp config_file_parsing/config_storing.cpp \
					config_file_parsing/config_exception_throw.cpp  config_file_parsing/token_name_checker.cpp config_file_parsing/config_values_extracter.cpp \
					config_file_parsing/ServerContext.cpp config_file_parsing/LocationContext.cpp

CONFIG_PARSE_OBJECTS = $(CONFIG_PARSE_SRCS:.cpp=.o)

MAIN_SRC = helper_functions.cpp

MAIN_OBJECTS = $(MAIN_SRC:.cpp=.o)

FUNS =  SERVER/main.cpp SERVER/Server.cpp request/Request.cpp request/ParseHeader.cpp utils/split.cpp utils/trim.cpp \
		utils/helpers.cpp

OBJS = ${FUNS:.cpp=.o}

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserver

RM = rm -f

all: $(NAME)

%.o : %.cpp includes/main.hpp SERVER/Server.hpp request/Request.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS) $(CONFIG_PARSE_OBJECTS) $(MAIN_OBJECTS)
	$(CC) $(FLAGS) $(OBJS)  $(CONFIG_PARSE_OBJECTS) $(MAIN_OBJECTS) -o $(NAME)

config_file_parsing/%.o :	config_file_parsing/%.cpp config_file_parsing/HttpContext.hpp \
							config_file_parsing/ServerContext.hpp config_file_parsing/LocationContext.hpp \
							config_file_parsing/config_parse.hpp

clean :
	$(RM) $(OBJS)
	rm -rf $(CONFIG_PARSE_OBJECTS)

fclean : clean
	$(RM) $(NAME)

re : fclean all
