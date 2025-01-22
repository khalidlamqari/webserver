# Colors
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
CYAN = \033[36m
RED = \033[31m
RESET = \033[0m
BOLD = \033[1m

#-------------------------------------------------------------------------------------------------------------------------------#

SERVER_SRCS			=	server_setup/server.cpp \
						server_setup/KqueueIdent.cpp \
						server_setup/SocketManager.cpp  \
						server_setup/ClientHandler.cpp \
						server_setup/KqueueEventQueue.cpp \
						server_setup/Socket.cpp

UTILS_SRCS			=	response/DefaultInfo.cpp Utils/helper_functions.cpp


CONFIG_PARSE_SRCS	=	config_file_parsing/token_name_checker.cpp \
						config_file_parsing/ConfigTokenizer.cpp \
						config_file_parsing/ConfigException.cpp \
						config_file_parsing/ConfigParser.cpp \
						config_file_parsing/ConfigValueExtractor.cpp

CONTEXTS_SRCS		=	Contexts/HttpContext.cpp \
						Contexts/LocationContext.cpp \
						Contexts/ServerContext.cpp

MAIN_SRCS			=	main.cpp

REQUEST_SRCS		=	Request/Request.cpp \
						Request/start_line_parser.cpp \
						Request/headers_parser.cpp \
						Request/body_parser.cpp

RESPONSE_SRCS		=	response/Response.cpp \
						response/StaticPage.cpp \
					 	response/response_tools.cpp

#-------------------------------------------------------------------------------------------------------------------------------#

RESPONSE_OBJS = ${RESPONSE_SRCS:.cpp=.o}

CONFIG_PARSE_OBJECTS = $(CONFIG_PARSE_SRCS:.cpp=.o)

UTILS_OBJECTS = $(UTILS_SRCS:.cpp=.o)

CONTEXTS_OBJECTS = $(CONTEXTS_SRCS:.cpp=.o)

SERVER_OBJECTS = $(SERVER_SRCS:.cpp=.o)

MAIN_OBJECTS = $(MAIN_SRCS:.cpp=.o)

REQUEST_OBJECTS = $(REQUEST_SRCS:.cpp=.o)

#-------------------------------------------------------------------------------------------------------------------------------#

CPP = c++

FLAGS =  -Wall -Wextra -Werror -std=c++98 -D LOG_ENABLED=1

NAME = webserv

#-------------------------------------------------------------------------------------------------------------------------------#

all : $(NAME)
	@echo "$(GREEN)🎉 Build completed successfully! webserver is ready to use.$(RESET)"

$(NAME) : $(CONFIG_PARSE_OBJECTS) $(UTILS_OBJECTS) $(CONTEXTS_OBJECTS) $(SERVER_OBJECTS) $(MAIN_OBJECTS) $(REQUEST_OBJECTS) $(RESPONSE_OBJS)
	@echo "$(BLUE)🔨 Creating the server executable...$(RESET)"
	@$(CPP) $(FLAGS) $(CONFIG_PARSE_OBJECTS) $(UTILS_OBJECTS) $(CONTEXTS_OBJECTS) $(SERVER_OBJECTS) $(MAIN_OBJECTS) $(REQUEST_OBJECTS) $(RESPONSE_OBJS) -o $(NAME)
	@echo "$(CYAN)🚀 Server setup is complete!$(RESET)"

Utils/%.o : Utils/%.cpp Utils/utils.hpp
	@echo "$(YELLOW)🛠️  Compiling utility files: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

config_file_parsing/%.o : config_file_parsing/%.cpp config_file_parsing/HttpConfigParser.hpp config_file_parsing/config_structs.hpp
	@echo "$(YELLOW)🛠️  Compiling config parser: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

Contexts/%.o : Contexts/%.cpp Contexts/HttpContext.hpp Contexts/LocationContext.hpp Contexts/ServerContext.hpp
	@echo "$(YELLOW)🛠️  Compiling context files: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

server_setup/%.o : server_setup/%.cpp server_setup/KqueueEventQueue.hpp server_setup/KqueueIdent.hpp server_setup/Server.hpp server_setup/SocketManager.hpp
	@echo "$(YELLOW)🛠️  Compiling server setup files: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

Request/%.o : Request/%.cpp Request/Request.hpp Request/request_parse.hpp
	@echo "$(YELLOW)🛠️  Compiling request handler files: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

response/%.o : response/%.cpp response/DefaultInfo.hpp response/Response.hpp response/includes/macros.hpp response/includes/main.hpp
	@echo "$(YELLOW)🛠️  Compiling response handler files: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

%.o : %.cpp webserv.hpp
	@echo "$(YELLOW)🛠️  Compiling main web server file: $<$(RESET)"
	@$(CPP) $(FLAGS) -c $< -o $@

#-------------------------------------------------------------------------------------------------------------------------------#

clean :
	@echo "$(RED)🧹 Cleaning up object files...$(RESET)"
	@rm -rf $(CONTEXTS_OBJECTS)
	@rm -rf $(CONFIG_PARSE_OBJECTS)
	@rm -rf $(UTILS_OBJECTS)
	@rm -rf $(SERVER_OBJECTS)
	@rm -rf $(MAIN_OBJECTS)
	@rm -rf $(REQUEST_OBJECTS)
	@rm -rf $(RESPONSE_OBJS)
	@echo "$(GREEN)✅ Object files removed.$(RESET)"

fclean : clean
	@rm -rf $(NAME)
	@echo "$(GREEN)✅ All build objects and executable removed.$(RESET)"

re : fclean all

.PHONY : clean
