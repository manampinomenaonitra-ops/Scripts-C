#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#define PORT 9999
#define BUFFER_SIZE 1024

// Fonction pour afficher l'heure dans les logs (Style Pro)
void log_message(const char* msg, int fd) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] %s (Client FD: %d)\n", t->tm_hour, t->tm_min, t->tm_sec, msg, fd);
}

int main() {
    int listenfd, connfd, max_fd, activity, i, n;
    int clients[FD_SETSIZE]; 
    struct sockaddr_in srv;
    char buffer[BUFFER_SIZE];
    fd_set readfds; 

    for (i = 0; i < FD_SETSIZE; i++) clients[i] = -1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("ERREUR BIND");
        exit(1);
    }
    listen(listenfd, 10);

    printf("\033[1;32m[SYSTEM] Serveur Multiplexé v4.0 démarré sur le port %d\033[0m\n", PORT);
    printf("[SYSTEM] Mode : Monothreadé (select) | Capacité : %d FDs\n", FD_SETSIZE);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(listenfd, &readfds);
        max_fd = listenfd;
        int current_connections = 0;

        for (i = 0; i < FD_SETSIZE; i++) {
            if (clients[i] > 0) {
                FD_SET(clients[i], &readfds);
                current_connections++;
                if (clients[i] > max_fd) max_fd = clients[i];
            }
        }

        struct timeval timeout = {5, 0}; // 5 secondes
        activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);

        if (activity == 0) {
            printf("[IDLE] En attente d'activité... (%d client(s) connecté(s))\n", current_connections);
            continue;
        }

        // Nouvelle connexion
        if (FD_ISSET(listenfd, &readfds)) {
            connfd = accept(listenfd, NULL, NULL);
            log_message("NOUVELLE_CONNEXION", connfd);
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clients[i] == -1) {
                    clients[i] = connfd;
                    break;
                }
            }
        }

        // Gestion des clients
        for (i = 0; i < FD_SETSIZE; i++) {
            int fd = clients[i];
            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                n = read(fd, buffer, BUFFER_SIZE - 1);

                if (n <= 0) {
                    log_message("DECONNEXION", fd);
                    close(fd);
                    clients[i] = -1;
                } else {
                    // Log du message reçu
                    printf("\033[0;36m[RECV]\033[0m FD %d dit: %.*s", fd, n, buffer);
                    
                    // Envoi de l'écho pur (sans "Partie 4")
                    write(fd, buffer, n);
                }
            }
        }
    }
    return 0;
}
