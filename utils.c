#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include "utils.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

int jchroot (const char *newroot)
{
  unsigned int ouid;
  unsigned int ogid;

  
  if (dir_unsafe (newroot))
    return UNSAFE_DIR_PERMS;
 
  if (chdir (newroot) < 0)
    {
      return CHROOT_ERR;
    }
  if (chroot (newroot) || chdir ("/"))
    {
      return CHROOT_ERR;
    }
  return 0;
}


int
dir_unsafe (const char *dir)
{

  struct stat mydir;
  if (lstat (dir, &mydir) == -1)
    return -1;
  if (mydir.st_uid != 0)
    return -1;
  if (!S_ISDIR (mydir.st_mode))
    return -1;
  if (mydir.st_mode & (S_IWGRP | S_IWOTH))
    return -1;

  return 0;
}

int
drop_privs (char *name)
{
  struct passwd *pw = getpwnam(name);
  int i, j;

  if (pw)
  {
  i = setgid (pw->pw_gid);
  j = setuid (pw->pw_uid);
  } else {
   fprintf(stderr,"%s: wrong password name %s. Error type: %s\n",__func__,name,strerror(errno));
   return 0;
  }
  return 1;
}

int
daemonize (void)
{
  pid_t pid;
  struct rlimit rl;
  long max_open;
  int fd;
  struct sigaction act[1];
  /* obtain max file descriptors */
  if (getrlimit (RLIMIT_NOFILE, &rl) < 0)
    return 0;
  max_open = rl.rlim_max;
  
  if ((pid = fork ()) == -1)
    return 0;
  /* parent exit */
  if (pid)
    exit (EXIT_SUCCESS);
  setsid ();
  /* ignore signals */
  act->sa_handler = SIG_IGN;
  sigemptyset (&act->sa_mask);
  act->sa_flags = 0;
  if (sigaction (SIGHUP, act, NULL) == -1)
    return 0;
    /* fork again */
  if ((pid = fork ()) == -1)
    return 0;
    /* parent exit again */
  if (pid)
    exit (EXIT_SUCCESS);
    /* change root dir */
  if (chdir ("/") == -1)
    return 0;
  umask (0);
  /* close all file descriptors */
  for (fd = 4; fd < max_open; ++fd)
    {
      close (fd);
    }
  act->sa_handler = SIG_DFL;
  sigemptyset (&act->sa_mask);
  act->sa_flags = 0;
  if (sigaction (SIGHUP, act, NULL) == -1)
    return 0;

  syslog (LOG_NOTICE, "Starting daemon application");
  
  return 1;
}


int
valid_user (char *username)
{
  struct passwd *pw;
  pw = getpwnam(username);
  if (pw != NULL)
	return pw->pw_uid;
  return -1; 
}

int
getgroup (char *username)
{
  struct passwd *pw;
  if ((pw = getpwnam (username)) == NULL)
    return -1;
  return pw->pw_gid;
}



