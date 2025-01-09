#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define PERM 0644

typedef int pipe_t[2];

int main(int argc, char **argv) {
  int N;
  int outfile; /* file risultato */
  int n, i; /* indici */
  char linea[250]; /* array per lettura*/
  int pid; /*per fork*/
  pipe_t *p; /* array dinamico per pipe figlio-padre*/
  pipe_t pn; /*pipe per ogni filgio-nipote*/
  int pidFiglio, status, ritorno;

  /* controllo parametri */
  if(argc < 4) {
    printf("Numero di parametri insufficiente"\n);
    exit(1);
  }
  N=argc-2;
  /* creazione file */
  if((outfile = open("/tmp/RISULTATO", O_CREAT | O_APPEND, PERM)) < 0){
    printf("Errore creazione outfile.\n");
    exit(2);
  }
  write(outfile, &argv[1], sizeof(argv[1]));

  /* allocazione pipe */
  if((p = (pipe_t *)malloc(N*sizeof(pipe_t))) == NULL) {
    printf("Malloc fallita.\n");
    exit(3);
  }
  for(i=0; i<N; i++) {
    if(pipe(p[i] < 0) {
      printf("Errore nella pipe %d.\n", i);
      exit(4);
    }
  }

  /*generazione figli*/
  for(n=0; n<N; n++) {
    if((pid=fork()) < 0) {
      printf("Errore nella fork %d.\n");
      exit(5);
    }
    if(pid==0){
      /* codice figlio||*/
/* 15min*/
