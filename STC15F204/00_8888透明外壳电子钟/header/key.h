#ifndef __KEY_H_
#define __KEY_H_

// 主函数main里需要调用一次初始化
void KEY_init();

// 主函数main里要循环调用该函数
void KEY_keyscan();

#endif