#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

void on_usr1(int signal) {
        printf("Dziecko umarlo\n");
}

void on_term(int signal) {
        kill(getppid(), SIGUSR1);
}

int main(int argc, char *argv[])
{
        sigset_t mask;
        struct sigaction usr1;
        struct sigaction term;
        pid_t pid = fork();
        switch (pid) {
                case -1:
                        printf("Fork failed\n");
                        return EXIT_FAILURE;

                case 0:
                        sigemptyset(&mask);
                        term.sa_mask = mask;
                        term.sa_flags = SA_SIGINFO;
                        term.sa_handler = (&on_term);
                        sigaction(SIGTERM, &term, NULL);
                        pause();
                        return EXIT_SUCCESS;
                default:
                        sigemptyset(&mask);
                        usr1.sa_mask = mask;
                        usr1.sa_flags = SA_SIGINFO;
                        usr1.sa_handler = (&on_usr1);
                        sigaction(SIGUSR1, &usr1, NULL);
                        sleep(2);
                        kill(pid, SIGTERM);
                        pause();
        }
        return EXIT_SUCCESS;
}
