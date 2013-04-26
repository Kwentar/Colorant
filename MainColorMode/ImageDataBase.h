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
	int getImages(ImageInDB *images);
	int getCountImages();


private:
	int getFileNumber();
	int writeImage(ImageInDB image);
	int writeImages(ImageInDB * images,int count);

};

