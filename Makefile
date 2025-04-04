TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f my_new_database.db -n
	./$(TARGET) -f my_new_database.db -a "Timmy H.,123 SheshireLn.,120"
	./$(TARGET) -f my_new_database.db -a "Timmy Z.,12345 Main St.,134"
	./$(TARGET) -f my_new_database.db -l
	./$(TARGET) -f my_new_database.db -d "Timmy H."
	./$(TARGET) -f my_new_database.db -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude

