#ifndef SERVER_H
#define SERVER_H

#include <syslog.h>
#include <unistd.h>

// Définitions globales
#define PORT 9999
#define PID_FILE "/tmp/myserverd.pid"

// Prototypes des fonctions
void daemonize(const char *pidfile);
void handle_client(int connfd);

#endif
