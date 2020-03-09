#ifndef __FS_DETERM__
#define __FS_DETERM__

#include <blkid/blkid.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>

void get_bd_fs_type(char* path, char* type);

#endif
