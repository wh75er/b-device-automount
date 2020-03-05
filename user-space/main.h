#ifndef __MAIN_C__
#define __MAIN_C__

#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/inotify.h>
#include <linux/unistd.h>
#include <linux/string.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#include "fs_determ.h"
#include "fs_mount.h"

static void handle_events(int fd, int wd, char* path);
int main();

#endif
