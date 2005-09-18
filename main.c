/*
 Copyright (c) 2005 Giorgio Zoppi <zoppi@cli.di.unipi.it>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/* System libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include "conf/conf.h"
#include "conf/proxyc.h"
#include <fcntl.h>
#include "utils.h"
#include "pidfile.h"
#include "socket/msocket.h"
#include "strlcpy.h"
#include "proxy_core.h"
#include "queue/queue.h"
#ifndef PATH_MAX

#include <limits.h>
#endif
typedef struct my_opt
{
  int do_chroot;
  int switchuser;
  int switchgroup;
  int userconf;
}
line_opt;



void usage (void);
void make_chroot (const char *jailpath);
void bzeropt (line_opt * flags);
void check_status (int status);
void sig_hup_handler(int signum);
void sig_term_handler(int signum);

volatile unsigned int reload_conf;
volatile unsigned int int_recv;

int
main (int argc, char **argv)
{

/* options handling vars */
  extern char *optarg;
  extern int optind;
  extern struct proxybind *head;
  extern unsigned long total;
  

  struct proxybind *tmpbind;

  char opt;
  char configfile[PATH_MAX];
  char chrootdir[PATH_MAX];
  char uid_name[20];
  
  
  line_opt bl_flags;
  
  uid_t myuid = 0, mygid = 0;
  


  /* Initializations */
  memset (configfile, '\0', sizeof (configfile) -1);
  memset (chrootdir,'\0', sizeof (chrootdir) -1);
  bzeropt (&bl_flags);
  reload_conf = 0;
  int_recv = 0;
  
  /* Setting handlers*/
  signal(SIGTERM,sig_term_handler);
  signal(SIGINT,sig_term_handler);
  signal(SIGHUP,sig_hup_handler);

  if (argc == 1)
    usage ();
  /* JCL */
  while ((opt = getopt (argc, argv, ":c:t:u:")) != -1)
    {
      switch (opt)
	{

	case 'c':		/* config file */
	  {

	    if (optarg)
	      {
		strlcpy (configfile, optarg, sizeof (configfile));
		bl_flags.userconf = 1;
	      }

	    break;
	  }
	case 't':		/* chrootdir */
	  {
	    if (optarg) {
		strlcpy (chrootdir, optarg, sizeof (chrootdir));
		bl_flags.do_chroot = 1;
	    }
	    break;
	  }
	 case 'u':		/* userid to switch */
	  {
	    strlcpy (uid_name, optarg, sizeof (uid_name));
	    bl_flags.switchuser = 1;
	    break;
	  }
	default:
	  usage ();
	  /* NOTREACHED */
	}
    }

  /* Checking if -u and  are present */
  if (bl_flags.switchuser == 0)
    usage ();
/*
  if (!daemonize ())
     {
     fprintf(stderr,"Daemonizing failed..exiting!\n");
     exit (EXIT_FAILURE);
     }
*/
    
  if ((pid_open(DEFAULT_PID,myuid,mygid)) < 0)
    {
      
      fprintf(stderr,"%s: An error occured during pidfile creation!\n",__func__);
     fprintf(stderr,"%s: Delete %s\n",__func__,DEFAULT_PID);
      syslog(LOG_INFO,"Exiting daemon...\n");
      exit(0);
    }
  loadFile(configfile);

  if (head == NULL)
	{
	fprintf(stderr,"Memory Error");
	exit(EXIT_FAILURE); 
	}
  if (total == 0)  {
    fprintf(stderr,"Errro");
    exit(EXIT_FAILURE);
  }
/*
  if (total > 1)
  {
      fprintf(stderr,"%s: Cannot handle multiple hosts\n",__func__);
  }
*/


proxy_core(head,total,chrootdir,uid_name);

return 0; 

}

void
usage (void)
{
  fprintf (stdout,
	   "mitmproxy 0.1.0.\n usage: \n mitmproxy [-c configfile] [-t chrootdir] -u userid\n");
  exit (EXIT_FAILURE);
}

void
bzeropt (line_opt * flags)
{
  flags->do_chroot = 0;
  flags->switchuser = 0;
  flags->switchgroup = 0;
  flags->userconf = 0;
}

void
check_status (int status)
{
  if (status != 0)
    {
      syslog (LOG_INFO, "Exiting daemon: error during checklog procedure");
      exit (EXIT_FAILURE);
    }
}


void sig_hup_handler(int signum)
{
syslog(LOG_INFO,"Reloading configuration\n");
reload_conf = 1;
}
void sig_term_handler(int signum)
{
syslog(LOG_INFO,"Exiting daemon\n");
int_recv = 1;
unlink(DEFAULT_PID);
exit(EXIT_FAILURE);
}
