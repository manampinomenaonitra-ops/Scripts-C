#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <errno.h>
#include "server.h"

int main() {
    int listenfd, connfd;
    struct sockaddr_in srv, cli;
    socklen_t addrlen = sizeof(cli);

    openlog("myserverd", LOG_PID | LOG_CONS, LOG_DAEMON);
    daemonize(PID_FILE);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        syslog(LOG_ERR, "Bind error: %m");
        exit(1);
    }
    listen(listenfd, 10);

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)&cli, &addrlen);
        if (connfd < 0) continue;

        if (fork() == 0) {
            close(listenfd);
            handle_client(connfd);
            exit(0);
        }
        close(connfd);
    }
    return 0;
}
