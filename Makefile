NAME?=spline_editor

CC:=g++
CFLAGS+=
LDLIBS+=-lsfml-graphics -lsfml-window -lsfml-system

SRC:=src/$(NAME).cpp

.PHONY: clean

program: $(SRC)
	$(CC) $^ $(CFLAGS) $(LDLIBS) -o $(NAME)

clean:
	-rm -f agario agario noise_flat noise_round spline_editor windy_balls