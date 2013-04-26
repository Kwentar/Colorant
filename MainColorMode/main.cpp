#include <Windows.h>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <locale.h>
#include "Constant.h"
#include "ImageDataBase.h"



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

/*---Удалить цветное изображение---*/
/*---Вход: имя изображения----------*/
/*---Выход: ничего------------------*/
/*----------------------------------*/
int DeleteImageFromBase(char* fileName);

/*---Расскрасить изображение--------*/
/*---Вход: имя изображения----------*/
/*---Выход: ничего------------------*/
/*----------------------------------*/
IplImage* ColorizeImage(char* fileName); 

/*---Перенести цвет с одного--------*/
/*----изображения на другое---------*/
/*---Вход: имена изображений--------*/
/*---Выход: окрашенное изображение--*/
/*----------------------------------*/
IplImage* SwapColor(char* fileNameTarget, char* fileNameSource); 

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
		else if(!strcmp(argv[1],"-delete"))
		{
			DeleteImageFromBase(argv[2]);			
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
	DWORD nImageCount;			//количество изображений в базе
	HANDLE hFile;				//дескриптор файла
	double signature[HIST_SIZE];
	char imageFileName[20],tmp[10];		//имя изображения для сохранения и промежуточная строковая переменная
	IplImage* img;				//изображение для добавления
	ImageDataBase _db;
	img=cvLoadImage(fileName);
	if(!img)
	{
		ErrorMessage();
		return -1;
	}
	GetSignature(img,signature);
	int number=_db.addImage(signature);
	strcpy(imageFileName,"base\\");
	itoa(number,tmp,10);
	strcat(imageFileName,tmp);
	strcat(imageFileName,".png");
	cvSaveImage(imageFileName,img);
	cvReleaseImage(&img);
	return 0;

}

int DeleteImageFromBase(char* fileName)
{
	ImageDataBase _db;
	_db.deleteImage(fileName);
	if(!DeleteFileA(fileName))
	{
		ErrorMessage();
		int i;
		scanf("%d",&i);
	}
	return 0;
}

IplImage* ColorizeImage(char* fileName)
{
	char imgFileName[20],tmp[10];
	int res=GetColorImage(fileName);
	printf("выбранное: %d\n",res);
	strcpy(imgFileName,"base\\");
	itoa(res,tmp,10);
	strcat(imgFileName,tmp);
	strcat(imgFileName,".png");
	SwapColor(fileName,imgFileName);
	return 0;
}

IplImage* SwapColor(char* fileNameTarget, char* fileNameSource)
{
	IplImage *targetImg,*sourceImg;
	IplImage *labTarget,*labSource;
	IplImage *l_planeTargetNorm,*l_planeSourceNorm;
	IplImage *resizeSourceImg;
	IplImage *l_planeTarget=0,*a_planeTarget=0,*b_planeTarget=0;
	IplImage *l_planeSource=0,*a_planeSource=0,*b_planeSource=0;
	Swatch swatches[GRID_SIZE*GRID_SIZE*2];
	int countSwatch;
	//load images
	targetImg=cvLoadImage(fileNameTarget);
	sourceImg=cvLoadImage(fileNameSource);
	if(!targetImg || !sourceImg)
	{
		ErrorMessage();
		return NULL;
	}
	cvSaveImage("source.jpg",sourceImg);
	//resize source image
	resizeSourceImg=cvCreateImage(cvGetSize(targetImg),sourceImg->depth,sourceImg->nChannels);
	cvResize(sourceImg,resizeSourceImg);
	//get channels both images
	labTarget=cvCreateImage(cvGetSize(targetImg),8,3);
	l_planeTarget=cvCreateImage(cvGetSize(targetImg),8,1);
	a_planeTarget=cvCreateImage(cvGetSize(targetImg),8,1);
	b_planeTarget=cvCreateImage(cvGetSize(targetImg),8,1);
	cvCvtColor(targetImg,labTarget,CV_BGR2Lab);
	cvCvtPixToPlane(labTarget,l_planeTarget,a_planeTarget,b_planeTarget,0);

	labSource=cvCreateImage(cvGetSize(resizeSourceImg),8,3);
	l_planeSource=cvCreateImage(cvGetSize(resizeSourceImg),8,1);
	a_planeSource=cvCreateImage(cvGetSize(resizeSourceImg),8,1);
	b_planeSource=cvCreateImage(cvGetSize(resizeSourceImg),8,1);
	cvCvtColor(resizeSourceImg,labSource,CV_BGR2Lab);
	cvCvtPixToPlane(labSource,l_planeSource,a_planeSource,b_planeSource,0);

	//Equalization images
	l_planeSourceNorm=cvCreateImage(cvGetSize(l_planeSource),8,1);
	l_planeTargetNorm=cvCreateImage(cvGetSize(l_planeTarget),8,1);
	cvEqualizeHist(l_planeSource,l_planeSourceNorm);
	cvEqualizeHist(l_planeTarget,l_planeTargetNorm);

	//get swatches
	countSwatch=GetSwatches(l_planeSourceNorm,swatches);

	//colorize
	for(int y=0;y<l_planeTarget->height;y++)
	{
		for(int x=0;x<l_planeTarget->width;x++)
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

			//get MO and dispers
			double XmidT=0;
			double St=0;
			for(int  m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<SIZE_AREA;p++)
				{
					XmidT+=Pixel(l_planeTargetNorm,y+m,x+p);
				}
			}
			XmidT/=SIZE_AREA*SIZE_AREA;
			for(int m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<=SIZE_AREA;p++)
				{
					St+=pow(Pixel(l_planeTargetNorm,y+m,x+p)-XmidT,2);
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
			Swatch newSw[GRID_SIZE*GRID_SIZE*2];
			for(int k=0;k<countSwatch;k++)
			{
				newSw[k].s=swatches[k].s/maxS;
				newSw[k].Xmid=swatches[k].Xmid/maxXmid;
				newSw[k].x=swatches[k].x;
				newSw[k].y=swatches[k].y;
			}

			double minDistance=20;
			int indexSwatch=0;
			for(int k=0;k<countSwatch;k++)
			{
				if(minDistance>=abs(newSw[k].Xmid-XmidT)+abs(newSw[k].s-St))
				{
					minDistance=abs(newSw[k].Xmid-XmidT)+abs(newSw[k].s-St);
					indexSwatch=k;
				}
			}

			Pixel(a_planeTarget,y,x)=Pixel(a_planeSource,newSw[indexSwatch].y,newSw[indexSwatch].x);
			Pixel(b_planeTarget,y,x)=Pixel(b_planeSource,newSw[indexSwatch].y,newSw[indexSwatch].x);
		}
	}

	cvCvtPlaneToPix(l_planeTarget,a_planeTarget,b_planeTarget,0,labTarget);
	cvCvtColor(labTarget,targetImg,CV_Lab2BGR);

	cvSaveImage("result.jpg",targetImg);

	return targetImg;
}


int GetSwatches(IplImage* img,Swatch *swatches)
{
	CvRNG rng = cvRNG(0xffffffff);
	int index=0;
	for(int y=0;y<img->height-img->height/GRID_SIZE-SIZE_AREA;y+=(double)(img->height/GRID_SIZE))
	{
		for(int x=0;x<img->width-img->width/GRID_SIZE-SIZE_AREA;x+=(double)(img->width/GRID_SIZE))
		{
			swatches[index].x =x+ cvRandInt(&rng)%(img->width/20);
			swatches[index].y =y+ cvRandInt(&rng)%(img->height/20);
			swatches[index].Xmid=0;
			swatches[index].s=0;
			for(int m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<SIZE_AREA;p++)
				{
					swatches[index].Xmid+=Pixel(img,y+m,x+p);
				}
			}
			swatches[index].Xmid/=SIZE_AREA*SIZE_AREA;
			for(int m=0;m<SIZE_AREA;m++)
			{
				for(int p=0;p<SIZE_AREA;p++)
				{
					swatches[index].s+=pow(Pixel(img,y+m,x+p)-swatches[index].Xmid,2.0);
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
	double resCompare=-1;
	IplImage *img;
	HANDLE hFile;
    img=cvLoadImage(fileName);
	if(!img)
	{
		ErrorMessage(); 
		return NULL;
	}
	ImageDataBase _db;
	ImageInDB* images;
	nImageCount=_db.getCountImages();
	images=new ImageInDB[nImageCount];
	_db.getImages(images);
	GetSignature(img,imgSignature);
	for(int i=0;i<nImageCount;i++)
	{
		printf("%d: %.4lf\n",i,CompareSigntatures(imgSignature,images[i].signature));
		if(resCompare<CompareSigntatures(imgSignature,images[i].signature))
		{
			resCompare=CompareSigntatures(imgSignature,images[i].signature);
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