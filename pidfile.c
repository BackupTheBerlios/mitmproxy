#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>

static int unlock_pid(int fd);
static int lock_pid(int fd);

int pid_open(const char *fname, uid_t puid, gid_t pgid)
{
char buf[20];
struct stat l_stat, f_stat;
int fd;


snprintf(buf,sizeof(buf),"%ld",(long)getpid());

if (lstat(fname,&l_stat) == -1)
{
if (errno != ENOENT)
    return -1;

if ((fd = open(fname,O_CREAT|O_EXCL|O_WRONLY,0600)) == -1)
{
syslog(LOG_INFO,"Opening pidfile failed\n");
    return -1;
}
} 
else return -1;

lstat(fname,&l_stat);

if (fstat(fd,&f_stat) == -1 ||
    l_stat.st_mode != f_stat.st_mode ||
    l_stat.st_ino  != f_stat.st_ino  ||
    l_stat.st_dev  != f_stat.st_dev )
    {
    syslog(LOG_INFO,"Possible race condition on file %s\n",fname);
    close(fd);
    return -1;
    }

if ((fchown(fd,puid,pgid))==-1)
    {
    return -1;
    }
if (lock_pid(fd) <0)
    {
    return -1;
    }
if (write(fd,buf,strlen(buf)) != strlen(buf))
{
unlock_pid(fd);
close(fd);
unlink(fname);
return -1;
}
unlock_pid(fd);
close(fd);
return fd;
}


int lock_pid(int fd)
{
struct flock lock;
/*if (fcntl(fd,F_GETLK,&lock)<0)
    return -1;
if (lock.l_type != F_UNLCK)
    return -1; */
/* I'm aware that the two fnctl are not atomic operations*/
lock.l_type  = F_WRLCK;
lock.l_start = 0 ;
lock.l_whence = SEEK_SET;
lock.l_len = 0;  
return ( fcntl(fd,F_SETLK,&lock) );
}

int unlock_pid(int fd)
{
struct flock lock;
lock.l_type  = F_UNLCK;
lock.l_start = 0 ;
lock.l_whence = SEEK_SET;
lock.l_len = 0;  
return ( fcntl(fd,F_SETLK,&lock) );
}
