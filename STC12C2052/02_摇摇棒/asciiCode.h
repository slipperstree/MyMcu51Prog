// ------------------  ASCII字模的数据表 ------------------------ //
// 码表从0x20~0x7e                                                //
// 字库: C:\Users\Slipperstree\Downloads\DoYoungStudio_XQ_MakeWords\lcd汉字取模软件\Asc5x8E.dot 纵向取模上高位//
// -------------------------------------------------------------- //
unsigned char code nAsciiDot[] =              // ASCII
{
	0x00,0x00,0x00,0x00,0x00, // - -

	0x00,0x00,0xFA,0x00,0x00, // -!-

	0x00,0xE0,0x00,0xE0,0x00, // -"-

	0x28,0xFE,0x28,0xFE,0x28, // -#-

	0x24,0x74,0xDE,0x54,0x48, // -$-

	0xC4,0xC8,0x10,0x26,0x46, // -%-

	0x6C,0x92,0x6A,0x04,0x0A, // -&-

	0x00,0x20,0xC0,0x80,0x00, // -'-

	0x00,0x38,0x44,0x82,0x00, // -(-

	0x00,0x82,0x44,0x38,0x00, // -)-

	0x44,0x28,0xFE,0x28,0x44, // -*-

	0x10,0x10,0xFE,0x10,0x10, // -+-

	0x02,0x0C,0x08,0x00,0x00, // -,-

	0x10,0x10,0x10,0x10,0x10, // ---

	0x00,0x06,0x06,0x00,0x00, // -.-

	0x04,0x08,0x10,0x20,0x40, // -/-

	0x7C,0x8A,0x92,0xA2,0x7C, // -0-

	0x00,0x42,0xFE,0x02,0x00, // -1-

	0x46,0x8A,0x92,0x92,0x62, // -2-

	0x84,0x82,0x92,0xB2,0xCC, // -3-

	0x18,0x28,0x48,0xFE,0x08, // -4-

	0xE4,0xA2,0xA2,0xA2,0x9C, // -5-

	0x3C,0x52,0x92,0x92,0x8C, // -6-

	0x80,0x8E,0x90,0xA0,0xC0, // -7-

	0x6C,0x92,0x92,0x92,0x6C, // -8-

	0x62,0x92,0x92,0x94,0x78, // -9-

	0x00,0x6C,0x6C,0x00,0x00, // -:-

	0x02,0x6C,0x6C,0x00,0x00, // -;-

	0x10,0x28,0x44,0x82,0x00, // -<-

	0x28,0x28,0x28,0x28,0x28, // -=-

	0x00,0x82,0x44,0x28,0x10, // ->-

	0x40,0x80,0x9A,0xA0,0x40, // -?-

	0x7C,0x82,0xBA,0xAA,0x7A, // -@-

	0x3E,0x48,0x88,0x48,0x3E, // -A-

	0xFE,0x92,0x92,0x92,0x6C, // -B-

	0x7C,0x82,0x82,0x82,0x44, // -C-

	0xFE,0x82,0x82,0x82,0x7C, // -D-

	0xFE,0x92,0x92,0x92,0x82, // -E-

	0xFE,0x90,0x90,0x90,0x80, // -F-

	0x7C,0x82,0x8A,0x8A,0x4E, // -G-

	0xFE,0x10,0x10,0x10,0xFE, // -H-

	0x00,0x82,0xFE,0x82,0x00, // -I-

	0x04,0x02,0x82,0xFC,0x80, // -J-

	0xFE,0x10,0x28,0x44,0x82, // -K-

	0xFE,0x02,0x02,0x02,0x02, // -L-

	0xFE,0x40,0x30,0x40,0xFE, // -M-

	0xFE,0x20,0x10,0x08,0xFE, // -N-

	0x7C,0x82,0x82,0x82,0x7C, // -O-

	0xFE,0x90,0x90,0x90,0x60, // -P-

	0x7C,0x82,0x8A,0x84,0x7A, // -Q-

	0xFE,0x90,0x98,0x94,0x62, // -R-

	0x64,0x92,0x92,0x92,0x4C, // -S-

	0x80,0x80,0xFE,0x80,0x80, // -T-

	0xFC,0x02,0x02,0x02,0xFC, // -U-

	0xF8,0x04,0x02,0x04,0xF8, // -V-

	0xFE,0x04,0x18,0x04,0xFE, // -W-

	0xC6,0x28,0x10,0x28,0xC6, // -X-

	0xC0,0x20,0x1E,0x20,0xC0, // -Y-

	0x86,0x8A,0x92,0xA2,0xC2, // -Z-

	0xFE,0xFE,0x82,0x82,0x00, // -[-

	0x40,0x20,0x10,0x08,0x04, // -\-

	0x00,0x82,0x82,0xFE,0xFE, // -]-

	0x20,0x40,0xFE,0x40,0x20, // -^-

	0x10,0x38,0x54,0x10,0x10, // -_-

	0x00,0x00,0x80,0x40,0x20, // -`-

	0x24,0x2A,0x2A,0x1C,0x02, // -a-

	0xFE,0x14,0x22,0x22,0x1C, // -b-

	0x1C,0x22,0x22,0x22,0x10, // -c-

	0x1C,0x22,0x22,0x14,0xFE, // -d-

	0x1C,0x2A,0x2A,0x2A,0x10, // -e-

	0x10,0x7E,0x90,0x90,0x40, // -f-

	0x19,0x25,0x25,0x25,0x1E, // -g-

	0xFE,0x10,0x20,0x20,0x1E, // -h-

	0x00,0x00,0x9E,0x00,0x00, // -i-

	0x00,0x01,0x11,0x9E,0x00, // -j-

	0xFE,0x08,0x14,0x22,0x02, // -k-

	0x00,0x82,0xFE,0x02,0x00, // -l-

	0x1E,0x20,0x1E,0x20,0x1E, // -m-

	0x20,0x1E,0x20,0x20,0x1E, // -n-

	0x1C,0x22,0x22,0x22,0x1C, // -o-

	0x3F,0x24,0x24,0x24,0x18, // -p-

	0x18,0x24,0x24,0x24,0x3F, // -q-

	0x20,0x1E,0x20,0x20,0x10, // -r-

	0x12,0x2A,0x2A,0x2A,0x24, // -s-

	0x20,0xFC,0x22,0x22,0x24, // -t-

	0x3C,0x02,0x02,0x3C,0x02, // -u-

	0x38,0x04,0x02,0x04,0x38, // -v-

	0x3C,0x02,0x3C,0x02,0x3C, // -w-

	0x22,0x14,0x08,0x14,0x22, // -x-

	0x39,0x05,0x05,0x09,0x3E, // -y-

	0x22,0x26,0x2A,0x32,0x22, // -z-

	0x10,0x6C,0x82,0x00,0x00, // -{-

	0x00,0x00,0xEE,0x00,0x00, // -|-

	0x00,0x00,0x82,0x6C,0x10, // -}-

	0x10,0x20,0x10,0x08,0x10, // -~-

	0xAA,0x54,0xAA,0x54,0xAA, // --
};
