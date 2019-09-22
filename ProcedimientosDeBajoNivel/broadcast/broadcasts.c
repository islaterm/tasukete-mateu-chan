#include "broadcasts.h"

// TODO: Revisar sistema de mensajería
// Posible solución (no aseguro que esté correcta)

Queue espera;
char *mensaje; // Mensaje emitido

void init()
{
  MakeQueue(espera);
}

void nListen(nTask emitter, char *msg)
{
  msg = NULL;             // El mensaje parte vacío hasta que emmiter envíe alguno
  START_CRITICAL();       // Se entra a la sección crítica
  while (mensaje == NULL) // Espera hasta recibir un mensaje
  {
    current_task->status = WAIT_TASK;
    PutTask(espera, current_task); // Agrega la tarea actual a la cola de espera
    ResumeNextReadyTask();          // Retoma la primera tarea de la cola de espera
  }
  msg = strcpy(mensaje);
  END_CRITICAL(); // Se sale de la sección crítica
}

void nBroadCast(char *msg, int msgLen)
{
  START_CRITICAL();
  mensaje = (char *)nMalloc(msgLen * sizeof(char *)); // Crea el mensaje
  // Copia el mensaje en la variable global
  mensaje = strcpy(msg);                              
  // Se coloca la tarea a la cola, así broadcast va a esperar hasta que todas las tareas 
  // nListen hayan terminado
  PutTask(ready_queue, current_task);
  // Se despiertan las tareas de la cola
  while (!EmptyQueue(espera))
  {
    nTask siguienteTarea = GetTask(espera);
    siguienteTarea->status = READY;
    PutTask(espera, siguienteTarea);
  }
  ResumeNextReadyTask();
  END_CRITICAL();
}