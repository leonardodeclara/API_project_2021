#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#define N 12 /*dimensione massima in numero di cifre di un peso del grafo*/
#define INFINITY INT_MAX-1

/*11/08/2021*/
/*Versione da 30L con gestione della classifica attraverso heap*/

typedef struct {
    unsigned long int dist;
    int order;
    int bef;
    int vis;
    /*indicatore che il nodo nella coda è già stato visitato, quindi non devo considerarlo tra i nodi
     a min distanza dalla sorgente*/
} G_node;


typedef struct Heap{
    int index;
    unsigned long score;
} Heap_node;

/*Funzioni per l'implementazione dell'heap*/
int parent(int);
void heap_insert(Heap_node *, int, unsigned long, int *);
void build_heap(Heap_node *, int*);
void max_heapify(Heap_node *, int, int *);
void delete_max(Heap_node *, int*, unsigned long, int);
void invert(Heap_node *, int, int);

/*Funzioni per la stampa della classifica*/
void print_alt(int);
void ranking_print(Heap_node*, int);

/*Funzioni per la gestione dell'input*/
unsigned long StrInt(char *);
void AddGraph(int, int);

/*Funzioni per il calcolo dei cammini minimi*/
unsigned long mst(int dim, int col, unsigned long[dim][col]);
int nearest_node(G_node *, int);

int main() {
    int d, k;
    /*Lettura prima riga di input*/
    if(scanf("%d %d ", &d, &k)>0)
        AddGraph(d, k);
    return 0;
}

void AddGraph(int d, int k)
{
    int dim = N*d; /*dimensione del buffer di lettura stringa di testo*/
    unsigned long point; /*var dove salvo il punteggio di un grafo*/
    int supp_count;
    char buf[dim]; /*buffer*/
    char tmp[N]; /*buffer pesi grafo*/

    unsigned long weight;
    unsigned long mtrx[d][d]; /*matrice di adiacenza grafo*/
    int i, j;
    int index=0; /*conteggio indice grafi*/
    int row=0, col=0;
    int size = 0; /*dimensione dell'heap*/

    unsigned long max=0; /*valore della radice dell'heap*/

    Heap_node * rank = (Heap_node *) malloc(sizeof(Heap_node)*k);

    /*inizio la lettura dei grafi (a partire dal primo AggiungiGrafo)*/
    while (fgets(buf, N*d-1, stdin)!= NULL)
    {
        /*lettura AggiungiGrafo, resetto var di righe e colonne*/
        if(buf[0]=='A'){
            row = 0;
            col = 0;

            if (index==1) {
                max = rank[0].score;
            }
            /*Classifica piena di grafi con punteggio=0, non verrà mai aggiornata*/
            if(index > k && max == 0){
                ranking_print(rank, k);
                return;
            }
        }
        /*Lettura TopK, ma non è l'ultimo dell'input: stampo \n dopo gli indici */
        else if(buf[0]=='T' && buf[4]=='\n'){
            /*Caso in cui mi chiede subito la topk ma non ho nulla in classifica*/
            if(index == 0) {
                putchar_unlocked('\n');
                }
            else
            {
                if (index > k){
                    supp_count = k;
                }
                else{
                    supp_count = index;
                }
                ranking_print(rank, supp_count);
                putchar_unlocked('\n');
            }
        }
        else if (buf[0]=='T') /*ultima TopK del file, non stampo un \n dopo*/
            {
            if (index > k){
                supp_count = k;
            }
            else{
                supp_count = index;
            }
            ranking_print(rank, supp_count);
            }
        /*Parte che non verrà mai eseguita nei test
        else if (row >= d)
            printf("Errore: devi aggiungere un nuovo grafo\n");*/
        else
        {
            j = 0;
            for(i = 0; buf[i]!='\n'; i++)
            {
                if (buf[i] == ',')
                {
                    tmp[j]='\0';
                    weight = StrInt(tmp); /*Conversione peso grafo da str a int*/
                    mtrx[row][col]= weight;
                    col++;
                    j = 0;
                }
                else
                {
                    tmp[j]=buf[i];
                    j++;
                }

            }
            tmp[j]='\0';
            weight = StrInt(tmp);
            mtrx[row][col]= weight;
            col = 0;
            row++;
            if (row >= d)
            {
                point = mst(d,d,mtrx); /*Calcolo punteggio grafo*/

                /*Classifica piena: inserimento nuovo nodo (solo se è minore della radice) e cancellazione del massimo*/
                if(index >= k)
                {
                    if(index == k)
                    {
                        build_heap(rank, &k);
                    }
                    max = rank[0].score;
                    if (point < max) //
                    {
                        delete_max(rank, &k, point, index);
                    }
                }
                else if (index < k) /*Non ho ancora inserito k nodi in classifica, inserisco tutti i grafi letti*/
                    {
                    heap_insert(rank, index, point, &size);
                    }
                index++;
            }
        }
    }
    /*Deallocazione classifica*/
    free(rank);
}

/*Conversione da str a int*/
unsigned long StrInt(char * s){
    unsigned long val = 0;
    for(int j = 0; s[j]!='\0'; j++)
    {
        val = 10*val + (s[j]-'0');
    }
    return val;
}
/*Stampa classifica*/
void ranking_print(Heap_node* rank, int k)
{
    int i;
    for (i = 0; i < k-1; i++)
    {
        print_alt(rank[i].index);
        putchar_unlocked(' ');

    }
    print_alt(rank[i].index);

}

void print_alt(int num){
    int tmp;
    if (num / 10 == 0){
        putchar_unlocked(num+'0');
    }
    else{
        print_alt(num/10);
        tmp= num%10;
        putchar_unlocked(tmp+'0');
    }
}

/*Funzioni per calcolo cammini minimi*/
unsigned long mst(int dim, int col, unsigned long graph[dim][col]){
    G_node queue[dim]; /*Vettore delle distanze dalla sorgente (nodo 0)*/
    int count = dim;
    int ind_next;
    unsigned long score = 0;

    /*Inserisco ogni nodo (tranne la sorgente) nella coda, inizializzando le distanze ad inf */
    for(int i = 1; i<dim; i++){
        queue[i].dist=INFINITY-1;
        //queue[i].order=i+1;
        queue[i].bef=-1;
        queue[i].vis=0;
    }
    /*Pongo la distanza della sorgente da se stessa = 0 e il suo predecessore a NULL (cioè -1)*/
    queue[0].dist = 0;
    queue[0].bef = 0;
    queue[0].vis = 0;

    ind_next = 0; /*Inizio scansionando i vicini della sorgente*/
    while (count != 0)
    {
        for(int k = 0; k < dim; k++){
            if(graph[ind_next][k] > 0){
                if (queue[ind_next].dist + graph[ind_next][k] < queue[k].dist)
                {
                    /*Aggiorno le distanze dei nodi se attraverso il nodo ind_next la distanza dalla sorgente diminuisce*/
                    queue[k].dist = queue[ind_next].dist + graph[ind_next][k];
                    queue[k].bef = ind_next;
                }
            }
        }
        queue[ind_next].vis = 1;
        ind_next = nearest_node(queue, dim); /*Cerco il nodo non visitato più vicino alla sorgente*/
        count--; /*In questo modo scandisco una volta ogni elemento della coda ed evito ripetizioni*/
    }

    /*Sommo tutti i valori delle distanze in un'unica variabile*/
    for(int j = 0; j < dim; j++){
        if(queue[j].dist==INFINITY-1)
        {
            queue[j].dist=0; /*I nodi non raggiungibili costano 0*/
        }
        score = score + queue[j].dist;
    }
    return score;
}

int nearest_node(G_node * queue, int dim){
    int min_ind = dim+1;
    unsigned long min_val =  INFINITY;
    /* Ciclo che scandisce la coda per trovare il nodo a distanza minima dalla sorgente
     * che non è ancora stato visitato;
     * fisso l'indice iniziale del minimo ad un valore fuori dalla coda */
    for(int i = 0; i < dim; i++)
    {
        if (queue[i].dist<min_val && queue[i].vis != 1)
        {
            min_val = queue[i].dist;
            min_ind = i;
        }
    }
    return min_ind;
}

/*Inserimento dei primi k elementi nell'heap*/
void heap_insert(Heap_node * rank, int index, unsigned long score, int *dim){
    int i;
    rank[*dim].score = score;
    rank[*dim].index = index;

    i = *dim;
    while(i > 0){
        /* Nuovo elemento finisce verso la radice se è più grande del padre o
         * se è uguale al padre ma ha un indice più alto */
        if (rank[parent(i)].score < rank[i].score || (rank[parent(i)].score == rank[i].score && rank[parent(i)].index < rank[i].index))
        {
            invert(rank, parent(i), i); /*Scambio di posizione padre e figlio*/
            i = parent(i); /*Mi sposto sul padre per confrontarlo con il nonno alla prossima iterazione*/
        }
        else
            i = 0;
    }
    (*dim)++; /*Incremento la dimensione dell'heap*/
}

/*Funzione per lo scambio di elementi nell'heap*/
void invert(Heap_node * rank, int ind1, int ind2){

    int tmp_ind = rank[ind1].index;
    unsigned long tmp_score = rank[ind1].score;
    rank[ind1].index=rank[ind2].index;
    rank[ind1].score=rank[ind2].score;
    rank[ind2].score = tmp_score;
    rank[ind2].index=tmp_ind;

}
/*Costruzione dell'heap*/
void build_heap(Heap_node * rank, int * k){
    int root_ind = (*k -1)/2;
    while(root_ind >= 0){
        max_heapify(rank, root_ind, k);
        root_ind--;
    }
}

void max_heapify(Heap_node* rank, int root, int * k){
    int l_child = 2*root + 1;
    int r_child = 2*root + 2;
    int ind_max;
    if(l_child <= *k-1 && rank[l_child].score>rank[root].score)
    {
        ind_max = l_child;
    }
    else if (l_child <= *k-1 && rank[l_child].score==rank[root].score && rank[l_child].index >rank[root].index)
    {
        ind_max = l_child;
    }
    else
        ind_max = root;

    if (r_child <= *k -1 && rank[r_child].score>rank[ind_max].score)
    {
        ind_max = r_child;
    }
    else if (r_child <= *k -1 && rank[r_child].score==rank[ind_max].score &&  rank[r_child].index >rank[ind_max].index)
    {
        ind_max = r_child;
    }

    if (ind_max != root)
    {
        invert(rank, root, ind_max);
        max_heapify(rank, ind_max, k);
    }
}
/*Sostituzione della radice  e ribilanciamento dell'heap*/
void delete_max(Heap_node * rank, int * k, unsigned long score, int index){
    rank[0].score= score;
    rank[0].index= index;
    max_heapify(rank, 0, k);
}

/*Calcolo indice padre di un nodo nell'heap*/
int parent(int ind){
    if (ind%2 != 0)
        return ind/2;
    else
        return (ind/2 -1);
}