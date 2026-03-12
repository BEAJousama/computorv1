NAME     = computor

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

SRCDIR   = src
OBJDIR   = obj

SRCS     = $(SRCDIR)/main.cpp \
           $(SRCDIR)/Lexer.cpp \
           $(SRCDIR)/Parser.cpp \
           $(SRCDIR)/ParserAPI.cpp \
           $(SRCDIR)/Computor.cpp

OBJS     = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# ── Rules ────────────────────────────────────────────────────────────────────

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "Built $(NAME)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	@echo "Cleaned object files"

fclean: clean
	rm -f $(NAME)
	@echo "Cleaned $(NAME)"

re: fclean all

.PHONY: all clean fclean re
