#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include "server.h"

int main() {
    int listenfd, connfd;
    struct sockaddr_in srv, cli;
    socklen_t addrlen = sizeof(cli);

    // Initialisation des logs (Exigence 5.3.1)
    openlog("myserverd", LOG_PID | LOG_CONS, LOG_DAEMON);

    // Passage en mode Daemon (Exigence 5.1)
    daemonize(PID_FILE);

    syslog(LOG_INFO, "Service myserverd démarré avec succès");

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        syslog(LOG_ERR, "Erreur création socket : %m");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        syslog(LOG_ERR, "Erreur bind : %m");
        exit(EXIT_FAILURE);
    }

    listen(listenfd, 10);

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)&cli, &addrlen);
        if (connfd < 0) {
            if (errno == EINTR) continue;
            syslog(LOG_ERR, "Erreur accept : %m");
            continue;
        }

        syslog(LOG_INFO, "Connexion acceptée de %s", inet_ntoa(cli.sin_addr));

        // Logique Multi-processus (Partie 2)
        pid_t p = fork();
        if (p == 0) {
            close(listenfd);
            handle_client(connfd);
            exit(EXIT_SUCCESS);
        } else if (p < 0) {
            syslog(LOG_ERR, "Erreur fork client : %m");
        }
        
        close(connfd);
    }

    closelog();
    return 0;
}
