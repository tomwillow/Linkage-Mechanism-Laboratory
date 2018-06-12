/*

This file is modified from Milo Yip's svpng.inc, its origin statement is attached below.

by Tom Willow

*/

/*
Copyright (C) 2017 Milo Yip. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file
\brief      svpng() is a minimalistic C function for saving RGB/RGBA image into uncompressed PNG.
\author     Milo Yip
\version    0.1.1
\copyright  MIT license
\sa         http://github.com/miloyip/svpng
*/

/*
Above is Milo Yip's origin statement.
*/
#pragma once
#include <stdio.h>
#include <Windows.h>

#define TOMPNG_PUT(u) fputc(u,fp)

//不带CRC的输出
#define TOMPNG_U8A(ua,l) for(i=0;i<l;++i) TOMPNG_PUT((ua)[i]);
#define TOMPNG_U32(u) TOMPNG_PUT((u)>>24); TOMPNG_PUT(((u)>>16)&0xFF);TOMPNG_PUT(((u)>>8)&0xFF);TOMPNG_PUT((u)&0xFF);

//带CRC的输出
#define TOMPNG_U8C(u) do { TOMPNG_PUT(u); c ^= (u); c = (c >> 4) ^ t[c & 15]; c = (c >> 4) ^ t[c & 15]; } while(0)
#define TOMPNG_U8AC(ua,l) do {for(i=0;i<l;++i) TOMPNG_U8C((ua)[i]);}while(0)
#define TOMPNG_U16LC(u) do{TOMPNG_U8C((u)&0xFF);TOMPNG_U8C(((u)>>8)&0xFF);}while(0)
#define TOMPNG_U32C(u) TOMPNG_U8C((u)>>24); TOMPNG_U8C(((u)>>16)&0xFF);TOMPNG_U8C(((u)>>8)&0xFF);TOMPNG_U8C((u)&0xFF);

#define TOMPNG_U8ADLER(u) do{TOMPNG_U8C(u);a=(a+(u))%65521;b=(b+a)%65521;}while(0)

#define TOMPNG_BEGIN(s,l) do {TOMPNG_U32(l);c=~0U;TOMPNG_U8AC(s,4);}while(0)
#define TOMPNG_END() TOMPNG_U32(~c)

static const char tompng_fileheader[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
/* CRC32 Table */

void tompng(FILE *fp, unsigned int width, unsigned int height, const unsigned char *rgb, int alpha)
{

static const unsigned t[] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };

	unsigned i(0), c;
	TOMPNG_U8A(tompng_fileheader, sizeof(tompng_fileheader));

	//IHDR
	TOMPNG_BEGIN("IHDR", 13);
	TOMPNG_U32C(width);
	TOMPNG_U32C(height);
	TOMPNG_U8C(8);
	TOMPNG_U8C(alpha ? 6 : 2);
	TOMPNG_U8AC("\0\0\0", 3);
	TOMPNG_END();

	//IDAT
	unsigned a = 1, b = 0, p = width * (alpha ? 4 : 3) + 1, x, y;
	TOMPNG_BEGIN("IDAT", 2 + height * (5 + p) + 4);
	TOMPNG_U8AC("\x78\1", 2);
	for (y = 0; y < height; y++) { 
		TOMPNG_U8C(y == height - 1);
		TOMPNG_U16LC(p); 
		TOMPNG_U16LC(~p);
		TOMPNG_U8ADLER(0);
		for (x = 0; x < p - 1; x++, rgb++)
			TOMPNG_U8ADLER(*rgb);
	}
	TOMPNG_U32C((b << 16) | a);
	TOMPNG_END();

	//IEND
	TOMPNG_BEGIN("IEND", 0);
	TOMPNG_END();
}

union union_achar2u{
	char c[4];
	unsigned u;
};


inline unsigned acharto32intbig(char *c)
{
	union_achar2u uni;
	for (int i = 3; i >= 0; --i)
		uni.c[i] = c[3-i];
	return uni.u;
}
inline unsigned fget32int_big(FILE *fp)
{
	union_achar2u uni;
	for (int i = 3; i >= 0; --i)
		uni.c[i] = fgetc(fp);
		return uni.u;
}

#define TOMPNG_OUT_U8A(ua,l) for(i=0;i<l;++i) printf("%c",ua[i]);

//not finished
bool tompng_read(FILE *fp)
{
	unsigned length;
	char fileheader[8];
	int i;
	char c;
	for (i = 0; i < 8; ++i)
	{
		fileheader[i] = fgetc(fp);
	}
	if (strncmp(fileheader, tompng_fileheader, 8) != 0)
	{
		printf("Error File Header.\n");
		return false;
	}

	char *buf;
	char chunk_name[4];
	int crc;
	while (!feof(fp))
	{
		//length
		length = fget32int_big(fp);
		if (length == 0)
		{
			printf("Error: length equals 0.\n");
			return false;
		}
		printf("Length : %d\n", length);

		//name
		fread(chunk_name, 1, 4, fp);
		TOMPNG_OUT_U8A(chunk_name, 4);
		printf("\n");

		//content
		buf = new char[length];
		fread(buf, 1, length, fp);

		char *now = buf;
		if (strncmp(chunk_name, "IHDR", 4) == 0)
		{
			unsigned width, height;
			width = acharto32intbig(now);
			now += 4;
			height = acharto32intbig(now);
			now += 4;
			printf("Width=%d Height=%d\n", width, height);

			char bit = *now++;
			char color = *now++;
			char compression = *now++;
			char filter = *now++;
			char interlace = *now++;

			printf("bit=%d\n", bit);
			printf("color=%d\n", color);
			printf("compression=%d\n", compression);
			printf("filter=%d\n", filter);
			printf("interlace=%d\n", interlace);
		}

		if (strncmp(chunk_name, "IDAT", 4) == 0)
		{
			unsigned fcheck = MAKEWORD(*(now + 1), *now);
			if (fcheck % 31 != 0)
			{
				printf("FCHECK fault.\n");
				delete[] buf;
				return false;
			}
			++now;

			unsigned flevel = ((*now) >> 6)&3;
			printf("FLEVEL=%d\n", flevel);

			bool fdict = ((*now) >> 5) & 1;
			printf("FDICT=%d\n", fdict);
		}

		delete[] buf;

		//CRC
		crc = fget32int_big(fp);
		printf("CRC : %X\n\n", crc);
	}
}