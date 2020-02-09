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
            perror("read");
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

            /* Print event type */

            if (event->mask & IN_CREATE )
                printf("IN_CREATE: ");

            /* Print the name of the watched directory */
            /* Print the name of the file */

            char *dev_path = NULL, *dev_type = NULL; 

            if (wd == event->wd && event->len) {
                size_t path_len = strlen(path);
                size_t name_len = (size_t)event->len;
                dev_path = malloc(path_len + name_len + 2);

                dev_type = malloc(50);

                if(!dev_path || !dev_type) {
                    perror("Memory allocating error");
                    exit(EXIT_FAILURE);
                }

                strcpy(dev_path, "");
                strcpy(dev_type, "");

                printf("%s/", path);
                printf("%s", event->name);
                strcat(dev_path, path); 
                strcat(dev_path, "/");
                strcat(dev_path, event->name); 

                printf("(%s)", dev_path);

                get_bd_fs_type(dev_path, dev_type);
                if(strcmp(dev_type, ""))
                    printf("FILE: %s has type %s\n", dev_path, dev_type);

                free(dev_path);
                free(dev_type);
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

int 
main()
{
    char* path = "/dev";
    char buf;
    int fd, poll_num;
    int wd;
    nfds_t nfds;
    struct pollfd fds[2];

    printf("Press ENTER key to terminate.\n");

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

    /* Console input */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify input */

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input */

    printf("Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {

                /* Console input is available. Empty stdin and quit */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {

                /* Inotify events are available */

                handle_events(fd, wd, path);
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */

    close(fd);

    exit(EXIT_SUCCESS);
}
