#include "GENERAL.h"
#include "BitmapLoader.h"

T_byte8 *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    T_byte8 *bitmapImage;  //store image data
	T_byte8 *retval = NULL;
	int imageIdx=0;  //image index counter

    DebugRoutine("ClientUpdate") ;


    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr != NULL)
	{
		//read the bitmap file header
		fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

		//verify that this is a bmp file by check bitmap id
		if (bitmapFileHeader.bfType ==0x4D42)
		{

			//read the bitmap info header
			fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); // small edit. forgot to add the closing bracket at sizeof

			//move file point to the begging of bitmap data
			fseek(filePtr, bitmapFileHeader.bOffBits, SEEK_SET);

			//allocate enough memory for the bitmap image data
			bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

			//verify memory allocation
			if (bitmapImage)
			{

				//read in the bitmap image data
				fread(bitmapImage,1,bitmapInfoHeader->biSizeImage,filePtr);

				//make sure bitmap image data was read
				if (bitmapImage != NULL)
				{
					retval = bitmapImage;
				}
			}
			else
			{
				free(bitmapImage);
			}
		}	
		//close file and return bitmap image data
		fclose(filePtr);
	}

	DebugEnd();
    return retval;
}