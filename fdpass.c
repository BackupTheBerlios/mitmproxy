#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "fdpass.h"

void
send_fd(int sock, int fd)
{

	struct msghdr msg;
	struct iovec vec;
	char ch = '\0';
	ssize_t n;

	char tmp[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;


	memset(&msg, 0, sizeof(msg));



	msg.msg_control = (caddr_t)tmp;
	msg.msg_controllen = CMSG_LEN(sizeof(int));
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int *)CMSG_DATA(cmsg) = fd;

	vec.iov_base = &ch;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	if ((n = sendmsg(sock, &msg, 0)) == -1)	
	{
	      
		fprintf(stderr,"%s: sendmsg(%d): %s", __func__, fd,
		    strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (n != 1){
		fprintf(stderr,"%s: sendmsg: expected sent 1 got %ld",
		    __func__, (long)n);
		exit(EXIT_FAILURE);
	}

}

int
receive_fd(int sock)
{

	struct msghdr msg;
	struct iovec vec;
	ssize_t n;
	char ch;
	int fd;

	char tmp[CMSG_SPACE(sizeof(int))];
	memset(tmp,0,sizeof(tmp));
	struct cmsghdr *cmsg;


	memset(&msg, 0, sizeof(msg));
	vec.iov_base = &ch;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	msg.msg_control = tmp;
	msg.msg_controllen = sizeof(tmp);


	if ((n = recvmsg(sock, &msg, 0)) == -1)
	{
		fprintf(stderr,"%s: recvmsg: %s", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (n != 1)
	{
		fprintf(stderr,"%s: recvmsg: expected received 1 got %ld",
		    __func__, (long)n);
		exit(EXIT_FAILURE);
	}


	cmsg = CMSG_FIRSTHDR(&msg);
	if (cmsg == NULL)
	{
		fprintf(stderr,"%s: no message header", __func__);
		exit(EXIT_FAILURE);
	}

	if (cmsg->cmsg_type != SCM_RIGHTS) {
		fprintf(stderr,"%s: expected type %d got %d", __func__,
		    SCM_RIGHTS, cmsg->cmsg_type);
		exit(EXIT_FAILURE);
	}

	fd = (*(int *)CMSG_DATA(cmsg));

	return fd;

}
