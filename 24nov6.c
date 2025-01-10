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
  int pidNipote;

  /* controllo parametri */
  if(argc < 4) {
    printf("Numero di parametri insufficiente"\n);
    exit(1);
  }
  N=argc-2;
  /* creazione file */
  if((outfile = open("/tmp/RISULTATO", O_WRONLY | O_CREAT | O_APPEND, PERM)) < 0){
    printf("Errore creazione outfile.\n");
    exit(2);
  }
  write(outfile, &argv[1], strlen(argv[1]));
  write(outfile, "\n", 1);

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
      /* codice figlio */
      /* chiusura pipe con padre che non servono */
      for(i=0; i<N; i++) [
          close(p[i][0]);
          if(n!=i){
            close(p[i][1]);
          }
      }
      /* creazione pipe figlio-nipote */
      if(pipe(pn) < 0) {
        printf("Creazione pipe figlio nipote %d fallita.\n", n);
        exit(-1);
      }

      /* creo il nipote */
      if((pid=fork()) < 0) {
        printf("Errore nella fork del nipote %d.\n", n);
        exit(-1);
      }
      if(pid==0) {
        /* codcie del nipote */
        /* chiudo le pipe padre rimaste e il lato nipote inutile */
        close(p[n][1]);
        close(pn[0]);
        /* chiudo stdout e metto la pipe al figlio per mandargli il ris dell'exec*/
        close(1);
        dup(pn[1]);
        close(pn[1]);
        execpl("ls", "ls", "-li", argv[n+2], (char *)0);
        /* se produce errori arrivo qui */
        perror("Qualcosa dell'exec è andato male.\n");
        exit(-1);
      }
      /* torna il codice ddel figlio */
      /* chiudo il lato col nipote che non serve */
      close(pn[1]);
      /* leggo ciò che ha madnato il nipote */
      i=0;
      while(read(pn[0], &linea[i], 1)){
        i++;
      }
      /* la scrivo sul file */
      write(outfile, linea, i);
      write(outfile, "\n", 1);
      /* invio la linea al padre */
      write(p[n][1], linea, i);

      /* guardo il valore di ritorno del figlio? */
      ritorno=-1;
      if((pidNipote=wait(&status)) < 0) {
        printf("errore wait\n");
        exit(-1);
      }
      if((status & 0xFF) != 0){
        printf("Terminazione anomala nipote %d\n", n);
        exit(-1);
      }
      ritorno=(int)((status >> 8) & 0xFF);
      /* ritorna al padre il ritorno del nipote */
      exit(ritorno);
  }

  /*codice padre rieccoci */
  /* chiudo le pipe */
  for(i=0; i<N; i++) {
    close(p[i][1]);
  }
  for(n=0; n<N; n++) {
    /* legge la linea */
    i=0;
    while(read(pn[0], &linea[i], 1)){
      i++;
    }
    linea[i]='\0';
    printf("La linea comunicata dal figlio %d, realtiva al file %s nella diretory %s è la seguente:\n%s\n", n, argv[n+2], argv[1], linea);
  }
  /* Il padre aspetta i figli */
    for (n=0; n < N; n++){
      pid = wait(&status);
      if (pid < 0){
        printf("Errore in wait\n");
        exit(7);
      }
      if ((status & 0xFF) != 0) {
        printf("Figlio con pid %d terminato in modo anomalo\n", pid);
      } else { 	
        ritorno=(int)((status >> 8) & 0xFF);
        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi o nel figlio o nel nipote)\n", pid, ritorno);
      }
    }
    exit(0);
}

