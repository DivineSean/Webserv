NAME		:= webserv

CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98
INCFLAGS	:= -Iinclude

SRCDIR		:= src
OBJDIR		:= obj

SRCS		:= main.cpp \
			   $(SRCDIR)/location.cpp \
			   $(SRCDIR)/server.cpp \
			   $(SRCDIR)/webserv.cpp \
			   $(SRCDIR)/runServer.cpp \
			   $(SRCDIR)/HTTP_Requests.cpp \
			   $(SRCDIR)/MethodHandler.cpp

OBJS		:= $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.cpp=.o)))
DEPS		:= $(wildcard include/*.hpp)

vpath %.cpp . $(SRCDIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp $(DEPS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
