#include <nSystem.h>
#include <fifoqueues.h>
#include <nSysimp.h>

#define true 1
#define false 0

typedef struct info {

} Info;

void consumidor();
void difusor();
void *nListen(int id);
void nMultiCast(void *info);