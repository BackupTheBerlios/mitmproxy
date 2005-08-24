#ifndef _UTILS_H
#define _UTILS_H
#define UNSAFE_DIR_PERMS -10
#define MUST_DROP_PRIV -11
#define CHROOT_ERR -12
int dir_unsafe(const char *dir);
int jchroot (const char *newroot);
int drop_privs (char *name);
int daemonize (void);
#endif
