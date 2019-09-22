#include <nSystem.h>
#include <nSysimp.h>

int nSend(nTask task, void *msg);
void *nReceive(nTask *pTask, int timeout);
void nReply(nTask task, int returnCode);