#ifndef _PID_FILE_H
#define _PID_FILE_H
#define DEFAULT_PID "/tmp/mypid.pid"
int pid_open(const char *fname, uid_t puid, gid_t pgid);
#endif
