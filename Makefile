CFLAGS=-fno-stack-protector -g
LDFLAGS=-z execstack
CC=gcc

.PHONY: all
all: cgi-bin/dump.cgi cgi-bin/view.cgi cgi-bin/post.cgi \
	exploit-exec-ps.o exploit-peek-passwd.o

cgi-bin/%.cgi: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<