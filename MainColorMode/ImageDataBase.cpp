#include "ImageDataBase.h"


ImageDataBase::ImageDataBase(void)
{
}

ImageDataBase::~ImageDataBase(void)
{
}

int ImageDataBase::addImage(double* signature)
{
	DWORD nBytesRead,nBytesWritten; //прочитано и записано байт
	DWORD nFileSize;			//размер файла
	DWORD nImageCount;			//количество изображений в базе
	HANDLE hFile;				//дескриптор файла
	char imageFileName[20],tmp[10];		//имя изображения для сохранения и промежуточная строковая переменная

	/*--------Добавляем изображение в базу---*/
	CreateDirectoryA("base",NULL);
	int number=getFileNumber();
	if(number==-1)
	{
		return -1;
	}
	ImageInDB addImage;
	for(int i=0;i<HIST_SIZE;i++)
	{
		addImage.signature[i]=signature[i];
	}
	itoa(number,tmp,10);
	strcpy(imageFileName,tmp);
	strcat(imageFileName,".png");
	strcpy(addImage.name,imageFileName);
	writeImage(addImage);
	return number;
}

int ImageDataBase::getFileNumber()
{
	int number=1;
	HANDLE hFile;
	DWORD nameLength;
	DWORD nBytesRead; //прочитано и записано байт
	DWORD nFileSize;			//размер файла
	DWORD nImageCount;
	int *numbers;
	int numberIndex=0;
	hFile=CreateFileA("base\\base.dat",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) 
	{
		return -1;
	}
	nFileSize=GetFileSize(hFile,NULL);
	if(!nFileSize)
	{
		return 1;;
	}
	else
	{
 		ReadFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesRead,NULL);
		numbers=new int[nImageCount];
		SetFilePointer(hFile,sizeof(nImageCount),NULL,FILE_BEGIN);
		for(int i=0;i<nImageCount;i++)
		{
			ReadFile(hFile,&nameLength,sizeof(nImageCount),&nBytesRead,NULL);
			char *fileName=new char[nameLength+1];
			ReadFile(hFile,fileName,nameLength,&nBytesRead,NULL);
			SetFilePointer(hFile,sizeof(double)*HIST_SIZE,NULL,FILE_CURRENT);
			char tmp[5];
			strncpy(tmp,fileName,nameLength-4);
			tmp[nameLength-4]='\0';
			numbers[numberIndex++]=atoi(tmp);
		}
		for(int i=0;i<nImageCount;i++)
		{
			bool exist=false;
			for(int j=0;j<nImageCount;j++)
			{
				if((i+1)==numbers[j])
				{
					exist=true;
					break;
				}
			}
			if(!exist)
			{
				return i+1;
			}
		}
		return nImageCount+1;
	}
}

int ImageDataBase::deleteImage(char * fileName)
{
	ImageInDB *images=0;
	DWORD nImageCount=getCountImages();
	images=new ImageInDB[nImageCount];
	getImages(images);
	nImageCount--;
	for(int i=0;i<nImageCount;i++)
	{
		if(strstr(fileName,images[i].name))
		{
			images[i]=images[nImageCount];
		}
	}
	writeImages(images,nImageCount);
	return 0;
}

int ImageDataBase::getImages(ImageInDB *images)
{
	int nameLength;
	DWORD nBytesRead;
	DWORD nImageCount;
	HANDLE hFile;
	nImageCount=getCountImages();
	hFile=CreateFileA("base\\base.dat",GENERIC_READ,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) 
	{
		return -1;
	}
	SetFilePointer(hFile,sizeof(nImageCount),NULL,FILE_BEGIN);
	for(int i=0;i<nImageCount;i++)
	{
		ReadFile(hFile,&nameLength,sizeof(nameLength),&nBytesRead,NULL);
		ReadFile(hFile,images[i].name,nameLength,&nBytesRead,NULL);
		images[i].name[nameLength]='\0';
		for(int j=0;j<HIST_SIZE;j++)
		{
			double currentBin;
 			ReadFile(hFile,&currentBin,sizeof(currentBin),&nBytesRead,NULL);
			images[i].signature[j]=currentBin;
		}
	}
	CloseHandle(hFile);
	return nImageCount;
}

int ImageDataBase::writeImage(ImageInDB image)
{
	HANDLE hFile;				//дескриптор файла
	DWORD nFileSize;
	DWORD nImageCount;
	DWORD nBytesRead,nBytesWritten;
	nImageCount=getCountImages();
	hFile=CreateFileA("base\\base.dat",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	nImageCount++;
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	WriteFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesWritten,NULL);
	SetFilePointer(hFile,0,NULL,FILE_END);
	DWORD nameLength=strlen(image.name);
	WriteFile(hFile,&nameLength,sizeof(nImageCount),&nBytesWritten,NULL);
	WriteFile(hFile,image.name,nameLength,&nBytesWritten,NULL);
	for(int i=0;i<HIST_SIZE;i++)
	{
		WriteFile(hFile,&image.signature[i],sizeof(double),&nBytesWritten,NULL);
	}
	CloseHandle(hFile);
	return 0;
}

int ImageDataBase::writeImages(ImageInDB * images,int count)
{
	HANDLE hFile=CreateFileA("base\\base.dat",GENERIC_READ|GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) //файл
	{
		return NULL;
	}
	CloseHandle(hFile);
	for(int i=0;i<count;i++)
	{
		writeImage(images[i]);
	}
	return 0;
}

int ImageDataBase::getCountImages()
{
	HANDLE hFile;				//дескриптор файла
	DWORD nFileSize;
	DWORD nImageCount;
	DWORD nBytesRead,nBytesWritten;
	hFile=CreateFileA("base\\base.dat",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) 
	{
		return -1;
	}
	nFileSize=GetFileSize(hFile,NULL);
	if(!nFileSize)
	{
		nImageCount=0;
	}
	else
	{
 		ReadFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesRead,NULL);
	}
	CloseHandle(hFile);
	return nImageCount;
}