all: mitmproxy 

CC = gcc
CFLAGS = -g -Wall
INCLUDE = .
OBJECTS =  strlcpy.o pidfile.o server.o utils.o fdpass.o \
	ipcfsm.o main_worker.o proxy_core.o \
	conf/conf.o \
	conf/proxyc.o \
	conf/proxycp.o \
	conf/proxycs.o \
	main.o  


.c.o: thproxy.h -lpthread
mitmproxy:  $(OBJECTS) thproxy.h
	gcc -g -o  $@ $(OBJECTS) -lpthread
clean:
	rm -fr *.o conf/*.o mitmproxy 
