#ifndef _PID_FILE_H
#define _PID_FILE_H
#define DEFAULT_PID "/var/run/mitmproxy.pid"
int pid_open(const char *fname, uid_t puid, gid_t pgid);
#endif
