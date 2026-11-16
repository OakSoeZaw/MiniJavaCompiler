CC = gcc
CFLAGS = -Wall -g -I$(SRC) -I$(BUILD)
SRC = src
BUILD = build

TARGET = $(BUILD)/minijava
OBJS = $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o $(BUILD)/ast.o $(BUILD)/semantic.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: $(SRC)/parser.y | $(BUILD)
	bison -t -d -v -o $(BUILD)/parser.tab.c $(SRC)/parser.y
	
$(BUILD)/lex.yy.c: $(BUILD)/parser.tab.h $(SRC)/lexer.l | $(BUILD)
	flex -o $(BUILD)/lex.yy.c $(SRC)/lexer.l	

$(BUILD)/parser.tab.o: $(BUILD)/parser.tab.c $(SRC)/ast.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/lex.yy.o: $(BUILD)/lex.yy.c $(BUILD)/parser.tab.h $(SRC)/ast.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/ast.o: $(SRC)/ast.c $(SRC)/ast.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/semantic.o: $(SRC)/semantic.c $(SRC)/semantic.h $(SRC)/ast.h
	$(CC) $(CFLAGS) -I$(SRC) -I$(BUILD) -c $< -o $@
	
$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

test: $(TARGET)
	mkdir -p logs
	@for d in base extensions errors; do \
		for f in tests/$$d/*.java; do \
			name=$$(basename "$$f" .java); \
			./$(TARGET) < "$$f" > "logs/$$d-$$name.out" 2> "logs/$$d-$$name.err"; \
			echo "$$d/$$name: $$(tail -1 "logs/$$d-$$name.out")"; \
		done; \
	done | tee logs/summary.txt

test-all: $(BUILD)/minijava
	sh run_all_tests.sh

.PHONY: all clean test test-all