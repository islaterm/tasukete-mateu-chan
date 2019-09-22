#include "difusor.h"

Queue pendientes;
int candado = OPEN;
void *buf[16];
int currId = 0;

void init()
{
  MakeQueue(pendientes);
}

void difusor()
{
  while (true)
  {
    Info *info = producir();
    nMultiCast(info);
  }
}

void consumidor()
{
  int id = 1;
  while (true)
  {
    Info *info = (Info *)nListen(id);
    consumidor(info);
    id++;
  }
}

void *nListen(int id)
{
  void *info = NULL; // Mensaje que se espera conseguir
  spinLock(&candado);
  while (id > currId) // Si el id solicitado es mayor que el último en emitirse espera
  {
    // Se crea un waitLock cerrado para forzar la espera, liberando antes el spinlock de
    // la sección crítica
    int waitLock = CLOSED;
    PutObj(pendientes, &waitLock);
    spinUnlock(&candado);
    spinLock(&waitLock);
    // Una vez que se despierta al proceso se debe volver a pedir el candado de la
    // sección crítica
    spinLock(&candado);
  }
  // Si el id solicitado solicita alguna de las últimas 16 la entrega
  if (id >= currId - 16)
  {
    info = buf[id % 16];
  }
  // Si no se entra al if, retorna NULL
  spinUnlock(&candado);
  return info;
}

void nMultiCast(void *info)
{
  spinLock(&candado);
  // Aumenta la transmisión actual y la guarda
  buf[(++currId) % 16] = info;
  // Se despiertan las tareas que estaban a la cola
  while (!EmptyQueue(pendientes))
  {
    int *pWaitLock = (int *)GetObj(pendientes); // Puntero al wait lock del otro core
    spinUnlock(pWaitLock);
  }
  spinUnlock(&candado);
}
