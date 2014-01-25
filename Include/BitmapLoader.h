#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;  //specifies the file type
    unsigned long bfSize;  //specifies the size in bytes of the bitmap file
    unsigned short bfReserved1;  //reserved; must be 0
    unsigned short bfReserved2;  //reserved; must be 0
    unsigned long bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    unsigned long biSize;  //specifies the number of bytes required by the struct
    long biWidth;  //specifies width in pixels
    long biHeight;  //species height in pixels
    unsigned short biPlanes; //specifies the number of color planes, must be 1
    unsigned short biBitCount; //specifies the number of bit per pixel
    unsigned long biCompression;//spcifies the type of compression
    unsigned long biSizeImage;  //size of image in bytes
    long biXPelsPerMeter;  //number of pixels per meter in x axis
    long biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned long biClrUsed;  //number of colors used by th ebitmap
    unsigned long biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

T_byte8 *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
#endif