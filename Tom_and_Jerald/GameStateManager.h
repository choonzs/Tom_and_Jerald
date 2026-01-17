#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

typedef void(*FP)(void);

extern int current, previous, next;

extern FP fpLoad, fpInitialize, fpUpdate, fpDraw, fpFree, fpUnload;

void GSM_Initialize(int startingState);
void GSM_Update();


#endif 