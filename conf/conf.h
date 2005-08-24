#ifndef _CONF_H
#define _CONF_H
#define CONF_FILE "/tmp/proxy.conf"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct proxybind
{
  struct in_addr ipaddress;
  uint16_t port;
  struct proxybind *next;
  struct proxybind *prev;
};
unsigned long total;
struct proxybind *head;
struct proxybind *allocProxy(void);
struct proxybind *freeProxy(struct proxybind *todelete,struct proxybind *head);
void freeAll(struct proxybind **head);
struct proxybind *prev(struct proxybind  *list);
struct proxybind *next(struct proxybind *list);
struct proxybind *insertProxy(struct proxybind *head, struct proxybind *item);
#define FOREACH_BIND(var, head)                \
        for((var) = head;                   \
            (var)!= NULL;                   \
            (var) = next(var))
#endif /* CONF.H */
