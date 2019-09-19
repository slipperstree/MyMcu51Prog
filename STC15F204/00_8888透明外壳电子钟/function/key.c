// ******* 根据需要修改头文件定义，key.h 和 common.h 是必须的 ***********************************
#include "../header/key.h"
#include "../header/common.h"
#include "../header/display.h"
#include "../header/uart.h"

// ******* 根据硬件电气连接修改下列定义 ***********************************

    // 按键个数
    #define KEY_CNT  2

    // 按键 IO口定义
    sbit BTN1 = P3^1;
    sbit BTN2 = P3^0;

    // 按键 按下状态和电平状态对应关系，根据具体电路修改
    #define KEY_DOWN 0   // 按键按下为低电平
    #define KEY_UP   1   // 按键未按下为高电平

// ******* 根据需要修改下列用于判断单击，双击有效的时间长短 ***********************************

    #define TICK_WAIT_CLICK_END         0x0800    //判断单击用，按下和弹起之间不能超过多久
    #define TICK_WAIT_DBCLICK_START     0x0600    //判断双击用，两次单击之间不能超过多久（如果判断成双击则单击无效）
    #define TICK_KEEPDOWN               0x2000    //判断按住不放用，按下以后需要持续多久
    #define TICK_KEEPDOWN_NEXT          0x0500    //按住不放的时候，每隔多久自动触发一次按住不放的事件

// ******* 根据业务需要自定义各按键按下的处理函数（函数名无所谓，但必须是void(void)形式，而且需要在KEY_keyscan中作为回调函数传递给共通的扫描函数） ***********************************

    // 按键1 单击
    void doBtn1Click(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // 按键1 双击
    void doBtn1DBClick(){
        DISPLAY_ShowMMSS_forAWhile(30);
    }

    // 按键1 长按（只触发一次）
    void doBtn1KeepPressStart(){
        DISPLAY_ShowYYYY();
    }

    // 按键2 单击
    void doBtn2Click(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

    // 按键2 双击
    void doBtn2DBClick(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // 按键2 长按（只触发一次）
    void doBtn2KeepPressStart(){
        DISPLAY_ShowHHMM();
    }

    // 按键回调事件函数指针类型定义 （不要修改）
    // 定义了一个名为『pBtnEventFunc』的函数指针类型，可以指向的函数没有返回值，没有参数
    typedef void (*pBtnEventFunc)();

    // 按键扫描的共通函数定义 （不要修改）
    void keyScanCommon(uchar btnUpDown, 
                    pBtnEventFunc fnClick, 
                    pBtnEventFunc fnDBClick, 
                    pBtnEventFunc fnKeepPressStart, 
                    uchar btnIdx);
    
    // 主函数main里要循环调用该函数（函数名不可以修改，内容根据需要自行增减）
    // 有几个按键就需要在本函数中调用几次keyScanCommon子函数
    void KEY_keyscan(){
        
        uchar idx = 0;

        // 依次扫描所有按键，并传递回调用的函数指针(理论上支持任意多个按键)
        // 参数1：传递各按键的电平值即可，也就是定义的sbit名即可
        // 参数2：按键发生【单击事件】时希望被调用的函数名
        // 参数3：按键发生【双击事件】时希望被调用的函数名
        // 参数4：按键发生【按住不放开始事件】】时希望被调用的函数名
        // 参数5：idx++不用改
        // 根据业务的需要，用不上的事件直接传递 0 即可
        keyScanCommon(BTN1, doBtn1Click, doBtn1DBClick, doBtn1KeepPressStart, idx++);
        keyScanCommon(BTN2, doBtn2Click, doBtn2DBClick,                    0, idx++);
        //keyScanCommon(BTN3, doBtn3Click,             0, idx++); // 不需要双击事件
        //keyScanCommon(BTN4,           0, doBtn4DBClick, idx++); // 不需要单击事件
        //....

    }

// ******* 后面的代码一般不需要修改 *****************************************************************************

#define STS_WAIT_CLICK_START        1
#define STS_WAIT_CLICK_END          2
#define STS_WAIT_KEEP_PRESS_END     3
#define STS_WAIT_DBCLICK_START      4
#define STS_WAIT_DBCLICK_END        5

static uchar btnStatus[KEY_CNT];
static uint ttWaitKeyClickEnd[KEY_CNT];         //单击开始后经过的时间
static uint ttWaitKeyDBClickStart[KEY_CNT];     //单击完成后经过的时间，如果在TICK_DCLICK时间内又检测到按键按下则单击失效
static uint ttWaitKeyDBClickEnd[KEY_CNT];       //双击开始后经过的时间

// 主函数main里需要调用一次初始化
void KEY_init(){
    uchar i = 0;
    for (i = 0; i < KEY_CNT; i++)
    {
        btnStatus[i] = STS_WAIT_CLICK_START;
        ttWaitKeyClickEnd[i] = 0;
        ttWaitKeyDBClickStart[i] = 0;
        ttWaitKeyDBClickEnd[i] = 0;
    }
    
}

// 共通的按键扫描逻辑
// btnbtnUpDown : 按键的键值（电平）
// fnClick : 单击事件回调函数（判定发生单击事件的时候调用指定函数）
// fnDBClick : 双击事件回调函数（判定发生双击事件的时候调用指定函数）
// fnKeepPressStart : 按住不放开始事件回调函数（判定发生按住不放开始事件的时候调用指定函数）
// btnIdx : 按键编号（从0开始）不同的按键不重复即可，目的是每个按键使用的状态变量不可以复用，通过数组下标各自分开
// 根据业务的需要，用不上的事件直接传递 0 即可
void keyScanCommon(uchar btnUpDown, 
                    pBtnEventFunc fnClick, 
                    pBtnEventFunc fnDBClick, 
                    pBtnEventFunc fnKeepPressStart, 
                    uchar btnIdx){
    
    // 按键按下 且 之前为松开状态：【按键刚刚被按下】
    if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
            // 切换到等待单击完成状态
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_CLICK_END;
        }
    }

    // 按键按下 且 为等待单击完成状态：【按键按下后并保持中】
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        // 计数
        if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}

        // 判断是否达到按住不放事件的要求时间
        if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN) {
            // 切换到等待按住不放完成状态，并触发按住不放开始事件（该事件只触发一次，连续按住不放时会多次重复触发另外一个事件，非本事件）
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_KEEP_PRESS_END;
            if (fnKeepPressStart != 0) fnKeepPressStart();
        }
    }

    // 按键放开 且 为等待按住不放完成状态：【长时间按住按键后刚刚松开】
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // 什么也不做，回到初始状态（当然，这里也可以搞一个【长时间按下后放开的事件】或者叫【长时间单击事件】,目前没有必要）
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
        }
    }

    // 按键放开 且 为等待单击完成状态：【按键刚刚松开】
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //测试用，找到合适的间隔
            //UART_SendByte((ttWaitKeyClickEnd[btnIdx] >> 8) & 0xff);
            //UART_SendByte(ttWaitKeyClickEnd[btnIdx] & 0xff);
            #endif

            // 如果按下并松开的时间间隔已经超过了单击要求的最大间隔，视为无效操作，丢弃，重新等待
            if (ttWaitKeyClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
            {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            }
            // 反之，则进入等待双击事件开始状态
            // 在一定时间内无按键按下的话就认为没有双击发生，才会触发单击事件
            else {
                ttWaitKeyDBClickStart[btnIdx] = 0;
                btnStatus[btnIdx] = STS_WAIT_DBCLICK_START;
            }
        }
    }

    // 按键松开 且 为等待双击事件开始状态：【单击结束后的一段时间内】
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
        // 计数(最大255)
        if (ttWaitKeyDBClickStart[btnIdx] < 0xffff) {ttWaitKeyDBClickStart[btnIdx]++;}

        if (ttWaitKeyDBClickStart[btnIdx] > TICK_WAIT_DBCLICK_START)
        {
            // 在一定时间内无按键按下，不再等待双击，单击生效，调用单击回调函数
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            if (fnClick != 0) fnClick();
        }
    }

    // 按键按下 且 等待双击事件开始状态：【单击结束后并在一定时间内再次按下】
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
        delay_ms(1);
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
            // 进入等待双击完成状态
            ttWaitKeyDBClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_DBCLICK_END;
        }
    }

    // 按键按下 且 为等待双击完成状态：【双击的第二次按键按下后并保持中】
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
        // 计数(最大255)
        if (ttWaitKeyDBClickEnd[btnIdx] < 0xffff) {ttWaitKeyDBClickEnd[btnIdx]++;}
    }

    // 按键松开 且 为等待双击完成状态：【双击的第二次按键刚刚松开】
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
            
            // 如果第二次按键按下并松开的时间间隔已经超过了一次有效单击要求的最大间隔，视为无效双击操作，丢弃，重新等待
            if (ttWaitKeyDBClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
            {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            }
            // 反之，不再继续期待三击事件，调用双击回调函数（如有必要响应三击甚至更多连击事件，跟单击等待双击一样追加相关标志位和等待逻辑即可）
            else {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (fnDBClick != 0) fnDBClick();
            }
        }
    }
}