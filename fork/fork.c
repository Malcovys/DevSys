#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 

int main() {
    pid_t pid = fork();

    if(pid == -1) {
        printf("Erreur lors de la creation du processus\n");
    } else if (pid == 0)
    {
        printf("Je suis le processus enfant, mon PID est %d\n", getpid());
    } else {
        printf("Je suis le processus parent, mon PID est %d et le PID de mon enfant est %d\n", getpid(), pid);
    }
    
    return 0;
}