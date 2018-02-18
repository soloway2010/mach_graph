#include "simple_bmp.h"
#include <iostream>
#include <fstream>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <vector>

struct Pixel
{
  unsigned char r, g, b;
};

#ifndef WIN32

#include <string.h> 

typedef unsigned short WORD;
typedef unsigned long  DWORD;

#pragma pack(1)
struct BITMAPFILEHEADER
{
  WORD    bfType;
  DWORD   bfSize;
  WORD    bfReserved1;
  WORD    bfReserved2;
  DWORD   bfOffBits;
};

#pragma pack(1)
struct BITMAPINFOHEADER
{
  DWORD    biSize;
  DWORD    biWidth;
  DWORD    biHeight;
  WORD     biPlanes;
  WORD     biBitCount;
  DWORD    biCompression;
  DWORD    biSizeImage;
  DWORD    biXPelsPerMeter;
  DWORD    biYPelsPerMeter;
  DWORD    biClrUsed;
  DWORD    biClrImportant;
};

#define BI_RGB 0L

#else
#include <windows.h>
#endif

static void WriteBMP(const char* fname, Pixel* a_pixelData, int width, int height)
{
  const int w = width;
  const int h = height;

  unsigned char *img = (unsigned char*)a_pixelData;

  unsigned char file[14] = {
    'B','M', // magic
    0,0,0,0, // size in bytes
    0,0, // app data
    0,0, // app data
    40 + 14,0,0,0 // start of data offset
  };

  unsigned char info[40] = {
    40,0,0,0, // info hd size
    0,0,0,0, // width
    0,0,0,0, // heigth
    1,0, // number color planes
    24,0, // bits per pixel
    0,0,0,0, // compression is none
    0,0,0,0, // image bits size
    0x13,0x0B,0,0, // horz resoluition in pixel / m
    0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
    0,0,0,0, // #colors in pallete
    0,0,0,0, // #important colors
  };


  int padSize = (4 - (w * 3) % 4) % 4;
  int sizeData = w * h * 3 + h * padSize;
  int sizeAll = sizeData + sizeof(file) + sizeof(info);

  file[2] = (unsigned char)(sizeAll);
  file[3] = (unsigned char)(sizeAll >> 8);
  file[4] = (unsigned char)(sizeAll >> 16);
  file[5] = (unsigned char)(sizeAll >> 24);

  info[4] = (unsigned char)(w);
  info[5] = (unsigned char)(w >> 8);
  info[6] = (unsigned char)(w >> 16);
  info[7] = (unsigned char)(w >> 24);

  info[8] = (unsigned char)(h);
  info[9] = (unsigned char)(h >> 8);
  info[10] = (unsigned char)(h >> 16);
  info[11] = (unsigned char)(h >> 24);

  info[20] = (unsigned char)(sizeData);
  info[21] = (unsigned char)(sizeData >> 8);
  info[22] = (unsigned char)(sizeData >> 16);
  info[23] = (unsigned char)(sizeData >> 24);

  std::ofstream stream(fname, std::ios::binary);

  stream.write((char*)file, sizeof(file));
  stream.write((char*)info, sizeof(info));

  unsigned char pad[3] = { 0,0,0 };

  for (int y = 0; y<h; y++)
  {
    for (int x = 0; x<w; x++)
    {
      unsigned char pixel[3];
      pixel[0] = img[(y*w + x) * 3 + 0];
      pixel[1] = img[(y*w + x) * 3 + 1];
      pixel[2] = img[(y*w + x) * 3 + 2];
      stream.write((char*)pixel, 3);
    }
    stream.write((char*)pad, padSize);
  }

  stream.close();
}

void SaveBMP(const char* fname, const int* pixels, int w, int h)
{
  std::vector<Pixel> pixels2(w*h);

  for (size_t i = 0; i < pixels2.size(); i++)
  {
    int pxData = pixels[i];
    Pixel px;
    px.r = (pxData & 0x00FF0000) >> 16;
    px.g = (pxData & 0x0000FF00) >> 8;
    px.b = (pxData & 0x000000FF);
    pixels2[i] = px;
  }

  WriteBMP(fname, &pixels2[0], w, h);
}


void SavePPM(const char* fname, const int* pixels, int w, int h)
{
  std::vector<Pixel> pixels2(w*h);

  for (size_t i = 0; i < pixels2.size(); i++)
  {
    int pxData = pixels[i];
    Pixel px;
    px.r = (pxData & 0x00FF0000) >> 16;
    px.g = (pxData & 0x0000FF00) >> 8;
    px.b = (pxData & 0x000000FF);
    pixels2[i] = px;
  }

  FILE *fp = fopen(fname, "wb"); /* b - binary mode */
  fprintf(fp, "P6\n%d %d\n255\n", w, h);
  fwrite((char*)&pixels2[0], sizeof(Pixel)*pixels2.size(), 1, fp);

  fclose(fp);

}


inline const int readInt(unsigned char* ptr) // THIS IS CORRECT BOTH FOR X86 AND PPC !!!
{
  const unsigned char b0 = ptr[0];
  const unsigned char b1 = ptr[1];
  const unsigned char b2 = ptr[2];
  const unsigned char b3 = ptr[3];

  return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}


unsigned char* ReadBMP(const char* filename, int* pW, int* pH)
{
  //int i;
  FILE* f = fopen(filename, "rb");

  if (f == NULL)
    throw "Argument Exception";

  unsigned char info[54];
  fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

  // extract image height and width from header
  //int width  = *(int*)&info[18];
  //int height = *(int*)&info[22];

  int width = readInt(&info[18]);
  int height = readInt(&info[22]);

  std::cout << std::endl;
  std::cout << "  Name: " << filename << std::endl;
  std::cout << " Width: " << width << std::endl;
  std::cout << "Height: " << height << std::endl;

  int row_padded = (width * 3 + 3) & (~3);
  unsigned char* data = new unsigned char[row_padded]; ///  THIS IS PIECI OF SHIT !!!!!!! <==== <---- !!!!!
  //unsigned char tmp;

  unsigned char* data2 = new unsigned char[width*height * 3];

  for (int i = 0; i < height; i++)
  {
    fread(data, sizeof(unsigned char), row_padded, f);

    /*
    for(int j = 0; j < width*3; j += 3)
    {
    // Convert (B, G, R) to (R, G, B)
    tmp = data[j];
    data[j] = data[j+2];
    data[j+2] = tmp;
    //std::cout << "R: "<< (int)data[j] << " G: " << (int)data[j+1]<< " B: " << (int)data[j+2]<< std::endl;
    }
    */

    for (int j = 0; j<width; j++)
    {
      int index = i*width + j;
      data2[index * 3 + 0] = data[j * 3 + 0];
      data2[index * 3 + 1] = data[j * 3 + 1];
      data2[index * 3 + 2] = data[j * 3 + 2];
    }

  }

  fclose(f);

  if (pW != 0)
    (*pW) = width;

  if (pH != 0)
    (*pH) = height;

  delete[] data;

  return data2;
}

std::vector<int> LoadBMP(const char* fname, int* w, int* h)
{

  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER info;

  memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
  memset(&info, 0, sizeof(BITMAPINFOHEADER));
  std::ifstream in(fname, std::ios::binary);

  if (!in.is_open())
  {
    std::cerr << "LoadBMP: can't open file" << std::endl;
    (*w) = 0;
    (*h) = 0;
    return std::vector<int>();
  }

  in.close();

  int testW = 0, testH = 0;

  unsigned char* bitdata24 = ReadBMP(fname, &testW, &testH);

  std::vector<int> res(testW*testH);
  uint8_t* pixels2 = (uint8_t*)&res[0];

  uint8_t* pixels = (uint8_t*)bitdata24;

  for (size_t i = 0; i < res.size(); i++)
  {
    pixels2[i * 4 + 0] = pixels[i * 3 + 2];
    pixels2[i * 4 + 1] = pixels[i * 3 + 1];
    pixels2[i * 4 + 2] = pixels[i * 3 + 0];
    pixels2[i * 4 + 3] = 255;
  }

  delete[] bitdata24;

  if (w != 0)
    (*w) = testW;

  if (h != 0)
    (*h) = testH;

  return res;
}
