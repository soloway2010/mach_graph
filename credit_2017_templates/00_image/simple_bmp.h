#include <vector>

std::vector<int> LoadBMP(const char* fname, int* w, int* h);
void SaveBMP(const char* fname, const int* pixels, int w, int h);

