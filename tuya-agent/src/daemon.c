#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "daemon.h"

Error daemonize(void)
{
    pid_t pid = fork();

    if (pid < 0)
        return -1;

    if (pid > 0)
        exit(0);

    if (setsid() < 0)
        return -1;
        
    umask(0);

    if (chdir("/") < 0)
    return -1;

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int fd = open("/dev/null", O_RDWR);

    if (fd < 0)
        return -1;

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    if (fd > STDERR_FILENO)
        close(fd);

    return 0;
}