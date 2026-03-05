#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int stopper = 0;
int n = 0;
pid_t *pids = NULL;

void on_hup_parent(int signal) {
  printf("Resetowanie dzieci\n");
  for (int i = 0; i < n; i++) {
    kill(pids[i], SIGHUP);
  }
}

void on_hup_child(int signal) {
  printf("[%d]Resetowanie mnie\n", getpid());
  stopper = 1;
}

int is_prime(int val) {
  if (val <= 1)
    return 0;
  if (val <= 3)
    return 1;
  if (val % 2 == 0 || val % 3 == 0)
    return 0;
  for (int i = 5; i * i <= val; i += 6) {
    if (val % i == 0 || val % (i + 2) == 0)
      return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Za malo argumentow\n");
    return EXIT_SUCCESS;
  }

  printf("Rodzic: %d\n", getpid());
  n = atoi(argv[1]);
  pids = malloc(sizeof(pid_t) * n);

  if (pids == NULL) {
    printf("Malloc failed\n");
    return EXIT_FAILURE;
  }

  struct sigaction hup;
  sigset_t mask;

  for (int i = 0; i < n; i++) {
    int pid = fork();
    switch (pid) {
    case -1:
      printf("fork failed\n");
      return EXIT_FAILURE;
    case 0:
      sigemptyset(&mask);
      hup.sa_mask = mask;
      hup.sa_flags = SA_SIGINFO;
      hup.sa_handler = (&on_hup_child);
      sigaction(SIGHUP, &hup, NULL);
      int tested = 0;
      while (1) {
        if (stopper) {
          tested = 0;
          stopper = 0;
        }
        while (!is_prime(++tested)) {
        }

        sleep(1);
        printf("[%d] %d\n", getpid(), tested);
      }
      return EXIT_SUCCESS;
    default:
      pids[i] = pid;
    }
  }

  sigemptyset(&mask);
  hup.sa_mask = mask;
  hup.sa_flags = SA_SIGINFO;
  hup.sa_handler = (&on_hup_parent);
  sigaction(SIGHUP, &hup, NULL);

  while (1) {
    pause();
  }
  free(pids);
  return EXIT_SUCCESS;
}
