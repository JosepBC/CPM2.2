#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define CERT 1
#define FALS 0
#define DEPTH 6

typedef struct {
    int taula[9][9];
    int i;
    int j;
} data;

data * taules;

// Ha de ser inicialment correcta !!
int taula[9][9] = \
        {1,2,3, 4,5,6,  7,8,9,  \
         9,8,7, 3,2,1,  6,5,4,  \
         6,5,4, 7,8,9,  1,2,3,  \
\
         7,9,8, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
\
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0};


void copy_table(int new_table[9][9], int original_table[9][9]) {
    for(int i=0; i<9; i++) {
        for(int j=0; j<9; j++) {
            new_table[i][j] = original_table[i][j];
        }
    }
}


int puc_posar(int x, int y, int z, int talua[][9]) {
    int i, j, pi, pj;

    for (i = 0; i < 9; i++) if (taula[x][i] == z) return FALS; // Files
    for (i = 0; i < 9; i++) if (taula[i][y] == z) return FALS; // Columnes
    
    // Quadrat
    pi = (x / 3) * 3; //truncament
    pj = y - y % 3; //truncament
    
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            if (taula[pi + i][pj + j] == z) return FALS;

    return CERT;
}

////////////////////////////////////////////////////////////////////
int recorrer(int i, int j, int taula[][9]) {
    int k;
    long int s = 0;

    if (taula[i][j]) { //Valor fixe no s'ha d'iterar

        if (j < 8) return(recorrer(i, j + 1, taula));
        else if (i < 8) return(recorrer(i + 1, 0, taula));
        else return 1; // Final de la taula

    } else { // hi ha un 0 hem de provar 

        for (k = 1; k < 10; k++) {
            if (puc_posar(i, j, k, taula)) {
                taula[i][j]= k; 
                if (j < 8) s += recorrer(i, j + 1, taula);
                else if (i < 8) s += recorrer(i + 1, 0, taula);
                else s++;
                taula[i][j]= 0;
            }
        }
    }
    
    return s;
}

int init_taules(int i, int j, int level) {
    int k;

    if (level > DEPTH) {
        //New element to list
        copy_table(taules[pointer].taula, taula);
        taules[pointer].i = i;
        taules[pointer].j = j;
        pointer++;
    }
    else if(taula[i][j]) { //Fixed value no need to iterate
        if (j < 8 ) return init_taules(i, j + 1, level);
        else if (i < 8) return init_taules(i +_1, 0, level);
    }
    else { //Elem that need to try
        for (k = 1; k < 10; k++) {
            if (puc_posar(i, j, k, taula)) {
                taula[i][j] = k;
                if (j < 8) init_taules(i, j + 1, level + 1);
                else if (i < 8) init_taules(i + 1, 0, level + 1);
                else {
                    copy_table(taules[pointer].taula, taula);
                    taules[pointer].i = i;
                    taules[pointer].j = j;
                    pointer++;
                }
                taula[i][j] = 0;
            }
        }

    }

}

////////////////////////////////////////////////////////////////////
int main(int nargs, char* args[]) {
    long int nsol = 0;
    long int sol;
    int id, total_proc;

    MPI_Init(&nargs, &args)
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_proc);

    int num_elements_per_proc = pointer / total_proc; //Total elements in each process
    int id_limit = pointer % total_proc; 
    int i = 0;
    int j = 0;

    while (i < id) {
        if (i < id_limit) j += num_elements_per_proc + 1;
        else j += num_elements_per_proc;
        i++;
    }


    nsol = recorrer(0,0);
    if (nsol == 54474240) {
        printf("Nsol ok\n");
        exit(0);
    } else {
        printf("Bad Nsol: %ld\n", nsol);
        exit(1);
    }

}