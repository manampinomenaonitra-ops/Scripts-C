#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include "server.h"

void handle_client(int connfd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int n = read(connfd, buffer, sizeof(buffer) - 1);
    
    if (n > 0) {
        // Echo : on renvoie ce qu'on a reçu
        write(connfd, buffer, n);
        syslog(LOG_INFO, "Echo effectué (fd=%d, octets=%d)", connfd, n);
    } else if (n < 0) {
        syslog(LOG_ERR, "Erreur de lecture sur fd=%d", connfd);
    }
    
    close(connfd);
}
