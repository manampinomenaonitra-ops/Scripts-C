#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 9999
#define BACKLOG 10
#define BUFFER_SIZE 1024

// --- LA FONCTION DEMANDÉE DANS TON EXTRAIT ---
void handle_client(int connfd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    
    // Lecture du message du client
    ssize_t n = read(connfd, buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        printf("[FILS %d] Message reçu : %s", getpid(), buffer);
        
        // Préparation de l'écho avec le PID pour prouver la concurrence
        char response[BUFFER_SIZE + 50];
        snprintf(response, sizeof(response), "[PID %d] Echo : %s", getpid(), buffer);
        write(connfd, response, strlen(response));
    }
    // La socket sera fermée par le close(connfd) juste après l'appel
}

// Nettoyage des processus fils (pour éviter les zombies lors du test des 8 clients)
void handler_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;
    pid_t pid;

    // Gestion des signaux
    struct sigaction sa;
    sa.sa_handler = handler_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        exit(1);
    }

    listen(listenfd, BACKLOG);
    printf("Serveur concurrent (fork) en cours d'exécution sur le port %d...\n", PORT);

    // --- LA BOUCLE EXACTE DE TON SUJET ---
    while(1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) continue;

        pid = fork();
        if (pid == 0) {          /* FILS */
            close(listenfd);     /* Le fils ferme la socket d'écoute */
            handle_client(connfd);
            close(connfd);
            exit(0);             /* Le fils s'arrête ici */
        }
        close(connfd);           /* PÈRE : ne parle pas au client */
    }

    return 0;
}