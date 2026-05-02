#pragma once

//--------------------------------------------------
// HCA（高压缩音频）类
//--------------------------------------------------
class clHCA {
public:
	clHCA(unsigned int ciphKey1 = 0xE0748978, unsigned int ciphKey2 = 0xCF222F1F);

	// HCA检查
	static bool CheckFile(void *data, unsigned int size);

	// 校验和
	static unsigned short CheckSum(void *data, int size, unsigned short sum = 0);

	// 输出信息到控制台
	bool PrintInfo(const char *filenameHCA);

	// 解密
	bool Decrypt(const char *filenameHCA);

	// 解码并保存为 WAVE 文件
	bool DecodeToWavefile(const char *filenameHCA, const char *filenameWAV, float volume = 1, int mode = 16, int loop = 0);
	bool DecodeToWavefileStream(void *fpHCA, const char *filenameWAV, float volume = 1, int mode = 16, int loop = 0);

	// 编码并保存为 HCA 文件
	//bool EncodeFromWavefile(const char *filenameWAV,const char *filenameHCA,float volume=1);
	//bool EncodeFromWavefileStream(void *fpWAV,const char *filenameHCA,float volume=1);

private:
	struct stHeader {// 文件信息（必填）
		unsigned int hca;              // 'HCA'
		unsigned short version;        // 版本，已确认存在 v1.3 和 v2.0 版本。
		unsigned short dataOffset;     // 数据偏移
	};
	struct stFormat {// 格式信息（必填）
		unsigned int fmt;              // 'fmt'
		unsigned int channelCount : 8;   // 通道数量：1-16
		unsigned int samplingRate : 24;  // 采样率 1 ~ 0x7FFFFF
		unsigned int blockCount;       // 块数量：0 个或更多
		unsigned short muteHeader;     // 开头静音部分（块数 * 0x400 + 0x80）
		unsigned short muteFooter;     // 结尾静音部分
	};
	struct stCompress {// 压缩信息（需要提供压缩信息或解码信息）
		unsigned int comp;             // 'comp'
		unsigned short blockSize;      // 块大小（对 CBR 有效？）：8 ~ 0xFFFF，VBR = 0。
		unsigned char r01;             // 未知 (1) 0-r02	v2.0 目前仅支持版本 1
		unsigned char r02;             // 未知 (15) r01~0x1F	v2.0 目前仅支持版本 15
		unsigned char r03;             // 未知 (1) (1)
		unsigned char r04;             // 未知 (1) (0)
		unsigned char r05;             // 未知 (0x80) (0x80)
		unsigned char r06;             // 未知 (0x80) (0x20)
		unsigned char r07;             // 未知 (0) (0x20)
		unsigned char r08;             // 未知 (0) (8)
		unsigned char reserve1;        // 预留
		unsigned char reserve2;        // 预留
	};
	struct stDecode {//解码信息（需要压缩信息或解码信息）
		unsigned int dec;              // 'dec'
		unsigned short blockSize;      // 块大小（对 CBR 有效？）：8 ~ 0xFFFF，VBR = 0。
		unsigned char r01;             // 未知 (1) 0-r02	v2.0 目前仅支持版本 1
		unsigned char r02;             // 未知 (15) r01~0x1F	v2.0 目前仅支持版本 15
		unsigned char count1;          // type0和type1 - 1
		unsigned char count2;          // type2 - 1
		unsigned char r03 : 4;         // 未知 (0)
		unsigned char r04 : 4;         // 未知 (0) 0 将被更正为 1。
		unsigned char enableCount2;    // count2使能
	};
	struct stVBR {// 可变比特率信息（已弃用？）
		unsigned int vbr;              // 'vbr'
		unsigned short r01;            // 未知 0~0x1FF
		unsigned short r02;            // 未知
	};
	struct stATH {//ATH 表格信息（已从 v2.0 版本开始弃用？）
		unsigned int ath;              // 'ath'
		unsigned short type;           // 类型（0：全部 0，1：表格 1）
	};
	struct stLoop {//循环信息
		unsigned int loop;             // 'loop'
		unsigned int start;            // 循环起始块索引 0～loopEnd
		unsigned int end;              // 循环终止块索引 loopStart~(stFormat::blockCount-1)
		unsigned short count;          // 循环计数为 0x80 会导致无限循环。
		unsigned short r01;            // 未知 (0x226) 
	};
	struct stCipher {//密码表信息
		unsigned int ciph;             // 'ciph'
		unsigned short type;           // 加密类型（0：无加密，1：无密钥加密，0x38：密钥加密）
	};
	struct stRVA {//相对音量调整信息
		unsigned int rva;              // 'rva'
		float volume;                  // 音量
	};
	struct stComment {//评论信息
		unsigned int comm;             // 'comm'
		unsigned char len;             // 评论长度？
																	 //char comment[];
	};
	struct stPadding {//填充
		unsigned int pad;              // 'pad'
	};
	unsigned int _version;
	unsigned int _dataOffset;
	unsigned int _channelCount;
	unsigned int _samplingRate;
	unsigned int _blockCount;
	unsigned int _muteHeader;
	unsigned int _muteFooter;
	unsigned int _blockSize;
	unsigned int _comp_r01;
	unsigned int _comp_r02;
	unsigned int _comp_r03;
	unsigned int _comp_r04;
	unsigned int _comp_r05;
	unsigned int _comp_r06;
	unsigned int _comp_r07;
	unsigned int _comp_r08;
	unsigned int _comp_r09;
	unsigned int _vbr_r01;
	unsigned int _vbr_r02;
	unsigned int _ath_type;
	unsigned int _loopStart;
	unsigned int _loopEnd;
	unsigned int _loopCount;
	unsigned int _loop_r01;
	bool _loopFlg;
	unsigned int _ciph_type;
	unsigned int _ciph_key1;
	unsigned int _ciph_key2;
	float _rva_volume;
	unsigned int _comm_len;
	char *_comm_comment;
	class clATH {
	public:
		clATH();
		bool Init(int type, unsigned int key);
		unsigned char *GetTable(void);
	private:
		unsigned char _table[0x80];
		void Init0(void);
		void Init1(unsigned int key);
	}_ath;
	class clCipher {
	public:
		clCipher();
		bool Init(int type, unsigned int key1, unsigned int key2);
		void Mask(void *data, int size);
	private:
		unsigned char _table[0x100];
		void Init0(void);
		void Init1(void);
		void Init56(unsigned int key1, unsigned int key2);
		void Init56_CreateTable(unsigned char *table, unsigned char key);
	}_cipher;
	class clData {
	public:
		clData(void *data, int size);
		int CheckBit(int bitSize);
		int GetBit(int bitSize);
		void AddBit(int bitSize);
	private:
		unsigned char *_data;
		int _size;
		int _bit;
	};
	struct stChannel {
		float block[0x80];
		float base[0x80];
		char value[0x80];
		char scale[0x80];
		char value2[8];
		int type;
		char *value3;
		unsigned int count;
		float wav1[0x80];
		float wav2[0x80];
		float wav3[0x80];
		float wave[8][0x80];
		void Decode1(clData *data, unsigned int a, int b, unsigned char *ath);
		void Decode2(clData *data);
		void Decode3(unsigned int a, unsigned int b, unsigned int c, unsigned int d);
		void Decode4(int index, unsigned int a, unsigned int b, unsigned int c);
		void Decode5(int index);
	}_channel[0x10];
	bool Decode(void *data, unsigned int size, unsigned int address);
	bool DecodeToWavefile_Decode(void *fp1, void *fp2, unsigned int address, unsigned int count, void *data, void *modeFunction);
	static void DecodeToWavefile_DecodeModeFloat(float f, void *fp);
	static void DecodeToWavefile_DecodeMode8bit(float f, void *fp);
	static void DecodeToWavefile_DecodeMode16bit(float f, void *fp);
	static void DecodeToWavefile_DecodeMode24bit(float f, void *fp);
	static void DecodeToWavefile_DecodeMode32bit(float f, void *fp);
};
