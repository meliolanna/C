/* fatto io e non c'è soluzione, ma compilava. non so se funziona */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#define PERM 0644

typedef int pipe_t[2];
typedef char l[250];

int mia_random(int n){
	int casuale;
	casuale = rand() % n;
	casuale++;
	return casuale;
}

int main(int argc, char **argv) {
	int N; 		/*numero file*/
	int pid; 	/*per fork*/
	int n, i,j; 	/*indici*/
	int L; 		/* lunghezza file */
	int fd;		/* per open file*/
	l linea;	/* linea */
	l *tutteLinee;	/* array dinamico per tuutte le linee */
	int r;		/* numero random */
	pipe_t *p;	/* array dinamico per la pipeline */
	int pidFiglio, status, ritorno;

	/* controllo il nuemro di parametri */
	if(argc < 5) {
		printf("Numero di parametri insufficienti.\n");
		exit(1);
	}
	N=(argc-1)/2;

	/* controllo  i numeri */
	for(i=0; i<N; i++){
		if(atoi(argv[2*i+2]) <= 0){
			printf("I numeri interi devono essere positivi");
			exit(2);
		}
	}
	
	/* allocazione pipe */
	p = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(p == NULL) {
		printf("Errore nella malloc.\n");
		exit(3);
	}
	for(i=0; i<N; i++) {
		if(pipe(p[i]) < 0){
			printf("Errore nella pipe.\n");
			exit(5);
		}
	}
	/*alloco array linee */
	tutteLinee= (l *)malloc(N*sizeof(l));
	if(tutteLinee == NULL) {
		printf("Errore allocazione array linee.\n");
		exit(6);
	}

	/* generazione figli */
	for(n=0; n<N; n++){
		if((pid=fork()) < 0) {
			printf("Errore nella fork.\n");
			exit(4);
		}
		if(pid==0) {
			/* codice figlio */
			/* chiudo le pipe inutili */
			for(i=0; i<N; i++){
				if(i!=n) {
					close(p[i][1]);
				}
				if((n > 0) && (i!=(n-1))){
					close(p[i][0]);
				}
			}

			/* inizialiazzo il seme*/
			srand(time(NULL));
			r=mia_random(atoi(argv[2*n+2]));
			/*apro il file corrispondente*/
			if((fd=open(argv[2*N+1], O_RDONLY)) < 0){
				printf("Errore nell'apertura file %s.\n", argv[2*N+1]);
				exit(-1);
			}

			/* ricevo l'array*/
			if(n>0){
				read(p[n-1][0], tutteLinee, sizeof(tutteLinee));
			}

			/*cerco la linea r-esima*/
			L=0;
			i=0;
			while(read(fd, &linea[i], 1)){
				if(linea[i] == '\n'){
					L++;
					if(L==r){
						break;
					}
					i=0;
				} /* se no continuo a leggere la riga*/
				else {
					i++;
				}
			}
			/* passo l'array con la linea nuova */
			for(j=0; j<=i; j++){
				tutteLinee[n][j]=linea[j];
			}
			write(p[n][1], tutteLinee, sizeof(tutteLinee));

			/*exit ritornando r*/
			exit(r);
		}
	} /* fine for dei figli */

	/* codice padre */
	/*chiudo pipe che non servono */
	for(i=0; i<N; i++){
		close(p[i][1]);
		if(i!=(N-1)){
			close(p[i][0]);
		}
	}

	/*ricevo l'ultima riga */
	read(p[N-1][0], tutteLinee, sizeof(tutteLinee));
	printf("Il padre ha ricevuto le seguenti linee dai figli:\n");
	for(n=0; n<N; n++){
		printf("Linea ricevuta dal figlio di indice %d:\n%s\n", n, tutteLinee[n]);
	}

	for(n=0; n<N;n++){
		if((pidFiglio=wait(&status)) < 0){
			printf("Errorei wait\n");
			exit(7);
		}
		if((status & 0xFF) != 0) {
			printf("Terminazione del figlio %d anomala.\n", n);
		}
		ritorno=(status >> 8) & 0xFF;
		printf("Il figlio di PID %d ha ritornato il valore %d.\n", pidFiglio, ritorno);
	}

	exit(0);
}
