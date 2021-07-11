// ******* ������Ҫ�޸�ͷ�ļ����壬key.h �� common.h �Ǳ���� ***********************************
#include "../header/key.h"
#include "../header/common.h"

// ******* ����Ӳ�����������޸����ж��� ***********************************

    // ��������
    #define KEY_CNT  2

    // ���� IO�ڶ���
    sbit BTN1 = P5^2;
    sbit BTN2 = P5^3;

    // ���� ����״̬�͵�ƽ״̬��Ӧ��ϵ�����ݾ����·�޸�
    #define KEY_DOWN 0   // ��������Ϊ�͵�ƽ
    #define KEY_UP   1   // ����δ����Ϊ�ߵ�ƽ

// ******* ������Ҫ�޸����������жϵ�����˫����Ч��ʱ�䳤�� ***********************************
    
    //����Ҫ��Ӧ˫��ʱ��ʱ�ɷſ�����(���Լ��ٴ������)
    #define NOT_USE_DBCLICK                       
    
    #define TICK_WAIT_CLICK_END         0x4000    //�жϵ����ã����º͵���֮�䲻�ܳ������
    #define TICK_WAIT_DBCLICK_START     0x0050    //�ж�˫���ã����ε���֮�䲻�ܳ�����ã�����жϳ�˫���򵥻���Ч��
    #define TICK_KEEPDOWN               0xA000    //�жϰ�ס�����ã������Ժ���Ҫ�������
    #define TICK_KEEPDOWN_NEXT          0x1000    //��ס���ŵ�ʱ��ÿ������Զ�����һ�ΰ�ס���ŵ��¼�

// ******* ����ҵ����Ҫ�Զ�����������µĴ�����������������ν����������void(void)��ʽ��������Ҫ��KEY_keyscan����Ϊ�ص��������ݸ���ͨ��ɨ�躯���� ***********************************

    // ����1 ����
    void doBtn1Click(){
        P55 = 1;
    }

    // ����1 ˫��
    void doBtn1DBClick(){
        
    }

    // ����1 ������ֻ����һ�Σ�
    void doBtn1KeepPressStart(){
        P55 = 1;
        UART1_Isr_Send_String("��ť1������");
    }

    // ����1 ����������������Σ�
    void doBtn1KeepPressKeepping(){
        
    }

    // ����2 ����
    void doBtn2Click(){
        P55 = 0;
    }

    // ����2 ˫��
    void doBtn2DBClick(){
        
    }

    // ����2 ������ֻ����һ�Σ�
    void doBtn2KeepPressStart(){
        P55 = 0;
        UART1_Isr_Send_String("��ť2������");
    }

    // ����2 ����������������Σ�
    void doBtn2KeepPressKeepping(){
        P55 = ~P55;
    }

    // �����ص��¼�����ָ�����Ͷ��� ����Ҫ�޸ģ�
    // ������һ����Ϊ��pBtnEventFunc���ĺ���ָ�����ͣ�����ָ��ĺ���û�з���ֵ��û�в���
    typedef void (*pBtnEventFunc)();

    // ����ɨ��Ĺ�ͨ�������� ����Ҫ�޸ģ�
    void keyScanCommon(uchar btnUpDown, 
                    pBtnEventFunc fnClick, 
                    pBtnEventFunc fnDBClick, 
                    pBtnEventFunc fnKeepPressStart, 
                    pBtnEventFunc fnKeepPressKeepping, 
                    uchar btnIdx);
    
    // ������main��Ҫѭ�����øú������������������޸ģ����ݸ�����Ҫ����������
    // �м�����������Ҫ�ڱ������е��ü���keyScanCommon�Ӻ���
    void KEY_keyscan(){
        
        uchar idx = 0;

        // ����ɨ�����а����������ݻص��õĺ���ָ��(������֧������������)
        // ����1�����ݸ������ĵ�ƽֵ���ɣ�Ҳ���Ƕ����sbit������
        // ����2�����������������¼���ʱϣ�������õĺ�����
        // ����3������������˫���¼���ʱϣ�������õĺ�����
        // ����4��������������ס���ſ�ʼ�¼�����ʱϣ�������õĺ�����
        // ����5��idx++���ø�
        // ����ҵ�����Ҫ���ò��ϵ��¼�ֱ�Ӵ��� 0 ����
        keyScanCommon(BTN1, doBtn1Click, 0, doBtn1KeepPressStart, 0, idx++);
        keyScanCommon(BTN2, doBtn2Click, 0, doBtn2KeepPressStart, doBtn2KeepPressKeepping, idx++);
        //keyScanCommon(BTN3, doBtn3Click,             0, idx++); // ����Ҫ˫���¼�
        //keyScanCommon(BTN4,           0, doBtn4DBClick, idx++); // ����Ҫ�����¼�
        //....

    }

// ******* ����Ĵ���һ�㲻��Ҫ�޸� *****************************************************************************

#define STS_WAIT_CLICK_START        1
#define STS_WAIT_CLICK_END          2
#define STS_WAIT_KEEP_PRESS_END     3
#define STS_WAIT_DBCLICK_START      4
#define STS_WAIT_DBCLICK_END        5

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
// fnKeepPressStart : ��ס���ſ�ʼ�¼��ص��������ж�������ס���ſ�ʼ�¼���ʱ�����ָ��������
// fnKeepPressKeepping : ��ס���Ų�����ס���������¼��ص��������ᷴ������ֱ�����֣�
// btnIdx : ������ţ���0��ʼ����ͬ�İ������ظ����ɣ�Ŀ����ÿ������ʹ�õ�״̬���������Ը��ã�ͨ�������±���Էֿ�
// ����ҵ�����Ҫ���ò��ϵ��¼�ֱ�Ӵ��� 0 ����
void keyScanCommon(uchar btnUpDown, 
                    pBtnEventFunc fnClick, 
                    pBtnEventFunc fnDBClick, 
                    pBtnEventFunc fnKeepPressStart,
                    pBtnEventFunc fnKeepPressKeepping, 
                    uchar btnIdx){
    
    // �������� �� ֮ǰΪ�ɿ�״̬���������ոձ����¡�
    if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
        delay_ms(1);
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
            // �л����ȴ��������״̬
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_CLICK_END;
        }
    }

    // �������� �� Ϊ�ȴ��������״̬�����������º󲢱����С�
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
            // ����
            if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}

            // �ж��Ƿ�ﵽ��ס�����¼���Ҫ��ʱ��
            if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN) {
                // �л����ȴ���ס�������״̬����������ס���ſ�ʼ�¼������¼�ֻ����һ�Σ�������ס����ʱ�����ظ���������һ���¼����Ǳ��¼���
                ttWaitKeyClickEnd[btnIdx] = 0;
                btnStatus[btnIdx] = STS_WAIT_KEEP_PRESS_END;
                if (fnKeepPressStart != 0) fnKeepPressStart();
            }
        }
    }

    // �������� �� Ϊ�ȴ���ס�������״̬�������������������У���Ӧ�����¼���
    else if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        if (btnUpDown == KEY_DOWN && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // ����
            if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}

            // �ж��Ƿ�ﵽ��ס���������¼���Ҫ��ʱ��
            if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN_NEXT) {
                // ���������¼�
                ttWaitKeyClickEnd[btnIdx] = 0;
                if (fnKeepPressKeepping != 0) fnKeepPressKeepping();
            }
        }
    }

    // �����ſ� �� Ϊ�ȴ���ס�������״̬������ʱ�䰴ס������ո��ɿ���
    else if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        delay_ms(1);
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // ʲôҲ�������ص���ʼ״̬����Ȼ������Ҳ���Ը�һ������ʱ�䰴�º�ſ����¼������߽С���ʱ�䵥���¼���,Ŀǰû�б�Ҫ��
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
        }
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
        if (btnUpDown == KEY_UP && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
            // ����(���255)
            if (ttWaitKeyDBClickStart[btnIdx] < 0xffff) {ttWaitKeyDBClickStart[btnIdx]++;}

            if (ttWaitKeyDBClickStart[btnIdx] > TICK_WAIT_DBCLICK_START)
            {
                // ��һ��ʱ�����ް������£����ٵȴ�˫����������Ч�����õ����ص�����
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (fnClick != 0) fnClick();
            }
        }
    }

    #ifndef NOT_USE_DBCLICK
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
                    if (fnDBClick != 0) fnDBClick();
                }
            }
        }
    #endif
}