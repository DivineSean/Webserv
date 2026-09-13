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

# ---- Build (run natively, or inside the container) ------------------------

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

# ---- Docker dev environment (run on the host) -----------------------------

up:
	docker compose up -d --build

bash: up
	docker compose exec webserv bash

down:
	docker compose down

logs:
	docker compose logs -f

.PHONY: all clean fclean re up bash down logs
