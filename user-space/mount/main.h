#ifndef __FS_MOUNT__
#define __FS_MOUNT__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <stdio.h>

int init_dir(const char* path);
void mount_dev(const char* mntpath, const char* devpath, const char* fstype, unsigned long mountflags, const void *data);

#endif
