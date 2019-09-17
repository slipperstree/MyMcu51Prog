// ******* 根据需要修改头文件定义，key.h 和 common.h 是必须的 ***********************************
#include "../header/key.h"
#include "../header/common.h"
#include "../header/display.h"
#include "../header/uart.h"

// ******* 根据硬件电气连接修改下列定义 ***********************************

    // 按键 IO口定义
    sbit BTN1 = P3^1;
    sbit BTN2 = P3^0;

    // 按键 按下状态和电平状态对应关系，根据具体电路修改
    #define KEY_DOWN 0   // 按键按下为低电平
    #define KEY_UP   1   // 按键未按下为高电平

// ******* 根据业务需要修改下列定义 ***********************************

    #define TICK_WAIT_CLICK_END         0x0800    //判断单击用，按下和弹起之间不能超过多久
    #define TICK_WAIT_DBCLICK_START     0x0600    //判断双击用，两次单击之间不能超过多久（如果判断成双击则单击无效）
    #define TICK_KEEPDOWN      5         //TODO:判断按住不放用，按下以后需要持续多久

// ******* 根据业务需要自己完成（填充）下列函数，不要修改函数名 ***********************************

    // 按键1的单击事件
    void doBtn1Click(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // 按键1的双击事件
    void doBtn1DBClick(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

    void doBtn2Down(){
        
    }

    void doBtn2Up(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

// ******* 以下代码一般不需要修改 ***********************************

#define STS_WAIT_CLICK_START    1
#define STS_WAIT_CLICK_END      2
#define STS_WAIT_DBCLICK_START  3
#define STS_WAIT_DBCLICK_END    4

static uchar btn1Status = STS_WAIT_CLICK_START;
static uchar btn2Status = STS_WAIT_CLICK_START;

void doBtn1Click();
void doBtn1DBClick();
void doBtn2Down();
void doBtn2Up();

//每扫描一次计数器加一，用于实现单击，双击，按住不放等操作
static uchar frameCounter = 0;
static uint ttWaitKey1ClickEnd = 0;         //单击开始后经过的时间
static uint ttWaitKey1DBClickStart = 0;     //单击完成后经过的时间，如果在TICK_DCLICK时间内又检测到按键按下则单击失效
static uint ttWaitKey1DBClickEnd = 0;         //双击开始后经过的时间

// 主函数main里要循环调用该函数
void KEY_keyscan(){

    frameCounter++;

    // ====== BTN1 处理 ========================================================
    // 按键按下 且 之前为松开状态：【按键刚刚被按下】
    if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_START){
            // 切换到等待单击完成状态
            ttWaitKey1ClickEnd = 0;
            btn1Status = STS_WAIT_CLICK_END;
            // doBtn1Down();
        }
    }

    // 按键按下 且 为等待单击完成状态：【按键按下后并保持中】
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_END){
        // 计数(最大255)
        if (ttWaitKey1ClickEnd < 0xffff) {ttWaitKey1ClickEnd++;}
    }

    // 按键放开 且 为等待单击完成状态：【按键刚刚松开】
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (BTN1 == KEY_UP && btn1Status == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //测试用，找到合适的间隔
            //UART_SendByte((ttWaitKey1ClickEnd >> 8) & 0xff);
            //UART_SendByte(ttWaitKey1ClickEnd & 0xff);
            #endif

            // 如果按下并松开的时间间隔已经超过了单击要求的最大间隔，视为无效操作，丢弃，重新等待
            if (ttWaitKey1ClickEnd > TICK_WAIT_CLICK_END)
            {
                btn1Status = STS_WAIT_CLICK_START;
            }
            // 反之，则进入等待双击事件开始状态
            // 在一定时间内无按键按下的话就认为没有双击发生，才会触发单击事件
            else {
                ttWaitKey1DBClickStart = 0;
                btn1Status = STS_WAIT_DBCLICK_START;
            }
        }
    }

    // 按键松开 且 为等待双击事件开始状态：【单击结束后的一段时间内】
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_START){
        // 计数(最大255)
        if (ttWaitKey1DBClickStart < 0xffff) {ttWaitKey1DBClickStart++;}

        if (ttWaitKey1DBClickStart > TICK_WAIT_DBCLICK_START)
        {
            // 在一定时间内无按键按下，不再等待双击，单击生效，调用单击函数
            btn1Status = STS_WAIT_CLICK_START;
            doBtn1Click();
        }
    }

    // 按键按下 且 等待双击事件开始状态：【单击结束后并在一定时间内再次按下】
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_START){
        delay_ms(1);
        if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_START){
            // 进入等待双击完成状态
            ttWaitKey1DBClickEnd = 0;
            btn1Status = STS_WAIT_DBCLICK_END;
        }
    }

    // 按键按下 且 为等待双击完成状态：【双击的第二次按键按下后并保持中】
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_END){
        // 计数(最大255)
        if (ttWaitKey1DBClickEnd < 0xffff) {ttWaitKey1DBClickEnd++;}
    }

    // 按键松开 且 为等待双击完成状态：【双击的第二次按键刚刚松开】
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_END){
        delay_ms(1);
        if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_END){
            
            // 如果第二次按键按下并松开的时间间隔已经超过了一次有效单击要求的最大间隔，视为无效双击操作，丢弃，重新等待
            if (ttWaitKey1DBClickEnd > TICK_WAIT_CLICK_END)
            {
                btn1Status = STS_WAIT_CLICK_START;
            }
            // 反之，不再继续期待三击事件，执行双击事件（如有必要响应三击甚至更多连击事件，跟单击等待双击一样追加相关标志位和等待逻辑即可）
            else {
                btn1Status = STS_WAIT_CLICK_START;
                doBtn1DBClick();
            }
        }
    }

    // ====== BTN2 处理 ========================================================
    if (BTN2 == KEY_DOWN && btn2Status == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (BTN2 == KEY_DOWN && btn2Status == STS_WAIT_CLICK_START){
            btn2Status = STS_WAIT_CLICK_END;
            // 按键2被按下
            doBtn2Down();
        }
    }

    if (BTN2 == KEY_UP && btn2Status == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (BTN2 == KEY_UP && btn2Status == STS_WAIT_CLICK_END){
            btn2Status = STS_WAIT_CLICK_START;
            // 按键2放开
            doBtn2Up();
        }
    }
}
