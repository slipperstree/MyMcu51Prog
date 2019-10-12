#include "../header/common.h"
#include "../header/GT20L16S1Y.h"

sbit pCE        =P2^0;         // 片选
sbit pSCLK      =P2^1;         // 时钟
sbit pSI        =P2^2;         // 数据输入（单片机->字库芯片）
sbit pSO        =P2^3;         // 数据输出（字库芯片->单片机）

// 不使用字库芯片而使用固定文字时放开这一段，并修改程序里的 bufHZ 为 ziku_table
// 取模软件，需要指定【阴码+顺向（高位在前）+行列式】的形式
// uchar code ziku_table[]={
// 0x00,0x30,0xC0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x60,0x10,/*"[",0*/
// 0x00,0x78,0x48,0x57,0x50,0x61,0x51,0x4A,0x4B,0x48,0x69,0x51,0x42,0x44,0x40,0x40,
// 0x40,0x40,0x40,0xFE,0x80,0x20,0x20,0x20,0xFC,0x20,0x28,0x24,0x22,0x22,0xA0,0x40,/*"陈",1*/
// 0x00,0xFF,0x01,0x01,0x01,0x7F,0x41,0x41,0x49,0x45,0x41,0x49,0x45,0x41,0x41,0x40,
// 0x00,0xFE,0x00,0x00,0x00,0xFC,0x04,0x04,0x44,0x24,0x04,0x44,0x24,0x04,0x14,0x08,/*"雨",2*/
// 0x00,0xFF,0x02,0x7A,0x4A,0x7A,0x00,0xFF,0x02,0x7A,0x4A,0x4A,0x7A,0x02,0x0A,0x04,
// 0x20,0x20,0x20,0x7E,0x42,0x84,0x10,0x10,0x10,0x10,0x28,0x28,0x28,0x44,0x44,0x82,/*"歌",3*/
// 0x00,0xC0,0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x20,0xC0,/*"]",4*/
// };

// 测试用的字母A的16x8点阵
// 可直接这样显示在画面上 -> setICDataToBuffer(ic_data_A, 16, 0);
unsigned char code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

unsigned long addr;
unsigned char ic_data[32];

// 芯片取到的数据转换成适合模块的数据格式并保存在大buffer中
void setICDataToBuffer(unsigned char *pICData, unsigned char size, unsigned char pos);

// 暂时只支持最前面的8个字节的显示（8个半角或4个全角）
void readICDataToBuffer(uchar* str){
	uchar pos;
	
	// GB2312-80编码的编码范围是高位0xa1－0xfe，低位是 0xa1-0xfe ，
	// 其中汉字范围为 0xb0a1 和 0xf7fe，如果只是简单地判断汉字，则只要查看高字节是否大于等于0xa1就可以了
	for (pos=0; pos<8; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			// 判定为半角ASCII码，调用ASCII码的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			// 判定为全角汉字（其实还有很多别的可能比如日文汉字，这里我们默认输入的字符串是GB2312编码）
			// 调用汉字的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(getICData_Hanzi_16x16(&str[pos]), 32, pos);
			pos++;//由于汉字占两个字节，这里手动让循环变量跳过下一个字节
		}
	}
}

// 取得点阵数据
// addr: 计算好的点阵数据的起始地址
// size：从起始地址连续取多少字节的点阵数据，目前只支持16（ASCII码）和32（GB2312汉字）两种
uchar* getICData(unsigned long addr, uchar size) {
	
	idata int idx = 0;

	//init
	pCE=1;
	pSI=0;
	pSCLK=0;
	
	//memset(ic_data, 32, 0x00);
	
	// 首先把片选信号（CS#）变为低
	pCE=0;
	
	// 紧跟着的是通过串行数据输入引脚（SI）移位输入 1 个字节的命令字（03 h）
	// 每一位在串行时钟（SCLK）上升沿被锁存。
	for (idx = 0; idx<8; idx++){
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		pSCLK=1;
	}
	
	// 然后继续通过串行数据输入引脚（SI）移位输入 3 个字节的地址
	// 每一位在串行时钟（SCLK）上升沿被锁存。
	for (idx = 0; idx<24; idx++){
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		
		// For test break point
		if(idx==23){
			idx = 23;
		}
		
		pSCLK=1;
	}
	
	// 经过仔细调试发现，芯片返回数据的时间线跟官方文档有所出入
	// 官方文档给出的时间线是传输完命令的最后一位以后的第一个SCLK的下降沿会从MO传回第一个字节的最高位，
	// 实际上，在传输命令的最后一个上升沿的瞬间，MO就已经开始往回传数据了，然后紧接着就会在每个下降沿传回后面的数据
	// 不知道为什么，但我手上的芯片确实如此，导致一开始总是缺少第一位数据
	// 所以在下面的循环之前先取第一位的数据
	
	// 奇怪，用stc12c5a60s2时，时序又没有问题了，搞不懂。。。下面两句暂时不用了。。。
	// 如果用了下面两句，表现为整个显示多网上位移一位。。。
	//ic_data[0] = ic_data[0] | pSO;
	//ic_data[0] <<= 1;
	
	// 然后芯片会将该地址的字节数据通过串行数据输出引脚（SO）移位输出，
	// 每一位在串行时钟（SCLK）下降沿被移出。
	// 由于第一位数据已经取过了，所以下面的循环从1而不是0开始
	for (idx = 0; idx<8*size ; idx++){
		pSCLK=0;
		
		// 每次产生下降沿，IC会继续返回一位点阵数据
		// 取得这一位点阵数据存放在当前字节的最低位
		// 注意，位赋值时不可以直接用或运算！！因为这种做法无法将原来的1改成0！！
		//       结果就是屏幕上最后一行会出现无规则的点，这些点都是从数组范围以外位移进来并且没有被置为0的点。（pSO=0）
		if (pSO == 0) {
			ic_data[idx/8] &= 0xFE; // 1111 1110 
		} else {
			ic_data[idx/8] |= 0x01; // 0000 0001
		}
		
		if(((idx+1)%8) > 0){
			// 进行左移以空出最低位以便继续接受下一位数据
			ic_data[idx/8] <<= 1;
		} else {
			// 每个字节只需要位移7次即可，最后一位，也就是真正的最低位的数据设置以后不再做左移
			// 否则会丢失最高位的数据，并产生点阵偏移
			//ic_data[idx/8] >>= 1;
		}
		
		pSCLK=1;
		
		//display();//test
	}
	
	// 读取字节数据后，则把片选信号（CS#）变为高，结束本次操作。
	pCE=1;
	return ic_data;
}

uchar* getICData_ASCII_8x16(uchar ch) {
	// ASCII字符所在地址的计算公式
	addr = (ch-0x20)*16+0x3b7c0;
	return getICData(addr, 16);
}

uchar* getICData_Hanzi_16x16(uchar* hz) {
	unsigned long hzGBCodeH8, hzGBCodeL8;
	
	// 由于后面有地址的计算，所以必须使用可容纳大数的long型作为中间变量，否则做地址运算时会溢出
	// 直接将uchar型赋值给long型的话，没有赋值到的高位有可能乱掉，所以跟0xFF相与一次，确保只留下uchar部分的数据
	hzGBCodeH8 = hz[0] & 0xFF;
	hzGBCodeL8 = hz[1] & 0xFF;
	
	// GB2312汉字所在地址的计算公式
	if(hzGBCodeH8 ==0xA9 && hzGBCodeL8 >=0xA1)
		addr = (282 + (hzGBCodeL8 - 0xA1 ))*32;
	else if(hzGBCodeH8 >=0xA1 && hzGBCodeH8 <= 0xA3 && hzGBCodeL8 >=0xA1)
		addr =( (hzGBCodeH8 - 0xA1) * 94 + (hzGBCodeL8 - 0xA1))*32;
	else if(hzGBCodeH8 >=0xB0 && hzGBCodeH8 <= 0xF7 && hzGBCodeL8 >=0xA1) {
		// addr = ((hzGBCodeH8 - 0xB0) * 94 + (hzGBCodeL8 - 0xA1)+ 846)*32;
		addr = (hzGBCodeH8 - 0xB0) * 94;
		addr += (hzGBCodeL8 - 0xA1) + 846;
		addr *= 32;
	}
	
	return getICData(addr, 32);
}

// 取得指定文字的指定列的点阵数据（两个字节）
// 返回参数结构：
//    colData[0] : 返回是否为Asc/汉字标志位。0为Ascii,1为汉字。
//    colData[1] : 返回指定列 上半部分数据
//    colData[2] : 返回指定列 下半部分数据
void getICData_Col(uchar* str, uchar colIdx, uchar* colData)
{
	idata unsigned long hzGBCodeH8, hzGBCodeL8;

	// 先清空目标字节
	colData[0] = 0x00; //返回是否为Asc/汉字标志位。0为Ascii,1为汉字。 
	colData[1] = 0x00; //返回指定列 上半部分数据
	colData[2] = 0x00; //返回指定列 下半部分数据

	// 计算点阵所在地址
	// GB2312-80编码的编码范围是高位0xa1－0xfe，低位是 0xa1-0xfe ，
	// 其中汉字范围为 0xb0a1 和 0xf7fe，如果只是简单地判断汉字，则只要查看高字节是否大于等于0xa1就可以了
	if (str[0] >= 0x20 && str[0] <= 0x7E) {
		// ASCII字符
		addr = (str[0]-0x20)*16+0x3b7c0;

		// 只需要指定列的2个字节，分两次取单字节
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+8, 1)[0];

		colData[0] = 0;
	} else {
		// 全角汉字
		// 由于后面有地址的计算，所以必须使用可容纳大数的long型作为中间变量，否则做地址运算时会溢出
		// 直接将uchar型赋值给long型的话，没有赋值到的高位有可能乱掉，所以跟0xFF相与一次，确保只留下uchar部分的数据
		hzGBCodeH8 = str[0] & 0xFF;
		hzGBCodeL8 = str[1] & 0xFF;

		// GB2312汉字所在地址的计算公式
		if(hzGBCodeH8 ==0xA9 && hzGBCodeL8 >=0xA1)
			addr = (282 + (hzGBCodeL8 - 0xA1 ))*32;
		else if(hzGBCodeH8 >=0xA1 && hzGBCodeH8 <= 0xA3 && hzGBCodeL8 >=0xA1)
			addr =( (hzGBCodeH8 - 0xA1) * 94 + (hzGBCodeL8 - 0xA1))*32;
		else if(hzGBCodeH8 >=0xB0 && hzGBCodeH8 <= 0xF7 && hzGBCodeL8 >=0xA1) {
			// addr = ((hzGBCodeH8 - 0xB0) * 94 + (hzGBCodeL8 - 0xA1)+ 846)*32;
			addr = (hzGBCodeH8 - 0xB0) * 94;
			addr += (hzGBCodeL8 - 0xA1) + 846;
			addr *= 32;
		}

		// 只需要整个汉字指定列的2个字节，分两次取单字节
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+16, 1)[0];

		colData[0] = 1;
	}
}
