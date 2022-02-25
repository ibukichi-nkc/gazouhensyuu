#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string.h>

#define FILE_HEADER_SIZE 14                // ファイルヘッダのサイズ
#define INFO_HEADER_SIZE 40                // 情報ヘッダのサイズ

using namespace std;

/*
* ファイルヘッダー構造体
*/
typedef struct FileHeader
{
	uint8_t data[FILE_HEADER_SIZE];  // 加工前データ（書き出しのため必要）
	string fileType;                 // ファイルタイプ
	int fileSize;                    // ファイルサイズ
} FileHeader;

/*
* 情報ヘッダー構造体
*/
typedef struct InfoHeader
{
	uint8_t data[INFO_HEADER_SIZE];  // 加工前データ（書き出しのため必要）
	int infoHeaderSize;              // 情報ヘッダのサイズ
	int width;                       // 画像の幅
	int height;                      // 画像の高さ
	int clrPerPixel;                 // 1ピクセル当たりの色数
	int dataSize;                    // 画像データのサイズ
} InfoHeader;

/*
* ピクセルの色情報構造体
*/
typedef struct Color
{
	int r;                // 赤成分
	int g;                // 緑成分
	int b;                // 青成分
} Color;

int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

/*
* ビットマップ処理クラス
*/
class BitMapProcessor
{
	FILE* bmp;               // ビットマップのファイルポインタ
	uint8_t* img;            // ビットマップデータ（加工用）
	uint8_t* org;            // ビットマップデータ（読み込み時）
	FileHeader fHeader;      // ファイルヘッダ
	InfoHeader iHeader;      // 情報ヘッダ

public:
	BitMapProcessor()
	{
		bmp = NULL;
		img = NULL;
		org = NULL;
	};

	~BitMapProcessor()
	{
		if (bmp != NULL)
		{
			fclose(bmp);
		}
		delete[]img;
		delete[]org;
	}
	void loadData(string filename);
	void dispBmpInfo();
	void writeData(int i, string filename, int n);
	Color getColor(int row, int col);
	void setColor(int row, int col, int r, int g, int b);
	void restore();
	int height() { return iHeader.height; };
	int width() { return iHeader.width; };
private:
	void readFileHeader();
	void readInfoHeader();
	void readBmpData();
	void takasa();
	void yoko();

};

/*
* 4ビット情報をInt整数値に変換
*/
int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	return b1 +
		b2 * 256 +
		b3 * 256 * 256 +
		b4 * 256 * 256 * 256;
}

/*
* ビットマップデータをロードする
*/
void BitMapProcessor::loadData(string filename)
{
	if (bmp != NULL)
	{
		fclose(bmp);
	}

	bmp = fopen(filename.c_str(), "rb");
	if (bmp == NULL)
	{
		printf("ファイルオープンに失敗しました。\n");
	}

	readFileHeader();
	readInfoHeader();
	readBmpData();
}

/*
* ファイルヘッダを読む
*/
void BitMapProcessor::readFileHeader()
{
	uint8_t data[FILE_HEADER_SIZE];
	size_t size = fread(data, sizeof(uint8_t), FILE_HEADER_SIZE, bmp);

	memcpy(fHeader.data, data, sizeof(data));
	fHeader.fileType = "";
	fHeader.fileType += data[0];
	fHeader.fileType += data[1];
	fHeader.fileSize = bit2Integer(data[2], data[3], data[4], data[5]);
}

/*
* 情報ヘッダを読む
*/
void BitMapProcessor::readInfoHeader()
{
	uint8_t data[INFO_HEADER_SIZE];
	size_t size = fread(data, sizeof(uint8_t), INFO_HEADER_SIZE, bmp);

	memcpy(iHeader.data, data, sizeof(data));
	iHeader.infoHeaderSize = bit2Integer(data[0], data[1], data[2], data[3]);
	iHeader.width = bit2Integer(data[4], data[5], data[6], data[7]);
	iHeader.height = bit2Integer(data[8], data[9], data[10], data[11]);
	iHeader.clrPerPixel = bit2Integer(data[14], data[15], 0, 0);
	iHeader.dataSize = bit2Integer(data[20], data[21], data[22], data[23]);
}

/*
* 画像データを読む
*/
void BitMapProcessor::readBmpData()
{
	if (img != NULL)
	{
		delete[]img;
	}

	int sz;
	sz = iHeader.dataSize;
	img = new uint8_t[sz];
	size_t size = fread(img, sizeof(uint8_t), sz, bmp);
	if (size != sz)
	{
		printf("画像データ読み込みのサイズが矛盾しています。\n");
	}

	// バックアップ用にorgに画像データをコピー
	if (org != NULL)
	{
		delete[]org;
	}
	org = new uint8_t[sz];
	memcpy(org, img, sz);
}

/*
* ビットマップ情報の表示（デバッグ用）
*/
void BitMapProcessor::dispBmpInfo()
{
	cout << "■ファイルヘッダ情報" << endl;
	cout << "ファイルタイプ: " << fHeader.fileType << endl;
	cout << "ファイルサイズ: " << fHeader.fileSize << endl;

	cout << "■情報ヘッダ情報" << endl;
	cout << "情報ヘッダサイズ: " << iHeader.infoHeaderSize << endl;
	cout << "画像幅: " << iHeader.width << endl;
	cout << "画像高: " << iHeader.height << endl;
	cout << "１ピクセルあたりの色数: " << iHeader.clrPerPixel << endl;
	cout << "画像データのサイズ: " << iHeader.dataSize << endl;
}

/*
* ビットマップデータのファイル書き出し
*/
void BitMapProcessor::writeData(int i, string filename, int n)
{
	char s[5];
	char str[15];
	int j=0;
	switch (i)
	{
	case 1:
		strcpy(s, "mono");
		j++;
		break;
	case 2:
		strcpy(s, "tori");
		j++;
		break;
	case 3:
		strcpy(s, "hann");
		j++;
		break;
	case 4:
		strcpy(s, "moza");
		j++;
		break;
	default:
		break;
	}
	sprintf(str, "%s%d%s", s, n, filename.c_str());
	FILE* out = fopen(str, "wb");

	if (out == NULL)
	{
		printf("書き出し先のファイルを開けません。\n");
	}

	fwrite(fHeader.data, sizeof(uint8_t), FILE_HEADER_SIZE, out);
	fwrite(iHeader.data, sizeof(uint8_t), INFO_HEADER_SIZE, out);
	fwrite(img, sizeof(uint8_t), iHeader.dataSize, out);

	fclose(out);
}

/*
* 指定されたピクセルの色を取得
*/
Color BitMapProcessor::getColor(int row, int col)
{
	int width;
	int bPos;
	int gPos;
	int rPos;

	if (row < 0 || row >= iHeader.height)
	{
		printf("getColor(): rowが範囲外です。\n");
	}

	if (col < 0 || col >= iHeader.width)
	{
		printf("getColor(): colが範囲外です。\n");
	}

	width = 3 * iHeader.width;
	while (width % 4)        // ビットマップの1列は4の倍数ビットからなる
	{
		++width;
	}

	bPos = row * width + 3 * col;
	gPos = bPos + 1;
	rPos = bPos + 2;

	Color color;
	color.r = img[rPos];
	color.g = img[gPos];
	color.b = img[bPos];

	return color;
}

/*
* 指定されたピクセルに色を設定
*/
void BitMapProcessor::setColor(int row, int col, int r, int g, int b)
{
	if (row < 0 || row >= iHeader.height)
	{
		printf("getColor(): rowが範囲外です。\n");
	}
	if (col < 0 || col >= iHeader.width)
	{
		printf("getColor(): colが範囲外です。\n");
	}

	int width;
	width = 3 * iHeader.width;
	while (width % 4)        // ビットマップの1列は4の倍数ビットからなる
	{
		++width;
	}

	int bPos = row * width + 3 * col;
	int gPos = bPos + 1;
	int rPos = bPos + 2;

	img[rPos] = r;
	img[gPos] = g;
	img[bPos] = b;
}

void BitMapProcessor::takasa()
{
	uint8_t takasa[FILE_HEADER_SIZE];
	size_t size = fread(takasa, sizeof(uint8_t), FILE_HEADER_SIZE, bmp);

	memcpy(fHeader.data, takasa, sizeof(takasa));
	fHeader.fileType = "";
	fHeader.fileType += takasa[0];
	fHeader.fileType += takasa[1];
	fHeader.fileSize = bit2Integer(takasa[2], takasa[3], takasa[4], takasa[5]);
}

void BitMapProcessor::yoko()
{
	uint8_t yoko[INFO_HEADER_SIZE];
	size_t size = fread(yoko, sizeof(uint8_t), INFO_HEADER_SIZE, bmp);

	memcpy(iHeader.data, yoko, sizeof(yoko));
	iHeader.infoHeaderSize = bit2Integer(yoko[0], yoko[1], yoko[2], yoko[3]);
	iHeader.width = bit2Integer(yoko[4], yoko[5], yoko[6], yoko[7]);
	iHeader.height = bit2Integer(yoko[8], yoko[9], yoko[10], yoko[11]);
	iHeader.clrPerPixel = bit2Integer(yoko[14], yoko[15], 0, 0);
	iHeader.dataSize = bit2Integer(yoko[20], yoko[21], yoko[22], yoko[23]);
}

/*
* ビットマップデータを加工前に復元する
*/
void BitMapProcessor::restore()
{
	memcpy(img, org, iHeader.dataSize);
}

/*
* テスト用関数（1）モノクロ化
*/
void twoTone(BitMapProcessor* bmp)
{
	int i;
	int j;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			int ave;
			ave = 0;
			ave += bmp->getColor(i, j).r;
			ave += bmp->getColor(i, j).g;
			ave += bmp->getColor(i, j).b;
			ave /= 3;

			bmp->setColor(i, j, ave, ave, ave);
		}
	}
}

/*
* テスト関数（2）指定範囲の切り取り
*/
void extractArea(BitMapProcessor* bmp, int r0, int r1, int c0, int c1)
{
	int i;
	int j;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			if (r0 <= i && i <= r1 && c0 <= j && j <= c1)
			{
				continue;
			}
			bmp->setColor(i, j, 255, 255, 255);
		}
	}
}

/*
* テスト関数（3） 色の反転
*/
void invert(BitMapProcessor* bmp)
{
	int i;
	int j;
	int ave;
	int r;
	int g;
	int b;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			ave = 0;
			r = bmp->getColor(i, j).r;
			g = bmp->getColor(i, j).g;
			b = bmp->getColor(i, j).b;

			bmp->setColor(i, j, 255 - r, 255 - g, 255 - b);
		}
	}
}

/*
* テスト関数（4）モザイク化
*/
void mosaic(BitMapProcessor* bmp, int level)
{
	int i;
	int j;
	int r;
	int g;
	int b;
	int cnt;
	int x;
	int y;
	int xt;
	int yt;

	if (level <= 0)
	{
		level = 1;
	}

	for (i = 0; i < bmp->height(); i += 2 * level)
	{
		for (j = 0; j < bmp->width(); j += 2 * level)
		{
			r = 0;
			g = 0;
			b = 0;
			cnt = 0;

			for (x = -level; x <= level; x++)
			{
				for (y = -level; y <= level; y++)
				{
					xt = i + x;
					yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
					{
						continue;
					}
					++cnt;
					r += bmp->getColor(xt, yt).r;
					g += bmp->getColor(xt, yt).g;
					b += bmp->getColor(xt, yt).b;
				}
			}

			r /= cnt;
			g /= cnt;
			b /= cnt;

			for (int x = -level; x <= level; x++)
			{
				for (int y = -level; y <= level; y++)
				{
					int xt = i + x;
					int yt = j + y;

					if (xt < 0 || yt < 0 || xt >= bmp->height() || yt >= bmp->width())
					{
						continue;
					}

					bmp->setColor(xt, yt, r, g, b);
				}
			}
		}
	}
}

void filetwoTone(BitMapProcessor* bmp)
{
	int i;
	int j;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			int ave;
			ave = 0;
			ave += bmp->getColor(i, j).r;
			ave += bmp->getColor(i, j).g;
			ave += bmp->getColor(i, j).b;
			ave /= 3;

			bmp->setColor(i, j, ave, ave, ave);
		}
	}
}

void fileArea(BitMapProcessor* bmp, int r0, int r1, int c0, int c1)
{
	int i;
	int j;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			if (r0 <= i && i <= r1 && c0 <= j && j <= c1)
			{
				continue;
			}
			bmp->setColor(i, j, 255, 255, 255);
		}
	}
}

void fileinvert(BitMapProcessor* bmp)
{
	int i;
	int j;
	int ave;
	int r;
	int g;
	int b;
	for (i = 0; i < bmp->height(); i++)
	{
		for (j = 0; j < bmp->width(); j++)
		{
			ave = 0;
			r = bmp->getColor(i, j).r;
			g = bmp->getColor(i, j).g;
			b = bmp->getColor(i, j).b;

			bmp->setColor(i, j, 255 - r, 255 - g, 255 - b);
		}
	}
}

/*
* メイン処理
*/
int main() {
	BitMapProcessor bmp[100];
	char bit[100];
	char kinou[5];
	int moza;
	int up;
	int down;
	int right;
	int left;
	int n = 0;
	int i = 1;
	int downwk;
	int rightwk;

	while (true)
	{
		printf("画像入力：アドレスを記入\n");
		printf("ツール選択：next\n");
		scanf("%s", bit);

		if (strcmp(bit, "next") != 0)
		{
			n++;
			// ビットマップデータのロード
			bmp[n].loadData(bit);

			// ビットマップ情報の表示
			bmp[n].dispBmpInfo();

			printf("\n");
		}
		else
		{
			break;
		}
	}

	while (true)
	{
		printf("\nモノクロ:mono　　トリミング:tori\n");
		printf("色反転:hann　　　モザイク:moza\n");
		printf("終了:exit\n");
		scanf("%s", kinou);

		if (strcmp(kinou, "mono") == 0)
		{
			// テスト1. モノクロ化
			for (i = 1; i <= n; i++)
			{
				twoTone(&bmp[i]);
				bmp[i].writeData(1, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "tori") == 0)
		{
			// テスト2. 指定範囲の切り出し
			printf("切り取る範囲\n");
			printf("上 下 左 右\n");
			scanf("%d %d %d %d", &up, &down, &left, &right);
			for (i = 1; i <= n; i++)
			{
				downwk = bmp[i].height() - down;
				rightwk = bmp[i].width() - right;
				extractArea(&bmp[i], up, downwk, left, rightwk);
				bmp[i].writeData(2, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "hann") == 0)
		{
			// テスト3. 色反転
			for (i = 1; i <= n; i++)
			{
				invert(&bmp[i]);
				bmp[i].writeData(3, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "moza") == 0)
		{
			// テスト4. モザイク
			printf("粗さ\n");
			scanf("%d", &moza);
			for (i = 1; i <= n; i++)
			{
				mosaic(&bmp[i], moza);
				bmp[i].writeData(4, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "exit") == 0)
		{
			break;
		}
		else
		{
			printf("関数が見つかりません\n");
		}
	}
	return 0;
}
