#include <iostream>
#include "simple_bmp.h"

void getRGB(int p, int& r, int &g, int &b){
	int mask = 0x000000FF;
	r = p & mask;
	p >>= 8;
	g = p & mask;
	b = (p >> 8);
}

void getPixel(int& p, int r, int g, int b){
	p = b;
	p <<= 8;
	p = p | g;
	p <<= 8;
	p = p | r;
}

int getMed(int* m){
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9 - j - 1; j++)
			if(m[j] > m[j+1]){
				int tmp = m[j];
				m[j] = m[j+1];
				m[j+1] = tmp;			
			}
	return m[4];
}

int main(int argc, const char** argv)
{
  int w = 0, h = 0;
  std::vector<int> data = LoadBMP("data/texture1.bmp", &w, &h);
  std::vector<int> res = LoadBMP("data/texture1.bmp", &w, &h);

  if (data.size() == 0)
  {
    std::cout << "can't load image" << std::endl;
    return 0;
  }


  for (int y = 1; y < h - 1; y++)
    for (int x = 1; x < w - 1; x++){
		int m1[9], m2[9], m3[9];
		int r, g, b;

		getRGB(data[x - 1 + (y - 1)*w], r, g, b);
		m1[0] = r;
		m2[0] = g;
		m3[0] = b;
		getRGB(data[x - 1 + (y)*w], r, g, b);
		m1[1] = r;
		m2[1] = g;
		m3[1] = b;
getRGB(data[x - 1 + (y + 1)*w], r, g, b);
		m1[2] = r;
		m2[2] = g;
		m3[2] = b;
getRGB(data[x + (y - 1)*w], r, g, b);
		m1[3] = r;
		m2[3] = g;
		m3[3] = b;
getRGB(data[x + (y)*w], r, g, b);
		m1[4] = r;
		m2[4] = g;
		m3[4] = b;
getRGB(data[x + (y + 1)*w], r, g, b);
		m1[5] = r;
		m2[5] = g;
		m3[5] = b;
getRGB(data[x + 1 + (y - 1)*w], r, g, b);
		m1[6] = r;
		m2[6] = g;
		m3[6] = b;
getRGB(data[x + 1 + (y)*w], r, g, b);
		m1[7] = r;
		m2[7] = g;
		m3[7] = b;
getRGB(data[x + 1 + (y + 1)*w], r, g, b);
		m1[8] = r;
		m2[8] = g;
		m3[8] = b;
		int p;
		getPixel(p, getMed(m1), getMed(m2), getMed(m3));
		res[x + y*w] = p;
	}
      

  SaveBMP("data/z_out.bmp", &res[0], w, h);

  std::cout << "finish" << std::endl;
  return 0;
}

