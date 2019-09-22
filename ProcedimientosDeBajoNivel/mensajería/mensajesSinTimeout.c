#include "mensajes.h"

int nSend(nTask task, void *msg)
{
  int returnCode;
  START_CRITICAL();
  nTask thisTask = current_task;
  if (task->status == WAIT_SEND)
  {
    task->status = READY;
    // Se coloca la tarea en primer lugar de la cola
    PushTask(ready_queue, task);
  }
  PutTask(task->send_queue, thisTask);
  thisTask->send.msg = msg;
  thisTask->status = WAIT_REPLY;
  ResumeNextReadyTask();
  returnCode = thisTask->send.rc;
  END_CRITICAL();
  return returnCode;
}

void *nReceive(nTask *pTask, int timeout)
{
  void *msg;
  nTask sendTask;
  START_CRITICAL();
  nTask thisTask = current_task;
  if (EmptyQueue(thisTask->send_queue) && timeout != 0)
  {
    thisTask->status = WAIT_SEND; // La tarea espera indefinidamente
    ResumeNextReadyTask();        // Se suspende indefinidamente hasta un nSend
  }
  sendTask = GetTask(thisTask->send_queue);
  if (pTask != NULL)
  {
    *pTask = sendTask;
  }
  msg = sendTask == NULL ? NULL : sendTask->send.msg;
  END_CRITICAL();
  return msg;
}

void nReply(nTask task, int returnCode)
{
  START_CRITICAL();
  PushTask(ready_queue, current_task);
  task->send.rc = returnCode;
  task->status = READY;
  PushTask(ready_queue, task);
  ResumeNextReadyTask();
  END_CRITICAL();
}