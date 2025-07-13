NAME = Matt_daemon

# Folders
SOURCE_FOLDER = src/
HEADERS_FOLDER = inc/

# Source files
SOURCE_FILES = main.cpp \
               server.cpp \
			   utils.cpp \
               tintin.cpp

SOURCES = $(addprefix $(SOURCE_FOLDER), $(SOURCE_FILES))

# Object files
OBJECTS = $(SOURCE_FILES:.cpp=.o)

# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -I$(HEADERS_FOLDER)

# Default rule
all: $(NAME)

# Linking
$(NAME): $(OBJECTS)
	@echo "🚧 Linking binary... 🚧"
	@$(CXX) $(OBJECTS) -o $(NAME)
	@echo "✅ Build complete!"

# Compiling object files
$(OBJECTS): 
	@echo "🚧 Compiling source files... 🚧"
	@$(CXX) -c $(SOURCES) $(CXXFLAGS)
	@echo "✅ Compilation done!"

# Clean object files
clean:
	@echo "🧹 Removing object files..."
	@rm -f $(OBJECTS)
	@rm -f leaks.$(NAME).txt
	@echo "✅ Done!"

# Clean binary and objects
fclean: clean
	@echo "🧹 Removing binary..."
	@rm -f $(NAME)
	@echo "✅ Done!"

# Rebuild everything
re: fclean all

# Linter rule
normi:
	@cpplint --root=inc $(SOURCES) $(HEADERS_FOLDER)*

# Test execution + memory check
test: re
	@echo "🧹 Limpiando log anterior de matt_daemon..."
	@rm -f /var/log/matt_daemon/matt_daemon.log
	@echo "🧪 Ejecutando script de test..."
	@./test_matt_daemon.sh
	@echo "\n🛡️ Valgrind execution 🛡️"
	@ valgrind --leak-check=full --show-leak-kinds=all --verbose \
	--log-file=leaks.$(NAME).txt ./$(NAME)
	@echo "\n📚 Valgrind summary 📚"
	@cat leaks.$(NAME).txt | grep 'ERROR SUMMARY'



.PHONY: all clean fclean re normi test
