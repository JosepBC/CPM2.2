#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <string.h>

#include <time.h>

#include <omp.h>

#define CERT 1
#define FALS 0

#define MAX_THREADS 128

long int solutions[MAX_THREADS];
int n_thread = 0;

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



void copyTable(int original_table[9][9], int copy_table[9][9]) {
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

int serial_recorrer(int i, int j, int tauler[9][9]) {
	int k;

	if (tauler[i][j]) {//Valor fixe no s'ha d'iterar
		if (j < 8) {
			serial_recorrer(i, j + 1, tauler);
		} else if (i < 8) {
			serial_recorrer(i + 1, 0, tauler);
		} else {
			solutions[omp_get_thread_num()]++; // Final de la taula
		}
	} else {// hi ha un 0 hem de provar
        for (k = 0; k < 10; k++) {
			if (pucPosar(i, j, k, tauler)) {
				tauler[i][j] = k; 
				if (j < 8)
					serial_recorrer(i, j + 1, tauler);
				else if (i < 8)
					serial_recorrer(i + 1, 0, tauler);
				else
					solutions[omp_get_thread_num()]++;
					
				tauler[i][j] = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
int recorrer(int i, int j, int tauler[9][9]) {
	int k;

    if(n_thread >= MAX_THREADS) {
        serial_recorrer(i, j, tauler);
    } else if (tauler[i][j]) {//Valor fixe no s'ha d'iterar
		if (j < 8)
			recorrer(i, j+1, tauler);
		else if (i < 8)
			recorrer(i+1, 0, tauler);
		else
			solutions[omp_get_thread_num()]++; // Final de la taula

	} else {// hi ha un 0 hem de provar
		int aux_table[9][9];
		copyTable(tauler, aux_table);

        for (k = 0; k < 10; k++) {
			if (pucPosar(i, j, k, aux_table)) {
				#pragma omp atomic
				n_thread++;
				
				#pragma omp task shared(solutions) firstprivate(aux_table, i, j)
				{
					aux_table[i][j] = k;
					if (j < 8)
						recorrer(i, j + 1, aux_table);
					else if (i < 8)
						recorrer(i + 1, 0, aux_table);
					else
						solutions[omp_get_thread_num()]++;

					#pragma omp atomic
					n_thread--;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
int main(int nargs, char* args[]) {
	int i = 0, j = 0, k = 0;
	
	long int nsol = 0;

	for (int i = 0; i < MAX_THREADS; i++) solutions[i] = 0;

	#pragma omp parallel shared(solutions)
	{
		#pragma omp single
		recorrer(i, j, taula);
	}
	
	for (int i = 0; i < MAX_THREADS; i++) nsol += solutions[i];

	
	if (nsol == 54474240) {
        printf("Nsol ok\n");
        exit(0);
    } else {
        printf("Bad Nsol: %ld\n", nsol);
        exit(1);
    }
	
}