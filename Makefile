NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = Src/main.cpp Src/Server.cpp Src/Client.cpp Src/CommandHandler.cpp Src/Channel.cpp

OBJ = $(SRC:.cpp=.o)

RESET  = \033[0m
BOLD   = \033[1m
CYAN   = \033[36m
GREEN  = \033[32m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@clear
	@echo "$(CYAN)$(BOLD)"
	@echo "  ██╗██████╗  ██████╗███████╗███████╗██████╗ ██╗   ██╗"
	@echo "  ██║██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██║   ██║"
	@echo "  ██║██████╔╝██║     ███████╗█████╗  ██████╔╝██║   ██║"
	@echo "  ██║██╔══██╗██║     ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝"
	@echo "  ██║██║  ██║╚██████╗███████║███████╗██║  ██║ ╚████╔╝ "
	@echo "  ╚═╝╚═╝  ╚═╝ ╚═════╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝ "
	@echo "$(RESET)"
	@echo "$(GREEN)$(BOLD)  ✓ Build successful — $(NAME) is ready!$(RESET)"
	@echo " "

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
  