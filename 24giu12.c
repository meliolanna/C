/* Soluzione della parte C dell'esame del 12 Giugno 2024: la comunicazione in ogni coppia va dal primo processo della coppia al secondo processo della coppia ed e' il secondo processo della coppia deve creare il file con terminazione ".max" sul quale poi deve scrivere */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define PERM 0644

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
	/* -------- Variabili locali ---------- */
	int pid;			/* process identifier per le fork() */
	int N; 				/* numero di file passati sulla riga di comando */
	int nro; 			/* massimo delle lunghezze delle linee calcolate da ogni figlio */
	int nroLinea; 			/* numero delle linee calcolate da ogni figlio e dal padre */
	char *FCreato;          	/* variabile per nome file da creare da parte dei processi figli secondi della coppia */

	int fcreato; 			/* file descriptor per creazione file da parte dein processi figli secondi della coppia */
	int fd; 			/* file descriptor per apertura file */	
	pipe_t *pipe_ps;       	 	/* array di pipe per la comunicazione dai figli primi della coppia ai figli secondi della coppia */
	int n;				/* indice per i figli */
	int i, j;			/* indici per i cicli */
	char linea[250];		/* array di caratteri per memorizzare la linea: come indicato dal testo si puo' supporre una lunghezza massima di ogni linea di 250 caratteri compreso il terminatore di linea */
	int jLetto; 			/* variabile usata dal figlio secondo della coppia per recuperare le lunghezze inviate dal figlio primo della coppia */
	char lineaLetta[250];		/* variabile usata dal figlio secondo della coppia per recuperare le linee inviate dal figlio primo della coppia */
	int status, ritorno;		/* variabili per la wait */
	/* ------------------------------------ */

	/* Controllo sul numero di parametri */
	if (argc < 2) /* Meno di un parametro */  
	{
		printf("Errore nel numero dei parametri dato che argc=%d (bisogna passare almeno un nome di file!)\n", argc);
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;
	printf("DEBUG-Sono il padre con pid %d e %d file e creero' %d processi figli\n", getpid(), N, 2*N);
	
       	/* allocazione memoria dinamica per pipe_ps. NOTA BENE: servono un numero di pipe che e' la meta' del numero di figli e quindi solo N! */	       
	pipe_ps = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (pipe_ps == NULL)
	{
		printf("Errore nella allocazione della memoria per le pipe\n");
		exit(2);
	}
	
	/* Creazione delle N pipe: ATTENZIONE VANNO CREATE solo N pipe */
	for (i=0; i < N; i++)
	{
		if(pipe(pipe_ps[i]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(3);
		}
	}

	/* Ciclo di generazione dei figli: NOTA BENE DEVONO ESSERE 2 * N  */
	for (n=0; n < 2*N; n++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork del figlio %d-esimo\n", n);
			exit(4);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio: in caso di errore torniamo 0 che non e' un valore accettabile (per quanto risulta dalla specifica della parte shell) */
			if (n < N) /* siamo nel codice dei figli primi della coppia */
			{
				/* stampa di debugging */
				printf("DEBUG-PRIMO DELLA COPPIA-Figlio di indice %d e pid %d associato al file %s\n",n,getpid(),argv[n+1]); 
				/* chiudiamo le pipe che non servono */
				/* ogni figlio PRIMO della coppia scrive solo sulla pipe_ps[n] */
				for (j=0;j<N;j++)
				{
					close(pipe_ps[j][0]);
					if (j!=n)
					{
						close(pipe_ps[j][1]);
					}
				}
			 	/* ogni figlio deve aprire il suo file associato */
                                fd=open(argv[n+1], O_RDONLY);
                                if (fd < 0)
	                        {
		                	printf("Impossibile aprire il file %s\n", argv[n+1]);
			                exit(0); /* in caso di errore, decidiamo di tornare 0 che non e' un valore accettabile */ 
				}

				/* adesso il figlio legge dal file una linea alla volta */
				j=0; 		/* azzeriamo l'indice della linea */
				nroLinea=0; 	/* azzeriamo il numero della linea */
				nro=-1;		/* settiamo il massimo a -1 */
		        	while (read(fd, &(linea[j]), 1))
				{
					if (linea[j] == '\n') 
			 		{ 
						nroLinea++; /* la prima linea sara' la numero 1! */
						//printf("DEBUG-Figlio %d e pid %d nroLinea=%d\n", n, getpid(), nroLinea);
						if ((nroLinea % 2) == 1) 
			 			{ 
							/* se siamo su una linea dispari */
							/* dobbiamo mandare al secondo processo della coppia la lunghezza della linea selezionata compreso il terminatore di linea (come int) e quindi incrementiamo j */
				   			j++;
							/* quindi, per prima cosa si invia la lunghezza della linea */
							write(pipe_ps[n][1], &j, sizeof(j));
							/* e poi anche la linea */
							write(pipe_ps[n][1], linea, j);

							/* verifichiamo e nel caso aggiorniamo il massimo */
							//printf("DEBUG-Figlio %d e pid %d j=%d e nro=%d\n", n, getpid(), j, nro);
							if (j > nro) 
								nro=j; 
						}
				   		j=0; /* azzeriamo l'indice per le prossime linee */
					}
					else j++; /* continuiamo a leggere */
				}
  			}	
			else /* siamo nel codice dei figli secondi della coppia */
			{
				/* stampa di debugging */
				printf("DEBUG-SECONDO DELLA COPPIA-Figlio di indice %d e pid %d associato al file %s\n",n,getpid(),argv[n-N+1]);

				/* i figli secondi della coppia devono creare il file specificato */
				FCreato=(char *)malloc(strlen(argv[n-N+1]) + 5); /* bisogna allocare una stringa lunga come il nome del file associato + il carattere '.' + i caratteri della parola max (3) + il terminatore di stringa: ATTENZIONE ALL'INDICE PER INDIVIDUARE IL FILE */
				if (FCreato == NULL)
				{
					printf("Errore nella malloc\n");
					exit(0);
				}
				/* copiamo il nome del file associato */
				strcpy(FCreato, argv[n-N+1]);
				/* concateniamo la stringa specificata dal testo */
				strcat(FCreato,".max");
				fcreato=creat(FCreato, PERM);
				if (fcreato < 0)
				{
					printf("Impossibile creare il file %s\n", FCreato);
					exit(0);
				}

				/* chiudiamo le pipe che non servono */
				/* ogni figlio SECONDO della coppia legge solo da pipe_ps[n-N] */
				for (j=0;j<N;j++)
				{
					close(pipe_ps[j][1]);
					if (j!= n-N)     /* ATTENZIONE ALL'INDICE CHE DEVE ESSERE USATO */ 	
					{
						close(pipe_ps[j][0]);
					}
				} 
				/* ogni figlio deve aprire il suo file associato: siamo nei figli secondi della coppia e quindi attenzione all'indice */
				fd=open(argv[n-N+1], O_RDONLY);
				if (fd < 0)
				{
					printf("Impossibile aprire il file %s\n", argv[n-N+1]);
					exit(0);
				}

				/* adesso il figlio legge dal file una linea alla volta */
				j=0; 		/* azzeriamo l'indice della linea */
				nroLinea=0; 	/* azzeriamo il numero della linea */
				nro=-1;		/* settiamo il massimo a -1 */
		        	while (read(fd, &(linea[j]), 1))
				{
					if (linea[j] == '\n') 
			 		{ 
						nroLinea++; /* la prima linea sara' la numero 1! */
						//printf("DEBUG-Figlio %d e pid %d nroLinea=%d\n", n, getpid(), nroLinea);
						if ((nroLinea % 2) == 0) 
			 			{ 
							/* dato che dobbiamo ricevere dal primo processo della coppia la lunghezza della linea selezionata compreso il terminatore di linea (come int) e quindi incrementiamo j */
				   			j++;
							/* quindi, per prima cosa si legge la lunghezza della linea */
							read(pipe_ps[n-N][0], &jLetto, sizeof(jLetto));
							/* e poi anche la linea */
							read(pipe_ps[n-N][0], &lineaLetta, jLetto);
							//printf("DEBUG-Per Figlio %d e pid %d: j=%d e jLetto=%d\n", n, getpid(), j, jLetto);
							if (j > jLetto)
			 				{		/* se la linea del secondo figlio della coppia e' piu' lunga, scriviamo quella sul file */
								write(fcreato, linea, j);
							}
							else
			 				{	 /* scriviamo la linea ricevuta sul file */
								write(fcreato, lineaLetta, jLetto);
							}

							/* verifichiamo e nel caso aggiorniamo il massimo */
							//printf("DEBUG-Figlio %d e pid %d j=%d e nro=%d\n", n, getpid(), j, nro);
							if (j > nro) 
								nro=j; 
						}
				   		j=0; /* azzeriamo l'indice per le prossime linee */
					}
					else j++; /* continuiamo a leggere */
				}
			}

			/* ogni figlio deve ritornare al padre il valore corrispondente al massimo */
			exit(nro);
		}
	}
	
	/* Codice del padre */
	/* Il padre chiude tutti i lati delle pipe dato che non le usa */
	for (i=0; i < N; i++)
 	{
		close(pipe_ps[i][0]);
		close(pipe_ps[i][1]);
	}

	/* Il padre aspetta tutti i figli */
	for (n=0; n < 2*N; n++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore in wait\n");
		exit(5);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 
			ritorno=(int)((status >> 8) &	0xFF); 
			printf("Il figlio con pid=%d ha ritornato %d (se 0 problemi)\n", pid, ritorno);
		}
	}

	exit(0);
}
