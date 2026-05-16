#ifndef SERVER_H
#define SERVER_H

#include <syslog.h>
#include <unistd.h>
#include <errno.h>

#define PORT 9999
#define PID_FILE "/tmp/myserverd.pid"

// Prototypes
void daemonize(const char *pidfile);
void handle_client(int connfd);

#endif
