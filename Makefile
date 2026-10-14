CC = gcc
CFLAGS = -Wall -g
SRC = src
BUILD = build

TARGET = $(BUILD)/minijava

all: $(TARGET)

$(TARGET): $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c
	$(CC) $(CFLAGS) $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c -o $(TARGET)

$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: $(SRC)/parser.y | $(BUILD)
	bison -t -d -v -o $(BUILD)/parser.tab.c $(SRC)/parser.y
	
$(BUILD)/lex.yy.c: $(BUILD)/parser.tab.h $(SRC)/lexer.l | $(BUILD)
	flex -o $(BUILD)/lex.yy.c $(SRC)/lexer.l	
	
$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: all clean