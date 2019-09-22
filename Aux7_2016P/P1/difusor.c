#include "difusor.h"

Queue pendientes;
void *buf[16];
int currId = 0;
nTask currentTask;

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
  START_CRITICAL();
  while (id > currId) // Si el id solicitado es mayor que el último en emitirse espera
  {
    current_task->status = WAIT_TASK;    // Espera el final de otra tarea
    PutTask(ready_queue, current_task); // Agrega la tarea actual a la cola de espera
    ResumeNextReadyTask();               // Retoma la primera tarea de la cola de espera
  }
  // Si el id solicitado solicita alguna de las últimas 16 la entrega
  if (id >= currId - 16)
  {
    info = buf[id % 16];
  }
  // Si no se entra al if, retorna NULL
  END_CRITICAL();
  return info;
}

void nMultiCast(void *info)
{
  START_CRITICAL();
  // Aumenta la transmisión actual y la guarda
  buf[(++currId) % 16] = info;
  // Se coloca la tarea a la cola para que quede al final. Recordar que las tareas que
  // se desbloquean quedan primeras.
  PutTask(ready_queue, current_task);
  // Se despiertan las tareas que estaban a la cola
  while (!EmptyQueue(pendientes))
  {
    nTask tareaEnEspera = GetTask(pendientes);
    tareaEnEspera->status = READY;
    PutTask(ready_queue, tareaEnEspera);
  }
  ResumeNextReadyTask();
  END_CRITICAL();
}
