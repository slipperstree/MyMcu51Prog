#ifndef __DISPLAY_H_
#define __DISPLAY_H_

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay();

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay();

void DISPLAY_ShowHHMM();
void DISPLAY_ShowMMDD();
void DISPLAY_ShowYYYY();
void DISPLAY_ShowMMSS();

// 显示一小会MMDD，然后切换回原来的显示状态
void DISPLAY_ShowMMDD_forAWhile(int interval);
// 显示一小会YYYY，然后切换回原来的显示状态
void DISPLAY_ShowYYYY_forAWhile(int interval);
// 显示一小会MMSS，然后切换回原来的显示状态
void DISPLAY_ShowMMSS_forAWhile(int interval);

#endif
