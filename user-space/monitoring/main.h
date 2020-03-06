#ifndef __MAIN_C__
#define __MAIN_C__

#include <errno.h>
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "fs_determ.h"

#define PROCFS_NAME "/proc/mount_info"

void write_to_file(char* dev_path, const char* dev_name, char* fs_type);
static void handle_events(int fd, int wd, char* path);
int main();

#endif
