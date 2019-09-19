// ******* ������Ҫ�޸�ͷ�ļ����壬key.h �� common.h �Ǳ���� ***********************************
#include "../header/key.h"
#include "../header/common.h"
#include "../header/display.h"
#include "../header/uart.h"

// ******* ����Ӳ�����������޸����ж��� ***********************************

    // ��������
    #define KEY_CNT  3

    // ���� IO�ڶ���
    sbit S2 = P3^4;
    sbit S3 = P3^5;
    sbit S4 = P3^6;
    sbit S5 = P3^7;

    // ���� ����״̬�͵�ƽ״̬��Ӧ��ϵ�����ݾ����·�޸�
    #define KEY_DOWN 0   // ��������Ϊ�͵�ƽ
    #define KEY_UP   1   // ����δ����Ϊ�ߵ�ƽ

// ******* ������Ҫ�޸����������жϵ�����˫����Ч��ʱ�䳤�� ***********************************

    #define TICK_WAIT_CLICK_END         0x0800    //�жϵ����ã����º͵���֮�䲻�ܳ������
    #define TICK_WAIT_DBCLICK_START     0x0600    //�ж�˫���ã����ε���֮�䲻�ܳ�����ã�����жϳ�˫���򵥻���Ч��
    #define TICK_KEEPDOWN      5         //TODO:�жϰ�ס�����ã������Ժ���Ҫ�������

// ******* ����ҵ����Ҫ�Զ�����������µĴ�����������������ν����������void(void)��ʽ��������Ҫ��KEY_keyscan����Ϊ�ص��������ݸ���ͨ��ɨ�躯���� ***********************************

    // ����1 ����
    void doBtn1Click(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // ����1 ˫��
    void doBtn1DBClick(){
        DISPLAY_ShowMMSS_forAWhile(30);
    }

    // ����2 ����
    void doBtn2Click(){
        DISPLAY_ShowYYYY_forAWhile(30);
    }

    // ����2 ˫��
    void doBtn2DBClick(){
        DISPLAY_ShowMMDD_forAWhile(30);
    }

    // ����3 ����
    void doBtn3Click(){
        DISPLAY_ShowYYYY();
    }

    // ����3 ˫��
    void doBtn3DBClick(){
        DISPLAY_ShowMMDD();
    }

    // �����ص��¼�����ָ�����Ͷ��� ����Ҫ�޸ģ�
    // ������һ����Ϊ��pBtnEventFunc���ĺ���ָ�����ͣ�����ָ��ĺ���û�з���ֵ��û�в���
    typedef void (*pBtnEventFunc)();

    // ����ɨ��Ĺ�ͨ�������� ����Ҫ�޸ģ�
    void keyScanCommon(uchar btnUpDown, pBtnEventFunc fnClick, pBtnEventFunc fnDBClick, uchar btnIdx);
    
    // ������main��Ҫѭ�����øú������������������޸ģ����ݸ�����Ҫ����������
    // �м�����������Ҫ�ڱ������е��ü���keyScanCommon�Ӻ���
    void KEY_keyscan(){
        
        uchar idx = 0;

        // ����ɨ�����а����������ݻص��õĺ���ָ��(������֧������������)
        // ����1�����ݸ������ĵ�ƽֵ���ɣ�Ҳ���Ƕ����sbit������
        // ����2�����������������¼���ʱϣ�������õĺ�����
        // ����3������������˫���¼���ʱϣ�������õĺ�����
        // ����4��idx++���ø�
        // ����ҵ�����Ҫ���п���ֻ��Ҫ��Ӧ�����¼�����ֻ��Ҫ��Ӧ˫���¼������Ǽ�ʹ����Ҫ��Ҳ����׼��һ���պ������ݽ�ȥ
        keyScanCommon(S2, doBtn1Click, doBtn1DBClick, idx++);
        keyScanCommon(S3, doBtn2Click, doBtn2DBClick, idx++);
        keyScanCommon(S4, doBtn3Click, doBtn3DBClick, idx++);
        //keyScanCommon(BTN4, doBtn3Click, doBtn4DBClick, idx++);
        //....

    }

// ******* ����Ĵ���һ�㲻��Ҫ�޸� *****************************************************************************

#define STS_WAIT_CLICK_START    1
#define STS_WAIT_CLICK_END      2
#define STS_WAIT_DBCLICK_START  3
#define STS_WAIT_DBCLICK_END    4

static idata uchar btnStatus[KEY_CNT];
static idata uint ttWaitKeyClickEnd[KEY_CNT];         //������ʼ�󾭹���ʱ��
static idata uint ttWaitKeyDBClickStart[KEY_CNT];     //������ɺ󾭹���ʱ�䣬�����TICK_DCLICKʱ�����ּ�⵽���������򵥻�ʧЧ
static idata uint ttWaitKeyDBClickEnd[KEY_CNT];       //˫����ʼ�󾭹���ʱ��

// ������main����Ҫ����һ�γ�ʼ��
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

// ��ͨ�İ���ɨ���߼�
// btnbtnUpDown : �����ļ�ֵ����ƽ��
// fnClick : �����¼��ص��������ж����������¼���ʱ�����ָ��������
// fnDBClick : ˫���¼��ص��������ж�����˫���¼���ʱ�����ָ��������
// btnIdx : ������ţ���0��ʼ����ͬ�İ������ظ����ɣ�Ŀ����ÿ������ʹ�õ�״̬���������Ը��ã�ͨ�������±���Էֿ�
void keyScanCommon(uchar btnUpDown, pBtnEventFunc fnClick, pBtnEventFunc fnDBClick, uchar btnIdx){
    
    // �������� �� ֮ǰΪ�ɿ�״̬���������ոձ����¡�
    if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
            // �л����ȴ��������״̬
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_CLICK_END;
            // doBtn1Down();
        }
    }

    // �������� �� Ϊ�ȴ��������״̬�����������º󲢱����С�
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        // ����(���255)
        if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}
    }

    // �����ſ� �� Ϊ�ȴ��������״̬���������ո��ɿ���
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //�����ã��ҵ����ʵļ��
            //UART_SendByte((ttWaitKeyClickEnd[btnIdx] >> 8) & 0xff);
            //UART_SendByte(ttWaitKeyClickEnd[btnIdx] & 0xff);
            #endif

            // ������²��ɿ���ʱ�����Ѿ������˵���Ҫ������������Ϊ��Ч���������������µȴ�
            if (ttWaitKeyClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
            {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            }
            // ��֮�������ȴ�˫���¼���ʼ״̬
            // ��һ��ʱ�����ް������µĻ�����Ϊû��˫���������Żᴥ�������¼�
            else {
                ttWaitKeyDBClickStart[btnIdx] = 0;
                btnStatus[btnIdx] = STS_WAIT_DBCLICK_START;
            }
        }
    }

    // �����ɿ� �� Ϊ�ȴ�˫���¼���ʼ״̬���������������һ��ʱ���ڡ�
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
        // ����(���255)
        if (ttWaitKeyDBClickStart[btnIdx] < 0xffff) {ttWaitKeyDBClickStart[btnIdx]++;}

        if (ttWaitKeyDBClickStart[btnIdx] > TICK_WAIT_DBCLICK_START)
        {
            // ��һ��ʱ�����ް������£����ٵȴ�˫����������Ч�����õ����ص�����
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            fnClick();
        }
    }

    // �������� �� �ȴ�˫���¼���ʼ״̬����������������һ��ʱ�����ٴΰ��¡�
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
        delay_ms(1);
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
            // ����ȴ�˫�����״̬
            ttWaitKeyDBClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_DBCLICK_END;
        }
    }

    // �������� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ������º󲢱����С�
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
        // ����(���255)
        if (ttWaitKeyDBClickEnd[btnIdx] < 0xffff) {ttWaitKeyDBClickEnd[btnIdx]++;}
    }

    // �����ɿ� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ����ո��ɿ���
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
            
            // ����ڶ��ΰ������²��ɿ���ʱ�����Ѿ�������һ����Ч����Ҫ������������Ϊ��Ч˫�����������������µȴ�
            if (ttWaitKeyDBClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
            {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
            }
            // ��֮�����ټ����ڴ������¼�������˫���ص����������б�Ҫ��Ӧ�����������������¼����������ȴ�˫��һ��׷����ر�־λ�͵ȴ��߼����ɣ�
            else {
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                fnDBClick();
            }
        }
    }
}