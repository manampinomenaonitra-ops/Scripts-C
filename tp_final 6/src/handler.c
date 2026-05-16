#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include "server.h"

void handle_client(int connfd) {
    char buffer[1024];
    int n;

    while ((n = read(connfd, buffer, sizeof(buffer) - 1)) > 0) {
        write(connfd, buffer, n);
        syslog(LOG_INFO, "Echo daemon: %d octets", n);
    }
    close(connfd);
}
