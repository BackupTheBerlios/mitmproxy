all: mitmproxy
CC = gcc
CFLAGS = -g -Wall -pg 
INCLUDE = .
OBJECTS =  thread.o strlcpy.o pidfile.o socket/msocket.o utils.o fdpass.o \
	ipcfsm.o main_worker.o proxy_core.o \
	conf/conf.o \
	conf/proxyc.o \
	conf/proxycp.o \
	conf/proxycs.o \
	queue/queue.o\
	queue/item.o \
	queue/xmmap.o\
	queue/mcopy.o \
	main.o  


.c.o: thproxy.h -lpthread
mitmproxy:  $(OBJECTS) thproxy.h 
	gcc -g -o  $@ $(OBJECTS) -lpthread
clean:
	rm -fr *.o conf/*.o mitmproxy queue/*.o
