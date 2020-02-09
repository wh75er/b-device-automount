#ifndef __MAIN_C__
#define __MAIN_C__

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

#include "fs_determ.h"

static void handle_events(int fd, int wd, char* path);
int main();

#endif