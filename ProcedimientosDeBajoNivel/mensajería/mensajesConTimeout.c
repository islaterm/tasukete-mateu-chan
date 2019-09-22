#include "mensajes.h"

// static limita el scope de la variable a este archivo
static int pendingSends;
static int pendingReceives;

void MsgEnd()
{
  if (pendingSends || pendingReceives)
  {
    nPrintf(2, "\nNro. de tareas bloqueadas en un nSend: %d\n", pendingSends);
    nPrintf(2, "Nro. de tareas bloqueadas en un nReceive: %d\n", pendingReceives);
  }
}

int nSend(nTask task, void *msg)
{
  int returnCode;

  START_CRITICAL();
  pendingSends++;
  // Lo que se define dentro de los curly boyes es local a ese entorno y no puede
  // ser accedido desde afuera de ellos.
  {
    nTask thisTask = current_task;
    if (task->status == WAIT_SEND || task->status == WAIT_SEND_TIMEOUT)
    {
      if (task->status == WAIT_SEND_TIMEOUT)
      {
        CancelTask(task);
      }
      task->status = READY;
      PushTask(ready_queue, task); // En primer lugar de la cola
    }
    else if (task->status == ZOMBIE)
    {
      nFatalError("nSend", "El receptor es un zombie\n");
    }
    PutTask(task->send_queue, thisTask); // se coloca thisTask en la cola de tareas ready
    thisTask->send.msg = msg;
    thisTask->status = WAIT_REPLY;
    ResumeNextReadyTask();
    returnCode = thisTask->send.rc;
  }
  pendingSends--;
  END_CRITICAL();
  return returnCode;
}

void *nReceive(nTask *pTask, int timeout)
{
  void *msg;
  nTask sendTask;
  START_CRITICAL();
  pendingReceives++;
  {
    nTask thisTask = current_task;
    if (EmptyQueue(thisTask->send_queue) && timeout != 0)
    {
      if (timeout > 0)
      {
        thisTask->status = WAIT_SEND_TIMEOUT;
        ProgramTask(timeout); // La tarea se despertará después del timeout
      }
      else
      {
        thisTask->status = WAIT_SEND; // Espera indefinidamente
      }
      ResumeNextReadyTask(); // Se suspende hasta nSend
    }
    sendTask = GetTask(thisTask->send_queue);
    if (pTask != NULL)
    {
      *pTask = sendTask;
    }
    msg = sendTask == NULL ? NULL : sendTask->send.msg;
  }
  pendingReceives--;
  END_CRITICAL();
  return msg;
}

void nReply(nTask task, int returnCode)
{
  START_CRITICAL();
  if (task->status != WAIT_REPLY)
  {
    nFatalError("nReply", "Esta tarea no espera un nReply");
  }
  PushTask(ready_queue, current_task);
  task->send.rc = returnCode;
  task->status = READY;
  PushTask(ready_queue, task);
  ResumeNextReadyTask();
  END_CRITICAL();
}