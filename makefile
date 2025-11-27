include config.mk

all: $(PROJ)

$(PROJ): $(C_SRC)
	$(C_GCC) src/$(PROJ).c -o $(PROJ).exe $(C_FLG)

release: $(C_SRC)
	$(C_GCC) src/$(PROJ).c -o $(PROJ).exe $(C_FLG) -O3

clean:
	del *.exe *.o

format:
	clang-format -i $(C_SRC)

.PHONY: all clean format debug
