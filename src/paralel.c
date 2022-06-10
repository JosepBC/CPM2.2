#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <assert.h>

#include <string.h>

#include <time.h>

#define CERT 1
#define FALS 0
#define MAX_TABLE 1500

//multiplico per 10 per a descartar els taulells que ja hem trobat al hora de repartir.
int table[MAX_TABLE * 10][9][9];
//guardem la posicio del candidat.
int table_idx[MAX_TABLE * 10];

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


void copy_table(int original_table[9][9], int copy_table[9][9]) {
    for(int i=0; i<9; i++) {
        for(int j=0; j<9; j++) {
            copy_table[i][j] = original_table[i][j];
        }
    }
}

int pucPosar(int x, int y, int z, int tauler[9][9]) {
    int i, j, pi, pj;

    for (i = 0; i < 9; i++) if (tauler[x][i] == z) return FALS; // Files
    for (i = 0; i < 9; i++) if (tauler[i][y] == z) return FALS; // Columnes

    // Quadrat
    pi = (x / 3) * 3; //truncament
    pj = y - y % 3; //truncament
    
    for (i = 0; i < 3; i++) {    
        for (j = 0; j < 3; j++) {
            if (tauler[pi + i][pj + j] == z) {
                return FALS;
            }
        }
    }
    return CERT;
}

int recorrer(int i, int j, int tauler[9][9]) {
    int k;
    long int solucio = 0;


    if(tauler[i][j]) { //Valor fixe no s'ha d'iterar
        if (j < 8) 
            return (recorrer(i, j + 1, tauler));
        else if (i < 8) 
            return (recorrer(i + 1, 0, tauler));
        else
            return (1);
    }
    else { // hi ha un 0 hem de provar

        for (k = 1; k < 10; k++) {
            if (pucPosar(i, j, k, tauler)) {
            	tauler[i][j] = k;
                if (j < 8) 
                    solucio += recorrer(i, j + 1, tauler);
                else if (i < 8) 
                    solucio += recorrer(i + 1, 0, tauler);
                else {
                    solucio++;
                }
                tauler[i][j] = 0;
            }
        }
    }
    return (solucio);
}


int main(int nargs, char* args[]) {
	
	int id, total_proc;
	int ini = 0;
	int fin = 0;
	long int sol = 0;
	long int nsol = 0;

	MPI_Init(&nargs, &args);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_proc);

    int aux_table[9][9];
	int explored_elem = FALS;

	copy_table(taula, aux_table);

	for (ini = 0; fin - ini < MAX_TABLE; ini++) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (aux_table[i][j] == 0) {
					for (int k = 1; k < 10 ; k++) {
						if (pucPosar(i, j, k ,aux_table)) {
							//Actualitzem la posicio final actual i guardem la posicio
							copy_table(aux_table, table[fin]);
							table[fin][i][j] = k;
							table_idx[fin] = i * 9 + j; //Mla
							fin ++;
						}
					}
					explored_elem = CERT;
				}
				if (explored_elem == CERT) break;
			}
			if (explored_elem == CERT) break;
		}
		//Actualitzem la taula
		copy_table(table[ini], aux_table);
		explored_elem = FALS;
	}
    ini --;

    int proc_ini = ini + id;
    // Repartim taulers als procesos
	// Calculem el rang de cada proces per a equilibrar la carrega
	//for (int proc_ini = ini + id; proc_ini < fin; proc_ini += total_proc)
	while (proc_ini < fin) {
		int mla = table_idx[proc_ini]; 
		nsol += recorrer(mla % 9, mla / 9, table[proc_ini]);
		proc_ini += total_proc;
	}

    MPI_Reduce(&nsol, &sol, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    if( id == 0) {
    	if (sol == 54474240) {
        printf("Nsol ok: %ld\n", sol);
        exit(0);
    } else {
        printf("Bad Nsol: %ld\n", sol);
        exit(1);
    }
    }

}
