#include <Windows.h>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <locale.h>

#define HIST_SIZE 128
#define SIZE_AREA 5

/*---Струкутура образца-------------*/
/*---s: Стандартное отклонение------*/
/*---Xmid: средневзвешенная яркость-*/
/*---x,y: координаты образца--------*/
struct Swatch
{
	double s;
	double Xmid;
	int x;
	int y;
};

/*---Добавить цветное изображение---*/
/*---Вход: имя изображения----------*/
/*---Выход: ничего------------------*/
/*----------------------------------*/
int AddImageInBase(char* fileName);

/*---Расскрасить изображение--------*/
/*---Вход: имя изображения----------*/
/*---Выход: ничего------------------*/
/*----------------------------------*/
IplImage* ColorizeImage(char* fileName); 

/*---Получить сигнатуру изображения-*/
/*---Вход: указатель на изображение-*/
/*---Выход: сигнатура---------------*/
/*----------------------------------*/
void GetSignature(IplImage* img,double* signature);

/*-----Сравнение сигнатур-----------*/
/*---корреляционным методом---------*/
/*---Вход: две сигнатуры------------*/
/*---Выход: результат сравнения-----*/
/*----------------------------------*/
double CompareSigntatures(double * first, double * second);

/*---Найти похожее сообщение--------*/
/*---Вход: изображение--------------*/
/*---Выход: номер похожего----------*/
/*---изображения в бд---------------*/
/*----------------------------------*/
int GetColorImage(char *fileName);

/*---Получить образцы с изображения-*/
/*---Вход: изображение--------------*/
/*---Выход: массив образцов---------*/
/*----------------------------------*/
int GetSwatches(IplImage* img,Swatch *swatches);

/*---Вывести информацию об ошибке---*/
/*---Вход: ничего-------------------*/
/*---Выход: сообщение---------------*/
/*----------------------------------*/
void ErrorMessage(); 

int main(int argc, char* argv[])
{
	IplImage *img;
	setlocale(0,".1251");
	if(argc==3)
	{
		if(!strcmp(argv[1],"-add"))
		{
			AddImageInBase(argv[2]);
		}
		else if (!strcmp(argv[1],"-colorize"))
		{
			IplImage* resultImg;
			resultImg=ColorizeImage(argv[2]);
		}
		else
		{
			printf("Invalid first parameter");
			return -1;
		}
	}
	else
	{
		printf("Invalid count of parameters");
		return -1;
	}
	cvWaitKey();
	return 0;
}


int AddImageInBase(char* fileName)
{
DWORD nBytesRead,nBytesWritten; //прочитано и записано байт
	DWORD nFileSize;			//размер файла
	DWORD nImageCount;			//количество изображений в базе
	HANDLE hFile;				//дескриптор файла
	double signature[HIST_SIZE];
	char imageFileName[20],tmp[10];		//имя изображения для сохранения и промежуточная строковая переменная
	IplImage* img;				//изображение для добавления
	
	img=cvLoadImage(fileName);
	if(!img)
	{
		ErrorMessage();
		return -1;
	}
	GetSignature(img,signature);
	/*--------Добавляем изображение в базу---*/
	if(CreateDirectoryA("base",NULL))//директория
	{
		printf("base was created");
	}
	hFile=CreateFileA("base\\base.dat",GENERIC_READ|GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) //файл
	{
		ErrorMessage(); 
		return -1;
	}
	nFileSize=GetFileSize(hFile,NULL);
	if(!nFileSize)
	{
		printf("File is empty\n");
		nImageCount=0;
	}
	else
	{
 		ReadFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesRead,NULL);
	}
	nImageCount++;
	printf("%d",nImageCount);
	strcpy(imageFileName,"base\\");
	itoa(nImageCount,tmp,10);
	strcat(imageFileName,tmp);
	strcat(imageFileName,".png");
	cvSaveImage(imageFileName,img);
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	WriteFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesWritten,NULL);
	SetFilePointer(hFile,0,NULL,FILE_END);
	for(int i=0;i<HIST_SIZE;i++)
	{
		WriteFile(hFile,&signature[i],sizeof(double),&nBytesWritten,NULL);
	}
	CloseHandle(hFile);
	cvReleaseImage(&img);

	return 0;

}


IplImage* ColorizeImage(char* fileName)
{
	IplImage *targetImage;
	IplImage *sourceImage,*resizeSourceImage;
	IplImage *labTarget, *labSource;
	IplImage *l_planeTarget,*a_planeTarget,*b_planeTarget; 
	IplImage *l_planeSource,*a_planeSource,*b_planeSource; 
	char imgFileName[20],tmp[10];
	Swatch swatches[500];

	int res=GetColorImage(fileName);
	strcpy(imgFileName,"base\\");
	itoa(res,tmp,10);
	strcat(imgFileName,tmp);
	strcat(imgFileName,".png");
	targetImage=cvLoadImage(fileName);
	if(!targetImage)
	{
		ErrorMessage();
		return NULL;
	}
	resizeSourceImage=cvCreateImage(cvGetSize(targetImage),8,3);
	sourceImage=cvLoadImage(imgFileName);
	if(!sourceImage)
	{
		ErrorMessage();
		return NULL;
	}
	cvResize(sourceImage,resizeSourceImage);
	labTarget=cvCreateImage(cvGetSize(targetImage),8,3);
	l_planeTarget = cvCreateImage( cvGetSize(targetImage), 8, 1 );
	a_planeTarget = cvCreateImage( cvGetSize(targetImage), 8, 1 );
	b_planeTarget = cvCreateImage( cvGetSize(targetImage), 8, 1 );
	cvCvtColor(targetImage,labTarget,CV_BGR2Lab);
	cvCvtPixToPlane(labTarget,l_planeTarget,a_planeTarget,b_planeTarget,0);
	labSource=cvCreateImage(cvGetSize(resizeSourceImage),8,3);
	l_planeSource = cvCreateImage( cvGetSize(resizeSourceImage), 8, 1 );
	a_planeSource = cvCreateImage( cvGetSize(resizeSourceImage), 8, 1 );
	b_planeSource = cvCreateImage( cvGetSize(resizeSourceImage), 8, 1 );
	cvCvtColor(resizeSourceImage,labSource,CV_BGR2Lab);
	cvCvtPixToPlane(labSource,l_planeSource,a_planeSource,b_planeSource,0);
	IplImage* lTar=cvCloneImage(l_planeSource);
	int **mask;
	mask = new int*[labTarget->height];
	for(int i=0;i<labTarget->width;i++)
	{
		mask[i]=new int[labTarget->width];
	}
	for( int i=0; i<labTarget->height; i++ ) 
	{
		for( int j=0 ; j<labTarget->width; j++ ) 
		{
			mask[i][j]=0;
		}
	}
	int countSwatch=GetSwatches(l_planeSource,swatches);
	int count128source=0;
	int count128target=0;
	for( int y=0; y<labTarget->height; y++ ) 
	{
		uchar* ptrSourcea = (uchar*) (a_planeSource->imageData) + (y) * a_planeSource->widthStep;
		for( int x=0; x<labTarget->width; x++ ) 
		{
			if(ptrSourcea[x]==128)
			{
				count128source++;
			}
		}
	}
	printf("128 source: %d\n",count128source);

	for( int y=0; y<labTarget->height; y++ ) 
	{
		uchar* ptrTargeta = (uchar*) (a_planeTarget->imageData) + y * a_planeTarget->widthStep;
		uchar* ptrTargetb = (uchar*) (b_planeTarget->imageData) + y * b_planeTarget->widthStep;
		for( int x=0; x<labTarget->width; x++ ) 
		{
			double maxS=0.1;
			double maxXmid=0.1;
			for(int k=0;k<countSwatch;k++)
			{
				if(swatches[k].s>maxS)
				{
					maxS=swatches[k].s;
				}
				if(swatches[k].Xmid>maxXmid)
				{
					maxXmid=swatches[k].Xmid;
				}
			}

			double XmidT=0;
			double St=0;
			for(int  m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<SIZE_AREA;p++)
				{
					uchar *ptrTargetl=(uchar*) (l_planeTarget->imageData) + (y+m) * l_planeSource->widthStep;
					XmidT+=ptrTargetl[x+p];
				}
			}
			XmidT/=SIZE_AREA*SIZE_AREA;
			for(int m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<=SIZE_AREA;p++)
				{
					uchar *ptrTargetl=(uchar*) (l_planeTarget->imageData) + (y+m) * l_planeSource->widthStep;
					St+=pow(ptrTargetl[x+p]-XmidT,2);
				}
			}
			St/=SIZE_AREA*SIZE_AREA-1;
			St=sqrt(St);
			if(St>maxS)
			{
				maxS=St;
			}
			St/=maxS;
			if(XmidT>maxXmid)
			{
				maxXmid=XmidT;
			}
			XmidT/=maxXmid;
			Swatch newSwatches[500];
			for(int k=0;k<countSwatch;k++)
			{
				newSwatches[k].s=swatches[k].s/maxS;
				newSwatches[k].Xmid=swatches[k].Xmid/maxXmid;
				newSwatches[k].x=swatches[k].x;
				newSwatches[k].y=swatches[k].y;
			}
			int indexSwatch=0;
			double minDistance=2;
			for(int k=0;k<countSwatch;k++)
			{
				
				if(abs(newSwatches[k].s-St) <minDistance)
				{
					minDistance=abs(newSwatches[k].s-St);
					indexSwatch=k;
				}
			}
			if(minDistance<0.01)
			{
				for(int m=0;m<SIZE_AREA;m++)
				{
					//int m=0;
					uchar* ptrSourcea = (uchar*) (a_planeSource->imageData) + (newSwatches[indexSwatch].y+m) * a_planeSource->widthStep;
					uchar* ptrSourceb = (uchar*) (b_planeSource->imageData) + (newSwatches[indexSwatch].y+m) * b_planeSource->widthStep;
					uchar* ptrTargeta = (uchar*) (a_planeTarget->imageData) + (y+m) * a_planeTarget->widthStep;
					uchar* ptrTargetb = (uchar*) (b_planeTarget->imageData) + (y+m) * b_planeTarget->widthStep;
					for(int p=0;p<=SIZE_AREA;p++)
				 	{
 						mask[y+m][x+p]=1;
						ptrTargeta[x+p]=ptrSourcea[newSwatches[indexSwatch].x+p];
						ptrTargetb[x+p]=ptrSourceb[newSwatches[indexSwatch].x+p];
						if(ptrTargeta[x]==128 && ptrTargetb[x]==128)
						{
							count128target++;
						}
					}
				}
			}
		}
	}
	printf("128 target: %d\n",count128target );
	int countBefore=0;
	for( int i=0; i<labTarget->height; i++ ) 
	{
		for( int j=0 ; j<labTarget->width; j++ ) 
		{
			if(!mask[i][j])
			{
				countBefore++;
			}
		}
	}
	printf("\nBefore: %d\n",countBefore);
	int countSom=0;
	int x=l_planeTarget->widthStep;
	int indexX,indexY;
	uchar* ptrTargetl = (uchar*) (l_planeTarget->imageData);
	uchar* ptrTargeta = (uchar*) (a_planeTarget->imageData);
	uchar* ptrTargetb = (uchar*) (b_planeTarget->imageData);
	for(int param=0;param<0;param++)
	{
		for( int i=0; i<labTarget->height; i++ ) 
		{
			for( int j=0 ; j<labTarget->width; j++ ) 
			{
				if(mask[i][j]==0 && i>=2 && j>=2 && (ptrTargeta+x*i)[j]==128 && (ptrTargetb+x*i)[j]==128)
				{
					bool findIt=false;
					int distance;
					for(int k=-2;k<=2;k++)
					{
						distance=50;
						for(int l=-2;l<=2;l++)
						{
							if(mask[i+k][j+l]==1)
							{
								int tmpDist=abs((ptrTargetl+x*i)[j]-(ptrTargetl+x*(i-1))[j-1]);
								if(tmpDist<distance)
								{
									distance=tmpDist;
									indexX=i+k;
									indexY=j+l;
									findIt=true;
								}
							}
						}
					}
					if(findIt && distance<40)
					{
						mask[i][j]=2;
						(ptrTargeta+x*i)[j]=(ptrTargeta+x*indexX)[indexY];
						(ptrTargetb+x*i)[j]=(ptrTargetb+x*indexX)[indexY];
						countSom++;
					}
				}
			}
		}
		for( int i=0; i<labTarget->height; i++ ) 
		{
			for( int j=0 ; j<labTarget->width; j++ ) 
			{
				if(mask[i][j]==2)
				{
					mask[i][j]=1;
				}
			}
		}
	}
	int countAfter=0;
	for( int i=0; i<labTarget->height; i++ ) 
	{
		for( int j=0 ; j<labTarget->width; j++ ) 
		{
			if(!mask[i][j])
			{
				countAfter++;
			}
		}
	}
	printf("Count: %d\n",countSom);
	printf("After: %d\n",countAfter);
	cvNamedWindow("lt");
	cvShowImage("lt",l_planeTarget);
	cvNamedWindow("ls");
	cvShowImage("ls",l_planeSource);
	cvNamedWindow("at");
	cvShowImage("at",a_planeTarget);
	cvNamedWindow("bt");
	cvShowImage("bt",b_planeTarget);
	cvNamedWindow("as");
	cvShowImage("as",a_planeSource);
	cvNamedWindow("bs");
	cvShowImage("bs",b_planeSource);
	cvCvtPlaneToPix(l_planeTarget,a_planeTarget,b_planeTarget,0,labTarget);
	cvCvtColor(labTarget,targetImage,CV_Lab2BGR);
	cvNamedWindow("colorImage");
	cvShowImage("colorImage",targetImage);
	cvSaveImage("result.jpg",targetImage);
}


int GetSwatches(IplImage* img,Swatch *swatches)
{
	CvRNG rng = cvRNG(0xffffffff);
	int index=0;
	for(int y=0;y<img->height-img->height/20-SIZE_AREA;y+=(double)(img->height/20))
	{
		for(int x=0;x<img->width-img->width/20-SIZE_AREA;x+=(double)(img->width/20))
		{
			swatches[index].x =x+ cvRandInt(&rng)%(img->width/20);
			swatches[index].y =y+ cvRandInt(&rng)%(img->height/20);
			swatches[index].Xmid=0;
			swatches[index].s=0;
			for(int m=0;m<SIZE_AREA;m++)
			{
				uchar *ptrSourcel=(uchar*) (img->imageData) + (swatches[index].y+m) * img->widthStep;
				for(int p=0;p<SIZE_AREA;p++)
				{
					swatches[index].Xmid+=ptrSourcel[swatches[index].x+p];
				}
			}
			swatches[index].Xmid/=SIZE_AREA*SIZE_AREA;
			for(int m=0;m<SIZE_AREA;m++)
			{
				uchar *ptrSourcel=(uchar*) (img->imageData) + (swatches[index].y+m) * img->widthStep;
				for(int p=0;p<SIZE_AREA;p++)
				{
					swatches[index].s+=pow(ptrSourcel[swatches[index].x+p]-swatches[index].Xmid,2.0);
				}
			}
			swatches[index].s/=SIZE_AREA*SIZE_AREA-1;
			swatches[index].s=sqrt(swatches[index].s);
			index++;

		}
	}
	return index;
}


int GetColorImage(char *fileName)
{
	DWORD nBytesRead;
	DWORD nImageCount;
	double imgSignature[HIST_SIZE];
	double imgSignatureBD[HIST_SIZE];
	int result=0;
	IplImage *img;
	HANDLE hFile;
	double** signatures; 
    img=cvLoadImage(fileName);
	if(!img)
	{
		ErrorMessage(); 
		return NULL;
	}
	hFile=CreateFileA("base\\base.dat",GENERIC_READ,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) //файл
	{
		ErrorMessage(); 
		return NULL;
	}
	ReadFile(hFile,&nImageCount,sizeof(nImageCount),&nBytesRead,NULL);
	printf("\n%d\n",nImageCount);
	signatures=new double*[nImageCount];
	for(int i=0;i<nImageCount;i++)
	{
		signatures[i]=new double[HIST_SIZE];
		for(int j=0;j<HIST_SIZE;j++)
		{
			double currentBin;
 			ReadFile(hFile,&currentBin,sizeof(currentBin),&nBytesRead,NULL);
			signatures[i][j]=currentBin;
		}
	}
	double resCompare=0;
	GetSignature(img,imgSignature);
	for(int i=0;i<nImageCount;i++)
	{
		printf("%d: %.4lf\n",i,CompareSigntatures(imgSignature,signatures[i]));
		if(resCompare<CompareSigntatures(imgSignature,signatures[i]))
		{
			resCompare=CompareSigntatures(imgSignature,signatures[i]);
			result=i;
		}
	}
	cvReleaseImage(&img);
	return result+1;
}


void GetSignature(IplImage* img,double* signature)
{
	float range_0[]={0,256};	//значения гистограммы
	float* ranges[] = { range_0 }; //массив значений
	int hist_size = HIST_SIZE;	//размер гистограммы
	CvHistogram *hist;			//гистограмма
	IplImage* innerImg,*l_plane,*a_plane,*b_plane; //изображения для разделения на каналы
	innerImg=cvCreateImage(cvGetSize(img),8,3);
	l_plane = cvCreateImage( cvGetSize(innerImg), 8, 1 );
	a_plane = cvCreateImage( cvGetSize(innerImg), 8, 1 );
	b_plane = cvCreateImage( cvGetSize(innerImg), 8, 1 );
	cvCvtColor(img,innerImg,CV_BGR2Lab);
	cvCvtPixToPlane(innerImg,l_plane,a_plane,b_plane,0);
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	cvCalcHist( &l_plane, hist, 0, NULL );
	cvNormalizeHist(hist,1.0);
	for(int i=0;i<HIST_SIZE;i++)
	{
		signature[i]=cvGetReal1D(hist->bins,i);
	}

	cvReleaseHist(&hist);
	cvReleaseImage(&innerImg);
	cvReleaseImage(&l_plane);
	cvReleaseImage(&a_plane);
	cvReleaseImage(&b_plane);
}


double CompareSigntatures(double * first, double * second)
{
	double d=0;
	int sizeH=HIST_SIZE;
	float range_0[]={0,256};	//значения гистограммы
	float* ranges[] = { range_0 }; //массив значений
	CvHistogram *hist1= cvCreateHist(1, &sizeH, CV_HIST_ARRAY, ranges, 1);
	CvHistogram *hist2= cvCreateHist(1, &sizeH, CV_HIST_ARRAY, ranges, 1);
	
	for(int i=0;i<HIST_SIZE;i++)
	{
		cvSetReal1D(hist1->bins,i,first[i]);
		cvSetReal1D(hist2->bins,i,second[i]);
	}
	d=cvCompareHist(hist1,hist2,CV_COMP_CORREL);
	return d;
}


void ErrorMessage() 
{ 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*) &lpMsgBuf,
        0, NULL );
    printf("%s\n",lpMsgBuf);
    LocalFree(lpMsgBuf);
}