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

    // 1. Vérifier si une instance tourne déjà (Exigence 5.1.4)
    if (access(pidfile, F_OK) == 0) {
        syslog(LOG_ERR, "Erreur : Le fichier PID %s existe déjà.", pidfile);
        exit(EXIT_FAILURE);
    }

    // 2. Premier fork (Exigence 5.1.1)
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Le père s'arrête

    // 3. Devenir leader de session
    if (setsid() < 0) exit(EXIT_FAILURE);

    // 4. Second fork (Exigence 5.1.2)
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Le premier fils s'arrête

    // 5. Environnement (Exigence 5.1.3)
    umask(0);
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    // Fermeture des descripteurs standards
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirection vers /dev/null
    open("/dev/null", O_RDONLY); // FD 0
    open("/dev/null", O_WRONLY); // FD 1
    open("/dev/null", O_RDWR);   // FD 2

    // 6. Écriture du PID (Exigence 5.1.4)
    int fd = open(pidfile, O_RDWR | O_CREAT, 0640);
    if (fd >= 0) {
        char str[16];
        snprintf(str, sizeof(str), "%d\n", getpid());
        write(fd, str, strlen(str));
        close(fd);
    }
}
