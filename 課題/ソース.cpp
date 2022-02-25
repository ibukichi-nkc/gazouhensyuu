#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string.h>

#define FILE_HEADER_SIZE 14                // �t�@�C���w�b�_�̃T�C�Y
#define INFO_HEADER_SIZE 40                // ���w�b�_�̃T�C�Y

using namespace std;

/*
* �t�@�C���w�b�_�[�\����
*/
typedef struct FileHeader
{
	uint8_t data[FILE_HEADER_SIZE];  // ���H�O�f�[�^�i�����o���̂��ߕK�v�j
	string fileType;                 // �t�@�C���^�C�v
	int fileSize;                    // �t�@�C���T�C�Y
} FileHeader;

/*
* ���w�b�_�[�\����
*/
typedef struct InfoHeader
{
	uint8_t data[INFO_HEADER_SIZE];  // ���H�O�f�[�^�i�����o���̂��ߕK�v�j
	int infoHeaderSize;              // ���w�b�_�̃T�C�Y
	int width;                       // �摜�̕�
	int height;                      // �摜�̍���
	int clrPerPixel;                 // 1�s�N�Z��������̐F��
	int dataSize;                    // �摜�f�[�^�̃T�C�Y
} InfoHeader;

/*
* �s�N�Z���̐F���\����
*/
typedef struct Color
{
	int r;                // �Ԑ���
	int g;                // �ΐ���
	int b;                // ����
} Color;

int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

/*
* �r�b�g�}�b�v�����N���X
*/
class BitMapProcessor
{
	FILE* bmp;               // �r�b�g�}�b�v�̃t�@�C���|�C���^
	uint8_t* img;            // �r�b�g�}�b�v�f�[�^�i���H�p�j
	uint8_t* org;            // �r�b�g�}�b�v�f�[�^�i�ǂݍ��ݎ��j
	FileHeader fHeader;      // �t�@�C���w�b�_
	InfoHeader iHeader;      // ���w�b�_

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
* 4�r�b�g����Int�����l�ɕϊ�
*/
int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	return b1 +
		b2 * 256 +
		b3 * 256 * 256 +
		b4 * 256 * 256 * 256;
}

/*
* �r�b�g�}�b�v�f�[�^�����[�h����
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
		printf("�t�@�C���I�[�v���Ɏ��s���܂����B\n");
	}

	readFileHeader();
	readInfoHeader();
	readBmpData();
}

/*
* �t�@�C���w�b�_��ǂ�
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
* ���w�b�_��ǂ�
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
* �摜�f�[�^��ǂ�
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
		printf("�摜�f�[�^�ǂݍ��݂̃T�C�Y���������Ă��܂��B\n");
	}

	// �o�b�N�A�b�v�p��org�ɉ摜�f�[�^���R�s�[
	if (org != NULL)
	{
		delete[]org;
	}
	org = new uint8_t[sz];
	memcpy(org, img, sz);
}

/*
* �r�b�g�}�b�v���̕\���i�f�o�b�O�p�j
*/
void BitMapProcessor::dispBmpInfo()
{
	cout << "���t�@�C���w�b�_���" << endl;
	cout << "�t�@�C���^�C�v: " << fHeader.fileType << endl;
	cout << "�t�@�C���T�C�Y: " << fHeader.fileSize << endl;

	cout << "�����w�b�_���" << endl;
	cout << "���w�b�_�T�C�Y: " << iHeader.infoHeaderSize << endl;
	cout << "�摜��: " << iHeader.width << endl;
	cout << "�摜��: " << iHeader.height << endl;
	cout << "�P�s�N�Z��������̐F��: " << iHeader.clrPerPixel << endl;
	cout << "�摜�f�[�^�̃T�C�Y: " << iHeader.dataSize << endl;
}

/*
* �r�b�g�}�b�v�f�[�^�̃t�@�C�������o��
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
		printf("�����o����̃t�@�C�����J���܂���B\n");
	}

	fwrite(fHeader.data, sizeof(uint8_t), FILE_HEADER_SIZE, out);
	fwrite(iHeader.data, sizeof(uint8_t), INFO_HEADER_SIZE, out);
	fwrite(img, sizeof(uint8_t), iHeader.dataSize, out);

	fclose(out);
}

/*
* �w�肳�ꂽ�s�N�Z���̐F���擾
*/
Color BitMapProcessor::getColor(int row, int col)
{
	int width;
	int bPos;
	int gPos;
	int rPos;

	if (row < 0 || row >= iHeader.height)
	{
		printf("getColor(): row���͈͊O�ł��B\n");
	}

	if (col < 0 || col >= iHeader.width)
	{
		printf("getColor(): col���͈͊O�ł��B\n");
	}

	width = 3 * iHeader.width;
	while (width % 4)        // �r�b�g�}�b�v��1���4�̔{���r�b�g����Ȃ�
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
* �w�肳�ꂽ�s�N�Z���ɐF��ݒ�
*/
void BitMapProcessor::setColor(int row, int col, int r, int g, int b)
{
	if (row < 0 || row >= iHeader.height)
	{
		printf("getColor(): row���͈͊O�ł��B\n");
	}
	if (col < 0 || col >= iHeader.width)
	{
		printf("getColor(): col���͈͊O�ł��B\n");
	}

	int width;
	width = 3 * iHeader.width;
	while (width % 4)        // �r�b�g�}�b�v��1���4�̔{���r�b�g����Ȃ�
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
* �r�b�g�}�b�v�f�[�^�����H�O�ɕ�������
*/
void BitMapProcessor::restore()
{
	memcpy(img, org, iHeader.dataSize);
}

/*
* �e�X�g�p�֐��i1�j���m�N����
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
* �e�X�g�֐��i2�j�w��͈͂̐؂���
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
* �e�X�g�֐��i3�j �F�̔��]
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
* �e�X�g�֐��i4�j���U�C�N��
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
* ���C������
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
		printf("�摜���́F�A�h���X���L��\n");
		printf("�c�[���I���Fnext\n");
		scanf("%s", bit);

		if (strcmp(bit, "next") != 0)
		{
			n++;
			// �r�b�g�}�b�v�f�[�^�̃��[�h
			bmp[n].loadData(bit);

			// �r�b�g�}�b�v���̕\��
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
		printf("\n���m�N��:mono�@�@�g���~���O:tori\n");
		printf("�F���]:hann�@�@�@���U�C�N:moza\n");
		printf("�I��:exit\n");
		scanf("%s", kinou);

		if (strcmp(kinou, "mono") == 0)
		{
			// �e�X�g1. ���m�N����
			for (i = 1; i <= n; i++)
			{
				twoTone(&bmp[i]);
				bmp[i].writeData(1, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "tori") == 0)
		{
			// �e�X�g2. �w��͈͂̐؂�o��
			printf("�؂���͈�\n");
			printf("�� �� �� �E\n");
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
			// �e�X�g3. �F���]
			for (i = 1; i <= n; i++)
			{
				invert(&bmp[i]);
				bmp[i].writeData(3, ".bmp", i);
				bmp[i].restore();
			}
		}
		else if (strcmp(kinou, "moza") == 0)
		{
			// �e�X�g4. ���U�C�N
			printf("�e��\n");
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
			printf("�֐���������܂���\n");
		}
	}
	return 0;
}
