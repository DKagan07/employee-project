TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f my_new_database.db -n
	./$(TARGET) -f my_new_database.db -a "Timmy H.,123 SheshireLn.,120"
	./$(TARGET) -f my_new_database.db -a "Timmy I.,12345 Main St.,134"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude



# ./$(TARGET) -f ./mynewdb.db -n 
# ./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"
