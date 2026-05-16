#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char buffer[1024];
    // On utilise fgets/printf car inetd gère la socket comme stdin/stdout
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        printf("Echo-inetd: %s", buffer);
        fflush(stdout); 
    }
    return 0;
}
