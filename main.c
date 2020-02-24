#include "main.h"

/* Read all available inotify events from the file descriptor 'fd'.
   wd is watch descriptor for the directory. */

static void
handle_events(int fd, int wd, char* path)
{
    /* Some systems cannot read integer variables if they are not
       properly aligned. On other systems, incorrect alignment may
       decrease performance. Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event. */

    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    char *ptr;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;) {

        /* Read some events. */

        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) {
            syslog(LOG_ERR, "Read failure.\n");
            exit(EXIT_FAILURE);
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */

        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        for (ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *) ptr;


            /* Print the name of the watched directory */
            /* Print the name of the file */

            char *dev_path = NULL, *fs_type = NULL; 

            if (wd == event->wd && event->len) {
                size_t path_len = strlen(path);
                size_t name_len = (size_t)event->len;
                dev_path = malloc(path_len + name_len + 2);

                fs_type = malloc(50);

                if(!dev_path || !fs_type) {
                    syslog(LOG_ERR, "Memory allocating error.\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(dev_path, "");
                strcpy(fs_type, "");

                printf("%s/", path);
                printf("%s", event->name);
                strcat(dev_path, path); 
                strcat(dev_path, "/");
                strcat(dev_path, event->name); 

                printf("(%s)", dev_path);

                /* Print event type */

                if (event->mask & IN_CREATE )
                    syslog(LOG_INFO, "Event IN_CREATE: %s/%s(%s)", path, event->name,
                                                                    dev_path);

                get_bd_fs_type(dev_path, fs_type);

                if(strcmp(fs_type, "")) {
                    syslog(LOG_INFO, "FILE: %s has type %s\n", dev_path, fs_type);

                    char* mnt_path = malloc(50);
                    if(!dev_path || !fs_type) {
                        syslog(LOG_ERR, "Memory allocating error.\n");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(mnt_path, "");
                    strcat(mnt_path, "/mnt/");
                    strcat(mnt_path, event->name);
                    mount_dev(mnt_path, dev_path, fs_type, MS_NOATIME, NULL);
                }

                free(dev_path);
                free(fs_type);
            }

            /* Print type of filesystem object */

            /*if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");

            */
        }
    }
}

/* redirect STDIN, STDOUT, STDERR to /dev/null */

void redirect_fds()
{
    for(int i = 0; i < 3; i++)
        close(i);

     if (open("/dev/null", O_RDWR) != 0)
     {
       syslog(LOG_ERR, "Unable to open /dev/null: %s", strerror(errno));
       exit(EXIT_FAILURE);
     }

    dup(0);
    dup(0);
}

/* make fork operation*/

int do_fork(void)
{
    int status = 0;

    switch(fork())
    {
        case 0:
            /* This is the child that will become the daemon. */
            break;

        case -1:
            /* Fork failure. */
            status = -1;
            break;

        default:
            /* Parent: Exit. */
            exit(0);
    }

    return status;
}

int daemonize()
{
    int status = 0;

    openlog("automount.daemon", LOG_PID, LOG_DAEMON);

    if((status = do_fork()) < 0)
        ;
    else if(setsid() < 0)
        status = -1;
    else {
        umask(0);
        if((chdir("/")) < 0)
            exit(EXIT_FAILURE);
        redirect_fds();
    }

    return status;
}

int 
main()
{
    char* path = "/dev";
    int fd, poll_num;
    int wd;
    nfds_t nfds;
    struct pollfd fds[1];

    /* Create the file descriptor for accessing the inotify API */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    /* Mark directories for events
       - File was created */

    wd = inotify_add_watch(fd, path,
                                  IN_CREATE);
    if (wd == -1) {
        fprintf(stderr, "Cannot watch '%s'\n", path);
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    /* Prepare for polling */

    nfds = 2;

    /* Inotify input */

    fds[0].fd = fd;
    fds[0].events = POLLIN;


    int status = daemonize();
    if(status != 0)
        exit(EXIT_FAILURE);

    /* Wait for events */
    syslog(LOG_INFO, "Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            syslog(LOG_ERR, "Polling error!\n");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {

                /* Inotify events are available */

                handle_events(fd, wd, path);
            }
        }
    }

    syslog(LOG_INFO, "Listening for events stopped.\n");

    /* Close inotify file descriptor */

    close(fd);

    exit(EXIT_SUCCESS);
}
