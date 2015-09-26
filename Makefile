CC	= gcc
CFLAGS	= -c -fPIC -O3 -Wall
LDFLAGS	= -shared
TARGET	= liberr.so
SRCS	= err_handler.c
HEADERS = err_handler.h
OBJS	= $(SRCS: .c=.o)

.PHONY: ALL clean install uninstall
ALL: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
%.o: %.c %.h
	$(CC) $(CFLAGS) $^

clean:
	$(RM) $(TARGET) $(OBJS) $(wildcard *.gch)

install:
	cp $(TARGET) /usr/lib/
	cp $(HEADERS) /usr/include

uninstall:
	$(RM) /usr/lib/$(TARGET)
	$(RM) /usr/include/$(HEADERS)
