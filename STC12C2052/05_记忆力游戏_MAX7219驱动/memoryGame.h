#include <STC12x20xx.h>

#ifndef __TXS_MEMORYGAME_H_
#define __TXS_MEMORYGAME_H_

#define uchar unsigned char
#define uint unsigned int

void mg_restart();
void mg_gameover();

uint mg_moveUp();
uint mg_moveDown();
uint mg_moveLeft();
uint mg_moveRight();

uint mg_KeepPressingU();
uint mg_KeepPressingD();
uint mg_KeepPressingL();
uint mg_KeepPressingR();

uint mg_moveNext();

void mg_createPuzzle();

void mg_moveEndBlock();

uchar mg_getRowDataForShow(uchar);

#endif