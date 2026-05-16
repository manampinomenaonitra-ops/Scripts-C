#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

/* Configuration du serveur */
#define PORT 9999
#define BACKLOG 10
#define BUFFER_SIZE 1024

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    char buffer[BUFFER_SIZE];
    int connection_count = 0;
    int opt = 1;

    // 1. Création de la socket TCP (Critère : Socket créée)
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERREUR] Création socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configuration SO_REUSEADDR (Critère : Configurée)
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERREUR] setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. Préparation de l'adresse et Liaison (Critère : Bind)
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("[ERREUR] Bind échoué");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 4. Écoute (Critère : Listen)
    if (listen(listenfd, BACKLOG) < 0) {
        perror("[ERREUR] Listen échoué");
        exit(EXIT_FAILURE);
    }

    printf("==========================================\n");
    printf("SERVEUR TCP ITÉRATIF (PARTIE 1) DÉMARRÉ\n");
    printf("Port d'écoute : %d\n", PORT);
    printf("En attente de clients...\n");
    printf("==========================================\n");

    // 5. Boucle principale (Critère : Boucle accept/read/write)
    while (1) {
        // Le serveur s'arrête ici jusqu'à l'arrivée d'un client
        connfd = accept(listenfd, NULL, NULL);
        
        if (connfd < 0) {
            // Gestion des erreurs sans arrêter le serveur (Critère : Gestion erreurs)
            perror("[ATTENTION] Erreur lors de l'accept");
            continue; 
        }

        connection_count++;
        printf("\n[LOG] Connexion #%d acceptée.\n", connection_count);

        // Nettoyage du tampon pour une nouvelle lecture
        memset(buffer, 0, BUFFER_SIZE);

        // Lecture du message envoyé par le client
        ssize_t n = read(connfd, buffer, BUFFER_SIZE - 1);

        if (n > 0) {
            // Affichage du log sur le serveur
            printf("[LOG] Client #%d a envoyé : %s", connection_count, buffer);

            // Préparation de l'écho numéroté
            char response[BUFFER_SIZE + 64];
            snprintf(response, sizeof(response), "[Connexion #%d] Echo : %s", connection_count, buffer);

            // Envoi au client
            if (write(connfd, response, strlen(response)) < 0) {
                perror("[ERREUR] Write échoué");
            } else {
                printf("[LOG] Réponse envoyée au client #%d.\n", connection_count);
            }
        } else if (n == 0) {
            printf("[LOG] Le client #%d s'est déconnecté sans message.\n", connection_count);
        } else {
            perror("[ERREUR] Read échoué");
        }

        // Fermeture de la connexion actuelle
        close(connfd);
        printf("[LOG] Connexion #%d terminée. Serveur libre pour le suivant.\n", connection_count);
    }

    // Sécurité (normalement jamais atteint car boucle infinie)
    close(listenfd);
    return 0;
}