/**
  \file
  \authors Giuseppe Zanichelli
  \brief File incaricato della simulazione vera e propria, e della visualizzazione del risultato.
*/

#include <stdio.h>
#include <time.h>

#include "drop.h"
#include "myrand.h"

// dichiafazione del tipo booleano e dei suoi valori true e false
typedef enum { false, true } bool;

/** inizializza la matrice mat ed il generatore pseudocasuale

   \param seed il seme della generazione

   \SPECIFICA
   1) la matrice mat[][] che rappresenta l'area di caduta viene inizializzata assegnando EMPTY ad ogni elemento
   2) il generatore psudocasuale viene inizializzato fissando il seme a seed (se seed >0) oppure il seme uguale a time(NULL) (se seed è <= 0)
*/
void init_all (int seed)
{
    // inizializzazione della matrice
    for(int i = 0; i<N; i++)
        for(int j = 0; j<M; j++)
            mat[i][j] = EMPTY;

    // inizializzazione del generatore
    if(seed > 0)
        my_srand(seed);
    else
        my_srand(time(NULL));
}

/** stampa la matrice mat sullo standard output utilizzando il carattere spazio (EMPTY) per le posizioni vuote e il carattere '*' (FULL) per le posizioni piene (i caratteri utilizzati nelle macro che li definiscono)
*/
void print_matrix (void)
{
    for(int i = 0; i<N; i++)
    {
        for(int j = 0; j<M; j++)
            printf("%c",mat[i][j]); // Stampo un elemento (penso ci siano metodi più veloci ma la leggibilità mi ringrazia)
        printf("\n"); // e poi andare a capo
    }
}

/* posizione iniziale */
#define P0_I 0
#define P0_J (M / 2) // divisione intera, essendo entrambi interi

#define IS_FULL(x) ((x) == FULL)
#define IS_EMPTY(x) ((x) == EMPTY)

/* estrazione numeri casuali */
#define RND_2()  (my_rand() & 1) // estrae un numero tra 1 e 0 (bitwise operator, sempre più veloci)
#define RND_3()  (my_rand() % 3) // aggiunge un errore dell'un per mille a favore dello 0, ma corregge quello di 1/32 del generatore (...)

/** calcola la caduta della prossima particella restituendo le coordinate (i,j) del prossimo elemento dell'area da mettere a FULL

   \param next_i l'indirizzo della variabile dove scrivere il valore della coordinata i
   \param next_j l'indirizzo della variabile dove scrivere il valore della coordinata j

   \SPECIFICA
   il prossimo elemento da riempire viene calcolato simulando la caduta di una particella
   a partire dalla posizione P0 di coordinate ( 0, M/2 ),
   ad ogni passo, se mi trovo nella posizione Pi = (i,j) considero l'insieme U delle celle non ancora piene fra la tre celle
              (i+1,j-1) (i+1,j) (i+1,j-1)
   e scelgo in modo equiprobabile fra le celle in U, ad esempio nel caso in cui le tre celle contengano rispettivamente
              EMPTY EMPTY FULL
   U = { (i+1,j-1), (i+1,j) }
   e scelgo fra i due elementi di U con probabilità 1/2.

   La caduta della particella si arresta quando di verifica uno dei due seguenti casi:
   1) sono arrivato all'ultima riga (la N-1) (quindi mi sedimento sul fondo)
   2) ho almeno una cella piena nell'intorno di (i,j)

                     (i,j-1)     (i,j)    (i,j+1)
                                (i+1,j)

    in entrambi i casi restituisco in *next_i il valore di i e in
    *next_j il valore di j

   \retval 0 se le coordinate sono state calcolate correttamente
   \retval -1 se il punto di caduta iniziale P0
    al centro della prima riga è già FULL in questo caso non viene modificato il valore di *next_i *next_j
*/
int step (int* next_i, int* next_j)
{
    // controllo se la simulazione è finita
    if(mat[P0_I][P0_J] == FULL)
        return -1; //e finisce qui

    /* ----- SIMULAZIONE ----- */

    // setto le variabili posizione alla posizione iniziale
    int i = P0_I;
    int j = P0_J;

    //queste servono per segnarsi quali celle sono libere
    bool left,  right;

    // Questo ciclo simula la caduta (i aumenta a ogni giro). Viene interrotto con break se la particella tocca un full
    for(;i<N-1;i++)
    {
        if(    ( j > 0   && IS_FULL(mat[i][j-1]) ) // controllo non sia sul bordo
            || IS_FULL(mat[i+1][j])
            || ( j < M-1 && IS_FULL(mat[i][j+1]) )
          )
          break;

        left  = (j > 0   && IS_EMPTY(mat[i+1][j-1])); // true se la cella sotto a sinistra è libera
        right = (j < M-1 && IS_EMPTY(mat[i+1][j+1])); // true se la cella sotto a destra è libera

        // scelta della direzione da prendere
        // ricorda: sotto si può sempre andare se si è giunti qui
        if(left)
            if(right)
                j += RND_3() - 1; // tutti e tre liberi: scelgo tra le tre possibilità
            else
                j += RND_2() - 1; // liberi sotto e sinistra
        else
            if(right)
                j += RND_2(); //liberi sotto e destra
            // ultimo caso j resta invariata
    }

    *next_i = i;
    *next_j = j;
    return 0;
}
