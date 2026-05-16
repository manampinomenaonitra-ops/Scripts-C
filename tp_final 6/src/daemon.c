#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include "server.h"

void daemonize(const char *pidfile) {
    pid_t pid;

    // Suppression de l'ancien fichier PID s'il existe pour éviter les blocages de test
    unlink(pidfile);

    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    if (setsid() < 0) exit(1);

    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY); // 0
    open("/dev/null", O_WRONLY); // 1
    open("/dev/null", O_RDWR);   // 2

    int fd = open(pidfile, O_RDWR | O_CREAT, 0644);
    if (fd >= 0) {
        char str[16];
        snprintf(str, sizeof(str), "%d\n", getpid());
        write(fd, str, strlen(str));
        close(fd);
    }
}
