#ifndef __DISPLAY_H_
#define __DISPLAY_H_

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay();

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay();

void DISPLAY_ShowHHMM();
void DISPLAY_ShowMMDD();
void DISPLAY_ShowYYYY();

// ��ʾһС��MMDD��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowMMDD_forAWhile(int interval);
// ��ʾһС��YYYY��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowYYYY_forAWhile(int interval);

#endif
