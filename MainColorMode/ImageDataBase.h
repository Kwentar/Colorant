#include <Windows.h>
#include "Constant.h"
#include "ImageInDB.h"

class ImageDataBase
{
public:
	ImageDataBase(void);
	~ImageDataBase(void);

	int addImage(double* signature);
	int deleteImage(char * fileName);

private:
	int getFileNumber();
	int getImages(ImageInDB *images);
	int writeImage(ImageInDB image);
	int writeImages(ImageInDB * images,int count);
	int getCountImages();

};

