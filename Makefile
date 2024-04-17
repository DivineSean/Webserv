NAME = webserv
SRC = main.cpp location.cpp server.cpp webserv.cpp runServer.cpp HTTP_Requests.cpp
OBJS = $(SRC:.cpp=.o)
CFLAGS = -Wall -Wextra -Werror -std=c++98
INC = location.hpp server.hpp webserv.hpp client.hpp request.hpp response.hpp
R        := $(shell tput -Txterm setaf 1)
G        := $(shell tput -Txterm setaf 2)
Y       := $(shell tput -Txterm setaf 3)

all: $(NAME)
	@echo $(G) "ALL functions are done!"

$(NAME): $(OBJS)
	@echo $(Y) Compiling: $< ... Done!
	@c++ $(CFLAGS) $(OBJS) -o $@

%.o: %.cpp $(INC)
	@echo $(Y) Compiling: $< ... Done!
	@c++ $(CFLAGS) -c $< -o $(<:.cpp=.o)

clean:
	@echo $(R) Cleaned
	@rm -f $(OBJS)

fclean: clean
	@echo $(R) Fully cleaned
	@rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re all