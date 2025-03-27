TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f my_new_database.db -n
	./$(TARGET) -f my_new_database.db -a "Timmy H.,123 SheshireLn.,120"
	./$(TARGET) -f my_new_database.db -a "Timmy I.,12345 Main St.,134"
	./$(TARGET) -f my_new_database.db -l
	./$(TARGET) -f my_new_database.db -u "Timmy H.,Timmy J.,12321 Maine St.,188"
	./$(TARGET) -f my_new_database.db -l
	echo "Should Fail Here"
	./$(TARGET) -f my_new_database.db -u "Timmy U.,Timmy A.,1 Penn St.,40"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude

