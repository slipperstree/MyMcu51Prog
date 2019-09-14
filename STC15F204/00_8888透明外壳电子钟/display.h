#define uchar unsigned char
#define uint unsigned int

// 数码管引脚连接 位
sbit A1 = P3^2;
sbit A2 = P3^3;
sbit A3 = P3^4;
sbit A4 = P3^5;

// 数码管引脚连接 段
sbit segA = P2^0;
sbit segB = P2^1;
sbit segC = P2^2;
sbit segD = P2^3;
sbit segE = P2^4;
sbit segF = P2^5;
sbit segG = P2^6;
sbit segDP = P2^7;

//第124位用数字显示code
uchar code digit124[17]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F
//第3位数码管在硬件上是倒过来接的，数字显示code不一样，要改
uchar code digit3[17]={~0x3f,~0x30,~0x5b,~0x79,~0x74,~0x6d,~0x6f,~0x38,~0x7f,~0x7d, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F(wrong)

int digitForShow;
uchar d1 = 0;
uchar d2 = 0;
uchar d3 = 0;
uchar d4 = 0;

void showPosition(uchar pos){

	// 共阳，位选高电平点亮（用的是三极管驱动，IO输出0，位选呈高电平）
	// 共阳，段码低电平点亮

	// 防止残影，先清屏，关闭所有显示
	A4 = A2 = A3 = A1 = 1;

	switch (pos)
	{
	case 1:
		P2 = digit124[d1];
		A1 = 0;
		break;
	case 2:
		P2 = digit124[d2] & 0x7f; // & 7f 的作用是将dp位置0，显示小数点
		A2 = 0;
		break;
	case 3:
		P2 = digit3[d3] & 0x7f; // & 7f 的作用是将dp位置0，显示小数点
		A3 = 0;
		break;
	case 4:
		P2 = digit124[d4];
		A4 = 0;
		break;
	default:
		break;
	}
}

void updateDisplay() {
	digitForShow = miao;
		
	d1 = shi % 100 / 10;
	d2 = shi % 10;			// 最后的|0x80是为了显示dp
	d3 = fen % 100 / 10;	// 最后的|0x80是为了显示dp
	d4 = fen % 10;
}