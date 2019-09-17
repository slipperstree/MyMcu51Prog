// ******* ������Ҫ�޸�ͷ�ļ����壬key.h �� common.h �Ǳ���� ***********************************
#include "../header/key.h"
#include "../header/common.h"
#include "../header/display.h"
#include "../header/uart.h"

// ******* ����Ӳ�����������޸����ж��� ***********************************

    // ���� IO�ڶ���
    sbit BTN1 = P3^1;
    sbit BTN2 = P3^0;

    // ���� ����״̬�͵�ƽ״̬��Ӧ��ϵ�����ݾ����·�޸�
    #define KEY_DOWN 0   // ��������Ϊ�͵�ƽ
    #define KEY_UP   1   // ����δ����Ϊ�ߵ�ƽ

// ******* ����ҵ����Ҫ�޸����ж��� ***********************************

    #define TICK_WAIT_CLICK_END         0x0800    //�жϵ����ã����º͵���֮�䲻�ܳ������
    #define TICK_WAIT_DBCLICK_START     0x0600    //�ж�˫���ã����ε���֮�䲻�ܳ�����ã�����жϳ�˫���򵥻���Ч��
    #define TICK_KEEPDOWN      5         //TODO:�жϰ�ס�����ã������Ժ���Ҫ�������

// ******* ����ҵ����Ҫ�Լ���ɣ���䣩���к�������Ҫ�޸ĺ����� ***********************************

    // ����1�ĵ����¼�
    void doBtn1Click(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // ����1��˫���¼�
    void doBtn1DBClick(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

    void doBtn2Down(){
        
    }

    void doBtn2Up(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

// ******* ���´���һ�㲻��Ҫ�޸� ***********************************

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

//ÿɨ��һ�μ�������һ������ʵ�ֵ�����˫������ס���ŵȲ���
static uchar frameCounter = 0;
static uint ttWaitKey1ClickEnd = 0;         //������ʼ�󾭹���ʱ��
static uint ttWaitKey1DBClickStart = 0;     //������ɺ󾭹���ʱ�䣬�����TICK_DCLICKʱ�����ּ�⵽���������򵥻�ʧЧ
static uint ttWaitKey1DBClickEnd = 0;         //˫����ʼ�󾭹���ʱ��

// ������main��Ҫѭ�����øú���
void KEY_keyscan(){

    frameCounter++;

    // ====== BTN1 ���� ========================================================
    // �������� �� ֮ǰΪ�ɿ�״̬���������ոձ����¡�
    if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_START){
            // �л����ȴ��������״̬
            ttWaitKey1ClickEnd = 0;
            btn1Status = STS_WAIT_CLICK_END;
            // doBtn1Down();
        }
    }

    // �������� �� Ϊ�ȴ��������״̬�����������º󲢱����С�
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_CLICK_END){
        // ����(���255)
        if (ttWaitKey1ClickEnd < 0xffff) {ttWaitKey1ClickEnd++;}
    }

    // �����ſ� �� Ϊ�ȴ��������״̬���������ո��ɿ���
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (BTN1 == KEY_UP && btn1Status == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //�����ã��ҵ����ʵļ��
            //UART_SendByte((ttWaitKey1ClickEnd >> 8) & 0xff);
            //UART_SendByte(ttWaitKey1ClickEnd & 0xff);
            #endif

            // ������²��ɿ���ʱ�����Ѿ������˵���Ҫ������������Ϊ��Ч���������������µȴ�
            if (ttWaitKey1ClickEnd > TICK_WAIT_CLICK_END)
            {
                btn1Status = STS_WAIT_CLICK_START;
            }
            // ��֮�������ȴ�˫���¼���ʼ״̬
            // ��һ��ʱ�����ް������µĻ�����Ϊû��˫���������Żᴥ�������¼�
            else {
                ttWaitKey1DBClickStart = 0;
                btn1Status = STS_WAIT_DBCLICK_START;
            }
        }
    }

    // �����ɿ� �� Ϊ�ȴ�˫���¼���ʼ״̬���������������һ��ʱ���ڡ�
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_START){
        // ����(���255)
        if (ttWaitKey1DBClickStart < 0xffff) {ttWaitKey1DBClickStart++;}

        if (ttWaitKey1DBClickStart > TICK_WAIT_DBCLICK_START)
        {
            // ��һ��ʱ�����ް������£����ٵȴ�˫����������Ч�����õ�������
            btn1Status = STS_WAIT_CLICK_START;
            doBtn1Click();
        }
    }

    // �������� �� �ȴ�˫���¼���ʼ״̬����������������һ��ʱ�����ٴΰ��¡�
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_START){
        delay_ms(1);
        if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_START){
            // ����ȴ�˫�����״̬
            ttWaitKey1DBClickEnd = 0;
            btn1Status = STS_WAIT_DBCLICK_END;
        }
    }

    // �������� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ������º󲢱����С�
    else if (BTN1 == KEY_DOWN && btn1Status == STS_WAIT_DBCLICK_END){
        // ����(���255)
        if (ttWaitKey1DBClickEnd < 0xffff) {ttWaitKey1DBClickEnd++;}
    }

    // �����ɿ� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ����ո��ɿ���
    else if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_END){
        delay_ms(1);
        if (BTN1 == KEY_UP && btn1Status == STS_WAIT_DBCLICK_END){
            
            // ����ڶ��ΰ������²��ɿ���ʱ�����Ѿ�������һ����Ч����Ҫ������������Ϊ��Ч˫�����������������µȴ�
            if (ttWaitKey1DBClickEnd > TICK_WAIT_CLICK_END)
            {
                btn1Status = STS_WAIT_CLICK_START;
            }
            // ��֮�����ټ����ڴ������¼���ִ��˫���¼������б�Ҫ��Ӧ�����������������¼����������ȴ�˫��һ��׷����ر�־λ�͵ȴ��߼����ɣ�
            else {
                btn1Status = STS_WAIT_CLICK_START;
                doBtn1DBClick();
            }
        }
    }

    // ====== BTN2 ���� ========================================================
    if (BTN2 == KEY_DOWN && btn2Status == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (BTN2 == KEY_DOWN && btn2Status == STS_WAIT_CLICK_START){
            btn2Status = STS_WAIT_CLICK_END;
            // ����2������
            doBtn2Down();
        }
    }

    if (BTN2 == KEY_UP && btn2Status == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (BTN2 == KEY_UP && btn2Status == STS_WAIT_CLICK_END){
            btn2Status = STS_WAIT_CLICK_START;
            // ����2�ſ�
            doBtn2Up();
        }
    }
}
