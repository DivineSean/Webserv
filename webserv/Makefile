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

GREEN		:= $(shell tput -Txterm setaf 2 2>/dev/null)
CYAN		:= $(shell tput -Txterm setaf 6 2>/dev/null)
RESET		:= $(shell tput -Txterm sgr0 2>/dev/null)

vpath %.cpp . $(SRCDIR)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)webserv is ready$(RESET)"

$(OBJDIR)/%.o: %.cpp $(DEPS) | $(OBJDIR)
	@$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@
	@echo "$(CYAN)compiled$(RESET) $(notdir $<)"

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)
	@echo "object files cleaned"

fclean: clean
	@rm -f $(NAME)
	@echo "webserv cleaned"

re: fclean all

.PHONY: all clean fclean re
