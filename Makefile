CC	:= gcc
CFLAGS	:= -c -fPIC -O3 -Wall
LDFLAGS	:= -shared
TARGET	:= liberr.so
OBJS	:= err_handler.o
HEADERS := err_handler.h
PROGS	:= test pression_test sigtstp_test

# BE CARE FOR USE THIS. NO ANY SPACE IN THE BRACKETS.
# SRCS	= err_handler.c
# OBJS	= $(SRCS:.c=.o)

.PHONY: ALL clean install uninstall
ALL: $(TARGET) $(PROGS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) $^

%: %.c
	$(CC) -o $@ $^ -Wall -lerr

clean:
	$(RM) $(TARGET) $(OBJS) $(wildcard *.h.gch) $(PROGS)

install:
	cp $(TARGET) /usr/lib/
	cp $(HEADERS) /usr/include

uninstall:
	$(RM) /usr/lib/$(TARGET)
	$(RM) /usr/include/$(HEADERS)
