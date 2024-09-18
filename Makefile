TARGET = bin/jvdb
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f mynewdb.db -n
	./$(TARGET) -f mynewdb.db -a "John Smith,123 45th St,40" -l
	./$(TARGET) -f mynewdb.db -a "Jane Doe,456 78th St,20" -l
	./$(TARGET) -f mynewdb.db -a "Tommy Chen,478 12th St,30" -l
	./$(TARGET) -f mynewdb.db -r "Jane Doe" -l
	./$(TARGET) -f mynewdb.db -u "Tommy Chen,70" -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
