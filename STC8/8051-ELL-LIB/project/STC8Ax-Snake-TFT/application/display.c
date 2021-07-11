#include "common.h"
#include "display.h"
#include "snake.h"
#include "ILI9163LCD.h"

//#define OLED_12864
#define ILI9163LCD

// 各种block的图形定义，列行式，阴码，目前只支持8x8
const unsigned char PIC8x8_APPLE[]= {0x78,0x84,0x85,0x8F,0x8E,0xC4,0xB4,0x78};

const unsigned char PIC8x8_HEAD_U[] = {0x1C,0x22,0x4B,0x80,0x80,0x4B,0x22,0x1C};
const unsigned char PIC8x8_HEAD_D[] = {0x38,0x44,0xD2,0x01,0x01,0xD2,0x44,0x38};
const unsigned char PIC8x8_HEAD_L[] = {0x24,0x66,0x81,0xA5,0x81,0x42,0x24,0x18};
const unsigned char PIC8x8_HEAD_R[] = {0x18,0x24,0x42,0x81,0xA5,0x81,0x66,0x24};

const unsigned char PIC8x8_BODY_H[] = {0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24};
const unsigned char PIC8x8_BODY_V[] = {0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00};
const unsigned char PIC8x8_BODY_UR[] = {0x00,0x00,0x0F,0x10,0x20,0x23,0x24,0x24};
const unsigned char PIC8x8_BODY_DR[] = {0x00,0x00,0xF0,0x08,0x04,0xC4,0x24,0x24};
const unsigned char PIC8x8_BODY_LD[] = {0x24,0x24,0xC4,0x04,0x08,0xF0,0x00,0x00};
const unsigned char PIC8x8_BODY_LU[] = {0x24,0x24,0x23,0x20,0x10,0x0F,0x00,0x00};

const unsigned char PIC8x8_TAIL_U[]= {0x00,0x00,0x7F,0xC0,0xC0,0x7F,0x00,0x00};
const unsigned char PIC8x8_TAIL_D[]= {0x00,0x00,0xFE,0x03,0x03,0xFE,0x00,0x00};
const unsigned char PIC8x8_TAIL_L[]= {0x24,0x24,0x24,0x24,0x24,0x24,0x3C,0x18};
const unsigned char PIC8x8_TAIL_R[]= {0x18,0x3C,0x24,0x24,0x24,0x24,0x24,0x24};

void DISP_Clear(void){
    #ifdef OLED_12864
        u8 column,page;
        for (page = 0; page < 8; page++){
            for (column = 0; column < BLOCK_SIZE * SNAKE_SIZE_X; column++)
            {
                OLED_Set_Pos(column, page);
                OLED_WR_Byte(0x00,OLED_DATA);
            }
        }
    #endif
    #ifdef ILI9163LCD
        ILI9163_FILL_BACKGROUND_SIZE(160, 180, 0x00, 0x00);
    #endif
}

unsigned char const* DISP_getPIC(uchar blockSts){

    uchar shiwei, gewei;
    if (blockSts == STS_APPLE)
    {
        return PIC8x8_APPLE;
    } else {
        shiwei = blockSts / 10*10;
        gewei = blockSts % 10;
        
        switch (shiwei)
        {
        case STS_HEAD:
            switch (gewei)
            {
            case STS_NEXT_TO_U:
                return PIC8x8_HEAD_U;
            case STS_NEXT_TO_D:
                return PIC8x8_HEAD_D;
            case STS_NEXT_TO_L:
                return PIC8x8_HEAD_L;
            case STS_NEXT_TO_R:
                return PIC8x8_HEAD_R;
            default:
                break;
            }
            break;
        case STS_BODY_PREV_U:
            switch (gewei)
            {
            case STS_NEXT_TO_D:
                return PIC8x8_BODY_V;
            case STS_NEXT_TO_L:
                return PIC8x8_BODY_LU;
            case STS_NEXT_TO_R:
                return PIC8x8_BODY_UR;
            default:
                break;
            }
            break;
        case STS_BODY_PREV_D:
            switch (gewei)
            {
            case STS_NEXT_TO_U:
                return PIC8x8_BODY_V;
            case STS_NEXT_TO_L:
                return PIC8x8_BODY_LD;
            case STS_NEXT_TO_R:
                return PIC8x8_BODY_DR;
            default:
                break;
            }
            break;
        case STS_BODY_PREV_L:
            switch (gewei)
            {
            case STS_NEXT_TO_U:
                return PIC8x8_BODY_LU;
            case STS_NEXT_TO_D:
                return PIC8x8_BODY_LD;
            case STS_NEXT_TO_R:
                return PIC8x8_BODY_H;
            default:
                break;
            }
            break;
        case STS_BODY_PREV_R:
            switch (gewei)
            {
            case STS_NEXT_TO_U:
                return PIC8x8_BODY_UR;
            case STS_NEXT_TO_D:
                return PIC8x8_BODY_DR;
            case STS_NEXT_TO_L:
                return PIC8x8_BODY_H;
            default:
                break;
            }
            break;
        case STS_TAIL:
            switch (gewei)
            {
            case STS_NEXT_TO_U:
                return PIC8x8_TAIL_U;
            case STS_NEXT_TO_D:
                return PIC8x8_TAIL_D;
            case STS_NEXT_TO_L:
                return PIC8x8_TAIL_L;
            case STS_NEXT_TO_R:
                return PIC8x8_TAIL_R;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

	return 0;
}

// 局部刷新函数，根据传入的游戏MAP指定位置，在画面上相应的区域进行重绘
// x,y : 游戏MAP坐标
// 
// 由于显示设备的画面刷新单位区域各不相同，有一些可以指定某一个点，
// 但有一些（比如这款12864的oled屏）是以字节为单位进行刷新的
// 所以，由于想刷新的区域所在的字节里也可能包含其他不应该被刷新的部分，
// 从而也需要取到这部分不应该被刷新的部分的数据，再加上这次真正想刷新的数据的部分，才能进行刷新，否则原来的数据就丢失了
void DISP_updateGameBlock(uchar x, uchar y){
    uchar blockSts;
    uchar const* pPIC;

    #ifdef OLED_12864
        uchar blockStartColumn,blockEndColumn,blockPage;
    #endif
    #ifdef ILI9163LCD
        u16 startX, startY,width,height;
        u8 colorH,colorL;
    #endif

    blockSts = snake_getPointData(x, y);
    pPIC = DISP_getPIC(blockSts);
    
    #ifdef OLED_12864
        blockStartColumn = getBlockStartColumnByX(x);
        blockEndColumn = getBlockEndColumnByX(x);
        blockPage = getBlockStartPageByY(y);

        for (column = blockStartColumn; column <= blockEndColumn; column++)
        {
            OLED_Set_Pos(column, blockPage);

            if (blockSts == STS_EMPTY) {
                OLED_WR_Byte(0x00,OLED_DATA);
            } else {
                OLED_WR_Byte(pPIC[column - blockStartColumn],OLED_DATA);
            }
        }
        // OLED_ShowNum(0, 6, x, 1, 16);
        // OLED_ShowNum(40, 6, y, 1, 16);
        // OLED_ShowNum(60, 6, blockSts, 2, 16);
    #endif
    #ifdef ILI9163LCD
        #define DRAW_HEAD_SIZE 5
        startX = x * DRAW_HEAD_SIZE + 1;
        startY = y * DRAW_HEAD_SIZE + 1;
        width = DRAW_HEAD_SIZE-2;
        height = DRAW_HEAD_SIZE-2;
        if (blockSts == STS_EMPTY) {
            ILI9163_FILL_Rectange(x * DRAW_HEAD_SIZE, y * DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, 0x00, 0x00);
        } else if(blockSts == STS_APPLE) {
            //ILI9163LCD(pPIC[column - blockStartColumn],OLED_DATA);
            ILI9163_FILL_Rectange(startX, startY, width, height, 0x00, 0x1f);
        } else if(blockSts / 10 == STS_HEAD / 10) {
            ILI9163_FILL_Rectange(x * DRAW_HEAD_SIZE, y * DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, 0x07, 0xe0);
        } else {
            ILI9163_FILL_Rectange(x * DRAW_HEAD_SIZE, y * DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, DRAW_HEAD_SIZE, 0x00, 0x00);
            ILI9163_FILL_Rectange(startX, startY, width, height, 0xff, 0xff);
        }
    #endif
}

//显示一串数字num
void DISP_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2){
    #ifdef OLED_12864
        // todo
    #endif
    #ifdef ILI9163LCD
        ILI9163_ShowNum(x, y, num, len, size2, 0x00, 0x00, 0xff, 0xff );
    #endif
}

//显示字符串   除汉字
void DISP_ShowString(u8 x,u8 y, u8 *chr,u8 Char_Size){
    #ifdef OLED_12864
        // todo
    #endif
    #ifdef ILI9163LCD
        // todo
    #endif
}	    
