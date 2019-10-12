#include "../header/common.h"
#include "../header/GT20L16S1Y.h"

sbit pCE        =P2^0;         // Ƭѡ
sbit pSCLK      =P2^1;         // ʱ��
sbit pSI        =P2^2;         // �������루��Ƭ��->�ֿ�оƬ��
sbit pSO        =P2^3;         // ����������ֿ�оƬ->��Ƭ����

// ��ʹ���ֿ�оƬ��ʹ�ù̶�����ʱ�ſ���һ�Σ����޸ĳ������ bufHZ Ϊ ziku_table
// ȡģ�������Ҫָ��������+˳�򣨸�λ��ǰ��+����ʽ������ʽ
// uchar code ziku_table[]={
// 0x00,0x30,0xC0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x60,0x10,/*"[",0*/
// 0x00,0x78,0x48,0x57,0x50,0x61,0x51,0x4A,0x4B,0x48,0x69,0x51,0x42,0x44,0x40,0x40,
// 0x40,0x40,0x40,0xFE,0x80,0x20,0x20,0x20,0xFC,0x20,0x28,0x24,0x22,0x22,0xA0,0x40,/*"��",1*/
// 0x00,0xFF,0x01,0x01,0x01,0x7F,0x41,0x41,0x49,0x45,0x41,0x49,0x45,0x41,0x41,0x40,
// 0x00,0xFE,0x00,0x00,0x00,0xFC,0x04,0x04,0x44,0x24,0x04,0x44,0x24,0x04,0x14,0x08,/*"��",2*/
// 0x00,0xFF,0x02,0x7A,0x4A,0x7A,0x00,0xFF,0x02,0x7A,0x4A,0x4A,0x7A,0x02,0x0A,0x04,
// 0x20,0x20,0x20,0x7E,0x42,0x84,0x10,0x10,0x10,0x10,0x28,0x28,0x28,0x44,0x44,0x82,/*"��",3*/
// 0x00,0xC0,0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x20,0xC0,/*"]",4*/
// };

// �����õ���ĸA��16x8����
// ��ֱ��������ʾ�ڻ����� -> setICDataToBuffer(ic_data_A, 16, 0);
unsigned char code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

unsigned long addr;
unsigned char ic_data[32];

// оƬȡ��������ת�����ʺ�ģ������ݸ�ʽ�������ڴ�buffer��
void setICDataToBuffer(unsigned char *pICData, unsigned char size, unsigned char pos);

// ��ʱֻ֧����ǰ���8���ֽڵ���ʾ��8����ǻ�4��ȫ�ǣ�
void readICDataToBuffer(uchar* str){
	uchar pos;
	
	// GB2312-80����ı��뷶Χ�Ǹ�λ0xa1��0xfe����λ�� 0xa1-0xfe ��
	// ���к��ַ�ΧΪ 0xb0a1 �� 0xf7fe�����ֻ�Ǽ򵥵��жϺ��֣���ֻҪ�鿴���ֽ��Ƿ���ڵ���0xa1�Ϳ�����
	for (pos=0; pos<8; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			// �ж�Ϊ���ASCII�룬����ASCII���ȡ������������ڵ�ǰ����ʾλ����
			setICDataToBuffer(getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			// �ж�Ϊȫ�Ǻ��֣���ʵ���кܶ��Ŀ��ܱ������ĺ��֣���������Ĭ��������ַ�����GB2312���룩
			// ���ú��ֵ�ȡ������������ڵ�ǰ����ʾλ����
			setICDataToBuffer(getICData_Hanzi_16x16(&str[pos]), 32, pos);
			pos++;//���ں���ռ�����ֽڣ������ֶ���ѭ������������һ���ֽ�
		}
	}
}

// ȡ�õ�������
// addr: ����õĵ������ݵ���ʼ��ַ
// size������ʼ��ַ����ȡ�����ֽڵĵ������ݣ�Ŀǰֻ֧��16��ASCII�룩��32��GB2312���֣�����
uchar* getICData(unsigned long addr, uchar size) {
	
	idata int idx = 0;

	//init
	pCE=1;
	pSI=0;
	pSCLK=0;
	
	//memset(ic_data, 32, 0x00);
	
	// ���Ȱ�Ƭѡ�źţ�CS#����Ϊ��
	pCE=0;
	
	// �����ŵ���ͨ�����������������ţ�SI����λ���� 1 ���ֽڵ������֣�03 h��
	// ÿһλ�ڴ���ʱ�ӣ�SCLK�������ر����档
	for (idx = 0; idx<8; idx++){
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		pSCLK=1;
	}
	
	// Ȼ�����ͨ�����������������ţ�SI����λ���� 3 ���ֽڵĵ�ַ
	// ÿһλ�ڴ���ʱ�ӣ�SCLK�������ر����档
	for (idx = 0; idx<24; idx++){
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		
		// For test break point
		if(idx==23){
			idx = 23;
		}
		
		pSCLK=1;
	}
	
	// ������ϸ���Է��֣�оƬ�������ݵ�ʱ���߸��ٷ��ĵ���������
	// �ٷ��ĵ�������ʱ�����Ǵ�������������һλ�Ժ�ĵ�һ��SCLK���½��ػ��MO���ص�һ���ֽڵ����λ��
	// ʵ���ϣ��ڴ�����������һ�������ص�˲�䣬MO���Ѿ���ʼ���ش������ˣ�Ȼ������žͻ���ÿ���½��ش��غ��������
	// ��֪��Ϊʲô���������ϵ�оƬȷʵ��ˣ�����һ��ʼ����ȱ�ٵ�һλ����
	// �����������ѭ��֮ǰ��ȡ��һλ������
	
	// ��֣���stc12c5a60s2ʱ��ʱ����û�������ˣ��㲻������������������ʱ�����ˡ�����
	// ��������������䣬����Ϊ������ʾ������λ��һλ������
	//ic_data[0] = ic_data[0] | pSO;
	//ic_data[0] <<= 1;
	
	// Ȼ��оƬ�Ὣ�õ�ַ���ֽ�����ͨ����������������ţ�SO����λ�����
	// ÿһλ�ڴ���ʱ�ӣ�SCLK���½��ر��Ƴ���
	// ���ڵ�һλ�����Ѿ�ȡ���ˣ����������ѭ����1������0��ʼ
	for (idx = 0; idx<8*size ; idx++){
		pSCLK=0;
		
		// ÿ�β����½��أ�IC���������һλ��������
		// ȡ����һλ�������ݴ���ڵ�ǰ�ֽڵ����λ
		// ע�⣬λ��ֵʱ������ֱ���û����㣡����Ϊ���������޷���ԭ����1�ĳ�0����
		//       ���������Ļ�����һ�л�����޹���ĵ㣬��Щ�㶼�Ǵ����鷶Χ����λ�ƽ�������û�б���Ϊ0�ĵ㡣��pSO=0��
		if (pSO == 0) {
			ic_data[idx/8] &= 0xFE; // 1111 1110 
		} else {
			ic_data[idx/8] |= 0x01; // 0000 0001
		}
		
		if(((idx+1)%8) > 0){
			// ���������Կճ����λ�Ա����������һλ����
			ic_data[idx/8] <<= 1;
		} else {
			// ÿ���ֽ�ֻ��Ҫλ��7�μ��ɣ����һλ��Ҳ�������������λ�����������Ժ���������
			// ����ᶪʧ���λ�����ݣ�����������ƫ��
			//ic_data[idx/8] >>= 1;
		}
		
		pSCLK=1;
		
		//display();//test
	}
	
	// ��ȡ�ֽ����ݺ����Ƭѡ�źţ�CS#����Ϊ�ߣ��������β�����
	pCE=1;
	return ic_data;
}

uchar* getICData_ASCII_8x16(uchar ch) {
	// ASCII�ַ����ڵ�ַ�ļ��㹫ʽ
	addr = (ch-0x20)*16+0x3b7c0;
	return getICData(addr, 16);
}

uchar* getICData_Hanzi_16x16(uchar* hz) {
	unsigned long hzGBCodeH8, hzGBCodeL8;
	
	// ���ں����е�ַ�ļ��㣬���Ա���ʹ�ÿ����ɴ�����long����Ϊ�м��������������ַ����ʱ�����
	// ֱ�ӽ�uchar�͸�ֵ��long�͵Ļ���û�и�ֵ���ĸ�λ�п����ҵ������Ը�0xFF����һ�Σ�ȷ��ֻ����uchar���ֵ�����
	hzGBCodeH8 = hz[0] & 0xFF;
	hzGBCodeL8 = hz[1] & 0xFF;
	
	// GB2312�������ڵ�ַ�ļ��㹫ʽ
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

// ȡ��ָ�����ֵ�ָ���еĵ������ݣ������ֽڣ�
// ���ز����ṹ��
//    colData[0] : �����Ƿ�ΪAsc/���ֱ�־λ��0ΪAscii,1Ϊ���֡�
//    colData[1] : ����ָ���� �ϰ벿������
//    colData[2] : ����ָ���� �°벿������
void getICData_Col(uchar* str, uchar colIdx, uchar* colData)
{
	idata unsigned long hzGBCodeH8, hzGBCodeL8;

	// �����Ŀ���ֽ�
	colData[0] = 0x00; //�����Ƿ�ΪAsc/���ֱ�־λ��0ΪAscii,1Ϊ���֡� 
	colData[1] = 0x00; //����ָ���� �ϰ벿������
	colData[2] = 0x00; //����ָ���� �°벿������

	// ����������ڵ�ַ
	// GB2312-80����ı��뷶Χ�Ǹ�λ0xa1��0xfe����λ�� 0xa1-0xfe ��
	// ���к��ַ�ΧΪ 0xb0a1 �� 0xf7fe�����ֻ�Ǽ򵥵��жϺ��֣���ֻҪ�鿴���ֽ��Ƿ���ڵ���0xa1�Ϳ�����
	if (str[0] >= 0x20 && str[0] <= 0x7E) {
		// ASCII�ַ�
		addr = (str[0]-0x20)*16+0x3b7c0;

		// ֻ��Ҫָ���е�2���ֽڣ�������ȡ���ֽ�
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+8, 1)[0];

		colData[0] = 0;
	} else {
		// ȫ�Ǻ���
		// ���ں����е�ַ�ļ��㣬���Ա���ʹ�ÿ����ɴ�����long����Ϊ�м��������������ַ����ʱ�����
		// ֱ�ӽ�uchar�͸�ֵ��long�͵Ļ���û�и�ֵ���ĸ�λ�п����ҵ������Ը�0xFF����һ�Σ�ȷ��ֻ����uchar���ֵ�����
		hzGBCodeH8 = str[0] & 0xFF;
		hzGBCodeL8 = str[1] & 0xFF;

		// GB2312�������ڵ�ַ�ļ��㹫ʽ
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

		// ֻ��Ҫ��������ָ���е�2���ֽڣ�������ȡ���ֽ�
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+16, 1)[0];

		colData[0] = 1;
	}
}
