#include <linux/netfilter_ipv4.h>

/* nf_getsockname() - netfilter SO_ORIGINAL_DST variant of getsockopt()
 *
 * Within the new Linux netfilter framework, NAT functionality is cleanly
 * separated from the TCP/IP core processing. In old days, you could easily
 * retrieve the original destination (IP address and port) of a transparently
 * proxied connection by calling the normal getsockname() syscall.
 * With netfilter, getsockname() returns the real local IP address and port.
 * However, the netfilter code gives all TCP sockets a new socket option,
 * SO_ORIGINAL_DST, for retrieval of the original IP/port combination.
 *
 * This file implements a function nf_getsockname(), with the same calling
 * convention as getsockname() itself; it uses SO_ORIGINAL_DST, and if that
 * fails, falls back to using getsockname() itself.
 *
 * Public domain by Patrick Schaaf <bof@bof.de>
 */

int fw_getsockname(int fd, struct sockaddr *sa,socklen_t* salen)
{
	if (*salen != sizeof(struct sockaddr_in)) {
		errno = EINVAL;
		return -1;
	}
#ifdef SO_ORIGINAL_DST
	if (0 == getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, sa, salen)) {
		return 0;
	}
#endif
	return getsockname(fd, sa, salen);
}
