#include <stdio.h>
#include <stdlib.h>
#include "nSystem.h"

#define MIN_LINEAS 100000
#define MAX_LINEAS 150000

int escritor(int num);

int nMain() {
	nSetTimeSlice(1);
	nSetNonBlockingStdio();
	nPrintf("Cantidad de archivos? ");
	char c[4];
	nRead(0, c, 3);
	int cant = atoi(c);
	if (cant <= 0)
		return;
	srand(time(0));
	nTask tareas[cant];
	int i;
	for (i = 0; i < cant; ++i) {
		tareas[i] = nEmitTask(escritor, i);
	}
	for (i = 0; i < cant; ++i) {
		nWaitTask(tareas[i]);
	}
}

int escritor(int num) {
	char archiv[30];
	sprintf(archiv, "archivo%d.txt", num);
	int fd = nOpen(archiv, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	nFprintf(fd, "Archivo %d\n\n", num);
	int lineas = MIN_LINEAS + (rand() % (MAX_LINEAS - MIN_LINEAS));
	nPrintf("Escribiendo archivo %s de %i lineas\n", archiv, lineas);
	int i;
	for (i = 1; i <= lineas; ++i) {
		nFprintf(fd, "linea %i: %i\n", i, rand());
	}
	nClose(fd);
	nPrintf("Archivo %s finalizado\n", archiv);
}
