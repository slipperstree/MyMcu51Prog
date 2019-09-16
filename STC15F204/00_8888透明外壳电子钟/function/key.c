#include "../header/key.h"
#include "../header/common.h"
#include "../header/display.h"

sbit BTN1 = P3^1;
sbit BTN2 = P3^0;

#define STS_KEYDOWN       1
#define STS_KEYUP         2

uchar btn1Status = STS_KEYUP;
uchar btn2Status = STS_KEYUP;

void doBtn1Down();
void doBtn1Up();
void doBtn2Down();
void doBtn2Up();

// ������main��Ҫѭ�����øú���
void KEY_keyscan(){
    if (BTN1 == 0 && btn1Status == STS_KEYUP){
        delay_ms(1);
        if (BTN1 == 0 && btn1Status == STS_KEYUP){
            btn1Status = STS_KEYDOWN;
            // ����1������
            doBtn1Down();
        }
    }

    if (BTN1 == 1 && btn1Status == STS_KEYDOWN){
        delay_ms(1);
        if (BTN1 == 1 && btn1Status == STS_KEYDOWN){
            btn1Status = STS_KEYUP;
            // ����1�ſ�
            doBtn1Up();
        }
    }

    if (BTN2 == 0 && btn2Status == STS_KEYUP){
        delay_ms(1);
        if (BTN2 == 0 && btn2Status == STS_KEYUP){
            btn2Status = STS_KEYDOWN;
            // ����2������
            doBtn2Down();
        }
    }

    if (BTN2 == 1 && btn2Status == STS_KEYDOWN){
        delay_ms(1);
        if (BTN2 == 1 && btn2Status == STS_KEYDOWN){
            btn2Status = STS_KEYUP;
            // ����2�ſ�
            doBtn2Up();
        }
    }
}

void doBtn1Down(){
    DISPLAY_ShowMMDD_forAWhile(30);
}

void doBtn1Up(){
    //DISPLAY_ShowHHMM();
}

void doBtn2Down(){
    DISPLAY_ShowYYYY_forAWhile(30);
}

void doBtn2Up(){
    //DISPLAY_ShowHHMM();
}