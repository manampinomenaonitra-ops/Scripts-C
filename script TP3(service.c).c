#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define PORT 9999
#define MAX_THREADS 16 // Pool de threads fixe (Exigence 3.1.4)
#define BUFFER_SIZE 1024

// --- GLOBALES & MUTEX (Exigence 3.1.3) ---
int connexions_actives = 0;
pthread_mutex_t mutex_compteur = PTHREAD_MUTEX_INITIALIZER;

// --- FONCTION AFFICHER_STATUT (Exigence 3.1.3 - Thread-safe) ---
void afficher_statut() {
    pthread_mutex_lock(&mutex_compteur);
    printf("[STATUT] Connexions en cours : %d/%d\n", connexions_actives, MAX_THREADS);
    pthread_mutex_unlock(&mutex_compteur);
}

// Fonction utilitaire pour modifier le compteur proprement
void modifier_compteur(int delta) {
    pthread_mutex_lock(&mutex_compteur);
    connexions_actives += delta;
    pthread_mutex_unlock(&mutex_compteur);
    afficher_statut();
}

// --- TRAITEMENT CLIENT ---
void handle_client(int connfd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t n = read(connfd, buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        char response[BUFFER_SIZE + 64];
        snprintf(response, sizeof(response), "[Thread %lu] Echo : %s", (unsigned long)pthread_self(), buffer);
        write(connfd, response, strlen(response));
    }
}

// --- FONCTION DE THREAD (Exigence 3.1.1) ---
void *handle_client_thread(void *arg) {
    // 3.2 Piège évité : on récupère la copie et on libère tout de suite
    int connfd = *(int *)arg;
    free(arg); 

    // 3.1.2 Libération auto des ressources
    pthread_detach(pthread_self()); 

    handle_client(connfd);

    close(connfd);
    modifier_compteur(-1); // Fin de connexion
    return NULL;
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr *)&srv, sizeof(srv));
    listen(listenfd, 5);

    printf("SERVEUR MULTI-THREADÉ DÉMARRÉ (PORT %d)\n", PORT);

    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) continue;

        // --- 3.1.4 POOL DE THREADS FIXE ---
        pthread_mutex_lock(&mutex_compteur);
        if (connexions_actives >= MAX_THREADS) {
            char *msg_err = "Désolé : Serveur saturé (Max 16 clients).\n";
            write(connfd, msg_err, strlen(msg_err));
            close(connfd);
            pthread_mutex_unlock(&mutex_compteur);
            printf("[ALERTE] Connexion refusée : Pool saturé.\n");
            continue;
        }
        pthread_mutex_unlock(&mutex_compteur);

        // --- 3.1.1 & 3.2 PASSAGE SÉCURISÉ ---
        modifier_compteur(1);
        int *fd_copy = malloc(sizeof(int));
        *fd_copy = connfd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client_thread, fd_copy) != 0) {
            perror("Erreur pthread_create");
            modifier_compteur(-1);
            free(fd_copy);
            close(connfd);
        }
    }
    return 0;
}