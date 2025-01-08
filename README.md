# C

*SOLUZIONI ESAMI C*

24giu12 - ESAME 12 GIUGNO 2024
- 2 processi per ogi file in input. uno per linee dispari, l'altro per le pari
- il primo manda al secondo due informazioni, il secondo  scrive su un file creato
- salvataggio pid dei processi e stampa dopo il ritorno
- creazione file con nome dato da concatenazione di cose

A - PROCESSI COMUNICANTI
1. Figli che comunicano con padre o viceversa
2. Figli che inviano in pipeline al padre o si fermano all'ultimo figlio

   a. La pipeline può procedere dal primo all'ultimo e poi forse al padre
   
   b. viceversa e poi padre
4. Figli che comunicano con nipoti, entrambe i versi
5. Nipoti con padre o viceverssa
6. Figli a coppie

   a. Figlio pari e figlio dispari

   b. Figlio i e i+N

   c. Figlio i e N-1-i, etc
8. Figli che comunicano a schema a ring

   a. Senza padre, quindi padre solo da innesco 

   b.Con padre nel ring, quindi pipe una in più dei figli 
Possono avere un reale scambio effettivo di informazioni o semplicemente di una sincronizzazione con un 'token' che è un singolo carattere. Meglio molto fare le sincronizzazioni con token, segnali molto sconisgliati.

B - NUMERO DI INFORMZIONI
1. Una sola informazione
2. Più informazioni in numero noto (anche se variabile, tipo N)
3. Più informazioni in numero non noto (tipo che va in base a ciò che trovo)

C - RECUPERO INFORMAZIONI
1. In ordine file e processi -> ultimo primo o viceversa
2. Se più info magari con istruzioni precise, sia figli in certo ordine al padre o viceversa
3. Ordine specifico, tipo prima tutti i pari o cose così
