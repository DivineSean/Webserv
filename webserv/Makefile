NAME		:= webserv

CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98
INCFLAGS	:= -Iinclude

SRCDIR		:= src
SRCS		:= main.cpp \
			   $(SRCDIR)/location.cpp \
			   $(SRCDIR)/server.cpp \
			   $(SRCDIR)/webserv.cpp \
			   $(SRCDIR)/runServer.cpp \
			   $(SRCDIR)/HTTP_Requests.cpp \
			   $(SRCDIR)/MethodHandler.cpp

OBJS		:= $(SRCS:.cpp=.o)
DEPS		:= $(wildcard include/*.hpp)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
