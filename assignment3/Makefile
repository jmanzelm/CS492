# Output filename here
NAME = assign3

# CPP source files
SRC = file_system.cpp

OBJ = $(SRC:.cpp=.o)
CXX = g++ $(CXXFLAGS)
CXXFLAGS = -g --std=c++11

all: $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	-$(RM) *~
	-$(RM) \#*
	-$(RM) *.o
	-$(RM) *.core

fclean:	clean
	-$(RM) $(NAME)

re: fclean all

