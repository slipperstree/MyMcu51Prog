#ifndef __DISPLAY_H_
#define __DISPLAY_H_

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay();

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay();

void DISPLAY_ShowHHMM();
void DISPLAY_ShowMMDD();

#endif
