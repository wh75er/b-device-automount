#include "fs_mount.h"

int init_dir(const char* path)
{
    /* Initializes the dir: checks if dir exists if not, 
     * dir will be created
     * Returns:
     *  0 if dir existed
     *  1 if dir wasn't existed and it was created
     *  -1 if error occurred */

    struct stat st = {0};

    if(stat(path, &st) == -1) {
        if(!mkdir(path, 0755))
            return 1;
        else
            return -1;
    }

    return 0;
}

void mount_dev(const char* mntpath, const char* devpath, const char* fstype, unsigned long mountflags, const void *data) 
{
    int dir = init_dir(mntpath);
    if(dir < 0)
        exit(EXIT_FAILURE);

    int mnt = mount(devpath, mntpath, fstype, mountflags, data);
    if(!mnt)
        syslog(LOG_INFO, "Device <%s> was mounted to <%s>\n", devpath, mntpath);
    else
        syslog(LOG_ERR, "Error occurred during mount: %s\n", strerror(errno));
}

/*
int main()
{
    mount_dev("/mnt/sdc1", "/dev/sdc1", "vfat", MS_NOATIME, NULL);

    return 1;
}
*/

