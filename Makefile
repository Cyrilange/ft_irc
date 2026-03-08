NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = Src/main.cpp Src/Server.cpp Src/Client.cpp Src/CommandHandler.cpp

OBJ = $(SRC:.cpp=.o)

RESET  = \033[0m
BOLD   = \033[1m
CYAN   = \033[36m
GREEN  = \033[32m

define BANNER
@printf "$(CYAN)$(BOLD)\n"
@printf "  ██╗██████╗  ██████╗███████╗███████╗██████╗ ██╗   ██╗\n"
@printf "  ██║██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██║   ██║\n"
@printf "  ██║██████╔╝██║     ███████╗█████╗  ██████╔╝██║   ██║\n"
@printf "  ██║██╔══██╗██║     ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝\n"
@printf "  ██║██║  ██║╚██████╗███████║███████╗██║  ██║ ╚████╔╝ \n"
@printf "  ╚═╝╚═╝  ╚═╝ ╚═════╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  \n"
@printf "$(RESET)\n"
endef

all: $(NAME)
	$(BANNER)
	@printf "$(GREEN)$(BOLD)  ✓ Build successful — $(NAME) is ready!$(RESET)\n\n"

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all