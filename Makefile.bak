CC	:= gcc
LD	:= ld
CFLAGS	:= -c -fPIC -O3 -Wall
LDFLAGS	:= -shared
OBJS	:= err_handler.o
HEADERS := err_handler.h
PROGS	:= test pression_test sigtstp_test mt_test
TARGET	:= liberr.so

# BE CARE FOR USE THIS. NO ANY SPACE IN THE BRACKETS.
# SRCS	= err_handler.c
# OBJS	= $(SRCS:.c=.o)

ifeq "$(M32)" "yes"
CFLAGS	+= -m32
LDFLAGS	+= -melf_i386
LIBS_32	:= -L/usr/lib/gcc/x86_64-linux-gnu/5.1.1/32/ -L/usr/lib
endif

.PHONY: ALL tests clean install uninstall
ALL: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LIBS_32) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $^

tests: $(PROGS)

%: %.c
	$(CC) -o $@ $^ -Wall -lerr -lpthread

clean:
	$(RM) $(TARGET) $(OBJS) $(wildcard *.h.gch) $(PROGS)

install:
	cp $(TARGET) /usr/lib/
	cp $(HEADERS) /usr/include

uninstall:
	$(RM) /usr/lib/$(TARGET)
	$(RM) /usr/include/$(HEADERS)
