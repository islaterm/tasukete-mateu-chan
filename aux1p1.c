#include <string.h>
#include <stdio.h>
#include "nSystem.h"

#define FALSE 0
#define TRUE  1

typedef struct Nodo {
	int valor;
	struct Nodo* izq;
	struct Nodo* der;
} Nodo;

int buscarSeq1(Nodo *node, int num);
int buscarSeq2(Nodo *node, int num);
int buscarSeq3(Nodo *node, int num);

char* escribir(Nodo *aux) {
	char* texto = (char*) nMalloc(50 * sizeof(char));
	char *v1, *v2;
	if (aux == NULL) {
		strcpy(texto, "");
	} else {
		v1 = escribir(aux->izq);
		v2 = escribir(aux->der);
		if (strcmp(v1, "") == 0 && strcmp(v2, "") == 0)
			sprintf(texto, "%d", aux->valor);
		else
			sprintf(texto, "%d(%s,%s)", aux->valor, v1, v2);
	}
	return texto;
}

Nodo* newNodo(int n, Nodo *izq, Nodo *der) {
	Nodo* p = (Nodo*) nMalloc(sizeof(Nodo));
	p->valor = n;
	p->izq = izq;
	p->der = der;
	return p;
}

int nMain() {
	Nodo *raiz = newNodo(2, newNodo(10, NULL, NULL), newNodo(5, newNodo(7,
			NULL, NULL), newNodo(8, NULL, newNodo(14, NULL, NULL))));
	nPrintf("ARBOL: %s\n", escribir(raiz));
	int n1 = 8;
	int n2 = 9;
	nPrintf("buscando el %d en profundidad => %d\n", n1, buscarSeq1(raiz, n1));
	nPrintf("buscando el %d en profundidad => %d\n", n2, buscarSeq1(raiz, n2));
	nPrintf("buscando el %d en paralelo => %d\n", n1, buscarSeq2(raiz, n1));
	nPrintf("buscando el %d en paralelo => %d\n", n2, buscarSeq2(raiz, n2));
	nPrintf("buscando el %d en paralelo optimizado => %d\n", n1, buscarSeq3(
			raiz, n1));
	nPrintf("buscando el %d en paralelo optimizado => %d\n", n2, buscarSeq3(
			raiz, n2));
}

int buscarSeq1(Nodo *node, int num) {
	if (node == NULL)
		return FALSE;
	else if (node->valor == num)
		return TRUE;
	else
		return buscarSeq1(node->izq, num) || buscarSeq1(node->der, num);
}

int buscarSeq2(Nodo *node, int num) {
	if (node == NULL) {
		return FALSE;
	} else if (node->valor == num) {
		return TRUE;
	} else {
		nTask task1 = nEmitTask(buscarSeq2, node->izq, num);
		nTask task2 = nEmitTask(buscarSeq2, node->der, num);
		int r1 = nWaitTask(task1);
		int r2 = nWaitTask(task2);
		return r1 || r2; //No hacer: return nWaitTask(task1) || nWaitTask(task2);
	}
}

int FOUND;
int buscarSeq3rec(Nodo *node, int num) {
	if (FOUND) {
		return TRUE;
	} else if (node == NULL) {
		return FALSE;
	} else if (node->valor == num) {
		FOUND = TRUE;
		return TRUE;
	} else {
		nTask task1 = nEmitTask(buscarSeq3rec, node->izq, num);
		nTask task2 = nEmitTask(buscarSeq3rec, node->der, num);
		int r1 = nWaitTask(task1);
		int r2 = nWaitTask(task2);
		return r1 || r2;
	}
}
int buscarSeq3(Nodo *node, int num) {
	FOUND = FALSE;
	buscarSeq3rec(node, num);
}

