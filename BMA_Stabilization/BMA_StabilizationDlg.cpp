
// BMA_StabilizationDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "BMA_Stabilization.h"
#include "BMA_StabilizationDlg.h"
#include "afxdialogex.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include<opencv/CvvImage.h>


#define referenceBlockSize 128
#define currentBlockSize 64


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FILE *fp = fopen("c:\\RESULT\\Shift_Width.txt", "wt");
FILE *fp2 = fopen("c:\\RESULT\\Shift_Hight.txt", "wt");
FILE *fp3 = fopen("c:\\RESULT\\Motion_Vector.txt", "wt");

int frameCount = 0;
int stabilizationCount = 0;


CDC	*pDCCurrent;
CWnd *pWndCurrent;
CvvImage cImageCurrent;
CvCapture* pCaptureCurrent;//cv capture point

CDC	*pDCReference;
CWnd *pWndReference;
CvvImage cImageReference;
CvCapture* pCaptureReference;//cv capture point

//CvCapture* pCaptureSling;//cv capture point

IplImage* referenceImage;
IplImage* currentImage;
IplImage* stabilizationImage;
//IplImage* slidingImage;


IplImage* reference1;
IplImage* reference2;
IplImage* reference3;
IplImage* reference4;
IplImage* reference5;
IplImage* reference6;
IplImage* reference7;
IplImage* reference8;
IplImage* reference9;
IplImage* reference10;
IplImage* reference11;
IplImage* reference12;

IplImage* current1;
IplImage* current2;
IplImage* current3;
IplImage* current4;
IplImage* current5;
IplImage* current6;
IplImage* current7;
IplImage* current8;
IplImage* current9;
IplImage* current10;
IplImage* current11;
IplImage* current12;




// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBMA_StabilizationDlg ��ȭ ����



CBMA_StabilizationDlg::CBMA_StabilizationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBMA_StabilizationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBMA_StabilizationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBMA_StabilizationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CBMA_StabilizationDlg::OnBnClickedBUTTON_OPEN)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CBMA_StabilizationDlg::OnBnClickedBUTTON_RUN)
END_MESSAGE_MAP()


// CBMA_StabilizationDlg �޽��� ó����

BOOL CBMA_StabilizationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	pWndReference = this->GetDlgItem(IDC_STATIC1);
	pDCReference = pWndReference->GetDC();
	pDCReference->GetSafeHdc();

	pWndCurrent = this->GetDlgItem(IDC_STATIC2);
	pDCCurrent = pWndCurrent->GetDC();
	pDCCurrent->GetSafeHdc();


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CBMA_StabilizationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CBMA_StabilizationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		cImageReference.CopyOf(referenceImage);
		cImageReference.Show(pDCReference->GetSafeHdc(), 0, 0, 640, 480);
		cImageCurrent.CopyOf(currentImage);
		cImageCurrent.Show(pDCCurrent->GetSafeHdc(), 0, 0, 640, 480);
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CBMA_StabilizationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBMA_StabilizationDlg::OnBnClickedBUTTON_OPEN()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char title[] = { "Open Image" };//set title name
	CString path;
	double currentFrame;
	//double slingFrame;
	CFileDialog dlg(TRUE, _T("*.bmp"), "", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "AVI files (*.avi) |*.avi|All Files (*.*)|*.*||", NULL);//open image
	dlg.m_ofn.lpstrTitle = title;//title name

	if (dlg.DoModal() == IDOK)
	{
		path = dlg.GetPathName(); // contain the selected filename  
	}//get path

	pCaptureCurrent = cvCaptureFromFile(path);//get point//������ ������ �ҷ����µ� ����ϴ� �Լ��̴�.
	pCaptureReference = cvCaptureFromFile(path);
	//	pCaptureSling = cvCaptureFromFile(path);

	currentFrame = cvGetCaptureProperty(pCaptureCurrent, CV_CAP_PROP_POS_FRAMES);//ȹ��� ������ ��ȣ<���� ������ ��ȣ>
	cvSetCaptureProperty(pCaptureCurrent, CV_CAP_PROP_POS_FRAMES, currentFrame + 1);//���� ������ ���� �ҷ�����


	//	slingFrame = cvGetCaptureProperty(pCaptureSling, CV_CAP_PROP_POS_FRAMES);
	//	cvSetCaptureProperty(pCaptureSling, CV_CAP_PROP_POS_FRAMES, slingFrame + 10);

	if (referenceImage = cvQueryFrame(pCaptureReference))//������ ���Ͽ��� ������ ������ ������ ������. cvcapture ����ü ���ο� �Ҵ�Ǿ� �ִ� �޸� ���� ���. ������ �������� ������ if�� ����.
	{
		cImageReference.CopyOf(referenceImage); //IplImage����ü �����͸� CvvImage�� ����
		cImageReference.Show(pDCReference->GetSafeHdc(), 0, 0, 640, 480);//0,0���� 640X480ũ��� �׸�
	}
	if (currentImage = cvQueryFrame(pCaptureCurrent))
	{
		cImageCurrent.CopyOf(currentImage);
		cImageCurrent.Show(pDCCurrent->GetSafeHdc(), 0, 0, 640, 480);
	}


	CPaintDC dc(this);//�ڵ����� dc����
	Invalidate();

}

void CBMA_StabilizationDlg::OnBnClickedBUTTON_RUN()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//	IplImage* slidingGrayImage = cvCreateImage(cvGetSize(referenceImage), IPL_DEPTH_8U ,1);

	int height, width;
	int frameNum = 0;
	IplImage* cannyImage;

	uchar* referenceImageData = 0;
	uchar* currentImageData = 0;
	uchar* slidingImageData = 0;

	height = referenceImage->height;
	width = referenceImage->width;

	int flag = 0;

	int gmvCount = 0;

		int gmvWidth[3] = {0};
		int gmvHeight[3] = {0};
	stabilizationImage = cvCreateImage(cvGetSize(referenceImage), IPL_DEPTH_8U, 1);//Ipl ����ü �޸𸮸� �����Ͽ� ������ ��ȯ.(size, depth<�̹����� ǥ���ϴ� ��Ʈ�� ũ��>, channel<1~4>),

	for (int totalFrame = 0;; totalFrame++)
	{
		//	fprintf(fp, "\n<<<<<<%d ������>>>>>>>\n", totalFrame);
		int pointLMV = 0;
		IplImage* referenceGrayImage = cvCreateImage(cvGetSize(referenceImage), IPL_DEPTH_8U, 1);
		IplImage* currentGrayImage = cvCreateImage(cvGetSize(referenceImage), IPL_DEPTH_8U, 1);
		/*
		if (referenceImage = cvQueryFrame(pCaptureReference))
		{
		cImageReference.CopyOf(referenceImage);
		cImageReference.Show(pDCReference->GetSafeHdc(), 0, 0,640,480);
		}
		if (currentImage = cvQueryFrame(pCaptureCurrent))
		{
		cImageCurrent.CopyOf(currentImage);
		cImageCurrent.Show(pDCCurrent->GetSafeHdc(), 0, 0,640,480);
		}*/



		if (totalFrame == 0)
		{
			if (referenceImage = cvQueryFrame(pCaptureReference))
			{
				cImageReference.CopyOf(referenceImage);
				cImageReference.Show(pDCReference->GetSafeHdc(), 0, 0, 640, 480);
			}
			if (currentImage = cvQueryFrame(pCaptureCurrent))
			{
				cImageCurrent.CopyOf(currentImage);
				cImageCurrent.Show(pDCCurrent->GetSafeHdc(), 0, 0, 640, 480);
			}
		}
		else
		{
			//cvCopyImage(stabilizationImage, referenceImage);
			referenceImage = stabilizationImage;

			if (currentImage = cvQueryFrame(pCaptureCurrent))
			{
				cImageCurrent.CopyOf(currentImage);
				cImageCurrent.Show(pDCCurrent->GetSafeHdc(), 0, 0, 640, 480);
			}
		}

		if (!currentImage)
		{
			break;
		}
		flag++;

		//�׷��̷� ��ȯ
		cvCvtColor(currentImage, currentGrayImage, CV_BGR2GRAY);//currentimage�� ������� ��ȯ
		currentGrayImage->origin = currentImage->origin;//origin==>������ ����. 0�� top-left, 1�� bottom left

		cvCvtColor(referenceImage, referenceGrayImage, CV_BGR2GRAY);
		referenceGrayImage->origin = referenceImage->origin;

		referenceImageData = (uchar *)referenceGrayImage->imageData;//pixel data. ù �ȼ� �����ʹ� imageData[0]
		currentImageData = (uchar *)currentGrayImage->imageData;


		uchar* currentData = 0;
		uchar* referenceData = 0;

		int BMDiffer = 0;
		int referHeightIni = 0, referWidthIni = 0;
		int firFlag = 0;
		int saveDiffer = 0;

		int lmvH[12] = { 0 };
		int lmvW[12] = { 0 };



		int referenceHeight = 128;
		int referenceWidth = 128;
		int currentHeight = 64;
		int currentWidth = 64;

		int iteration = ((referenceBlockSize - currentBlockSize) + 1) * ((referenceBlockSize - currentBlockSize) + 1);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CString savePath;
		uchar* referCmp = 0;
		uchar* curCmp = 0;
		int usingBlock = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		int pixelDiff = 0;
		int accReferPixel = 0;
		int accCurPixel = 0;

		uchar *cannyData = 0;
		int white = 0, black = 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		for (int i = 56, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 57, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j]; //pixel ��
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);//MAD:MAD�� �ּ��� �� (i,j)�� local motion vector
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference1 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(25, 24, referenceBlockSize, referenceBlockSize));//���� block���� ����.rect(x,y,width,height)
			cvCopyImage(referenceGrayImage, reference1);

			current1 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(57, 56, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current1);

			cannyImage = cvCreateImage(cvGetSize(current1), IPL_DEPTH_8U, 1);
			cvCanny(current1, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}
			if (white >= 0)
			{
				currentData = (uchar *)current1->imageData;
				referenceData = (uchar *)reference1->imageData;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));

						}
					}
					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference1);
			cvReleaseImage(&current1);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 56, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 210, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];

				//pixelDiff =  pixelDiff + abs((curCmp[i*width+j] - referCmp[i*width+j]));

			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference2 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(178, 24, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference2);

			current2 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(210, 56, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current2);

			cannyImage = cvCreateImage(cvGetSize(current2), IPL_DEPTH_8U, 1);
			cvCanny(current2, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current2->imageData;
				referenceData = (uchar *)reference2->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference2);
			cvReleaseImage(&current2);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);

		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 56, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 363, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{

				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
				//pixelDiff =  pixelDiff + abs((curCmp[i*width+j] - referCmp[i*width+j]));

			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);
		if (pixelDiff >= 0)
		{
			reference3 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(331, 24, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference3);

			current3 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(363, 56, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current3);

			cannyImage = cvCreateImage(cvGetSize(current3), IPL_DEPTH_8U, 1);
			cvCanny(current3, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current3->imageData;
				referenceData = (uchar *)reference3->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference3);
			cvReleaseImage(&current3);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 56, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 516, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference4 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(484, 24, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference4);

			current4 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(516, 56, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current4);

			cannyImage = cvCreateImage(cvGetSize(current4), IPL_DEPTH_8U, 1);
			cvCanny(current4, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current4->imageData;
				referenceData = (uchar *)reference4->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);

						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}

				usingBlock++;
			}
			cvReleaseImage(&reference4);
			cvReleaseImage(&current4);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 208, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 57, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference5 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(25, 176, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference5);

			current5 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(57, 208, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current5);

			cannyImage = cvCreateImage(cvGetSize(current5), IPL_DEPTH_8U, 1);
			cvCanny(current5, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current5->imageData;
				referenceData = (uchar *)reference5->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}

					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference5);
			cvReleaseImage(&current5);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 208, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 210, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference6 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(178, 176, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference6);

			current6 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(210, 208, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current6);

			cannyImage = cvCreateImage(cvGetSize(current6), IPL_DEPTH_8U, 1);
			cvCanny(current6, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current6->imageData;
				referenceData = (uchar *)reference6->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);

						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference6);
			cvReleaseImage(&current6);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 208, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 363, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference7 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(331, 176, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference7);

			current7 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(363, 208, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current7);

			cannyImage = cvCreateImage(cvGetSize(current7), IPL_DEPTH_8U, 1);
			cvCanny(current7, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current7->imageData;
				referenceData = (uchar *)reference7->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);

						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference7);
			cvReleaseImage(&current7);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 208, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 516, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference8 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(484, 176, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference8);

			current8 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(516, 208, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current8);

			cannyImage = cvCreateImage(cvGetSize(current8), IPL_DEPTH_8U, 1);
			cvCanny(current8, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current8->imageData;
				referenceData = (uchar *)reference8->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}

					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference8);
			cvReleaseImage(&current8);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);

		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 360, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 57, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference9 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(25, 328, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference9);

			current9 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(57, 360, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current9);

			cannyImage = cvCreateImage(cvGetSize(current9), IPL_DEPTH_8U, 1);
			cvCanny(current9, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current9->imageData;
				referenceData = (uchar *)reference9->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference9);
			cvReleaseImage(&current9);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 360, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 210, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference10 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(178, 328, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference10);

			current10 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(210, 360, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current10);

			cannyImage = cvCreateImage(cvGetSize(current10), IPL_DEPTH_8U, 1);
			cvCanny(current10, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current10->imageData;
				referenceData = (uchar *)reference10->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}
					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&current10);
			cvReleaseImage(&reference10);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 360, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 363, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference11 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels); \
				cvSetImageROI(referenceGrayImage, cvRect(331, 328, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference11);

			current11 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(363, 360, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current11);

			cannyImage = cvCreateImage(cvGetSize(current11), IPL_DEPTH_8U, 1);
			cvCanny(current11, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current11->imageData;
				referenceData = (uchar *)reference11->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));

						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}

					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference11);
			cvReleaseImage(&current11);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pixelDiff = 0;
		accReferPixel = 0;
		accCurPixel = 0;

		cannyImage = 0;
		white = 0;
		black = 0;

		referCmp = (uchar *)referenceGrayImage->imageData;
		curCmp = (uchar *)currentGrayImage->imageData;

		for (int i = 360, p = 1; p <= 64; i++, p++)                      //����
		{
			for (int j = 516, q = 1; q <= 64; j++, q++)                   //�ʺ�
			{
				accReferPixel = accReferPixel + referCmp[i*width + j];
				accCurPixel = accCurPixel + curCmp[i*width + j];
			}
		}
		pixelDiff = (abs(accReferPixel - accCurPixel)) / (64 * 64);;
		//fprintf(fp, "pixel diff : %d\n", pixelDiff);

		if (pixelDiff >= 0)
		{
			reference12 = cvCreateImage(cvSize(referenceBlockSize, referenceBlockSize), referenceGrayImage->depth, referenceGrayImage->nChannels);
			cvSetImageROI(referenceGrayImage, cvRect(484, 328, referenceBlockSize, referenceBlockSize));
			cvCopyImage(referenceGrayImage, reference12);

			current12 = cvCreateImage(cvSize(currentBlockSize, currentBlockSize), currentGrayImage->depth, currentGrayImage->nChannels);
			cvSetImageROI(currentGrayImage, cvRect(516, 360, currentBlockSize, currentBlockSize));
			cvCopyImage(currentGrayImage, current12);

			cannyImage = cvCreateImage(cvGetSize(current12), IPL_DEPTH_8U, 1);
			cvCanny(current12, cannyImage, 100, 150, 3);
			cannyData = (uchar *)cannyImage->imageData;

			for (int i = 0; i<currentBlockSize; i++)
			{
				for (int j = 0; j<currentBlockSize; j++)
				{
					if (cannyData[i*currentBlockSize + j] > 127)
					{
						white++;
					}
					else if (cannyData[i*currentBlockSize + j] <= 127)
					{
						black++;
					}
				}
			}

			if (white >= 0)
			{
				currentData = (uchar *)current12->imageData;
				referenceData = (uchar *)reference12->imageData;

				BMDiffer = 0;
				referHeightIni = 0, referWidthIni = 0;
				firFlag = 0;
				saveDiffer = 0;

				for (int counting = 1; counting <= iteration; counting++)
				{
					BMDiffer = 0;
					for (int curHeight = 0, referHeight = referHeightIni; curHeight <= (currentBlockSize - 1); curHeight++, referHeight++)                      //����
					{
						for (int curWidth = 0, referWidth = referWidthIni; curWidth <= (currentBlockSize - 1); curWidth++, referWidth++)                   //�ʺ�
						{
							BMDiffer = BMDiffer + ((referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth])
								* (referenceData[referHeight*referenceWidth + referWidth] - currentData[curHeight*currentWidth + curWidth]));
						}
					}

					if (firFlag == 0)
					{
						saveDiffer = BMDiffer;
					}
					else
					{
						if (saveDiffer > BMDiffer)
						{
							saveDiffer = BMDiffer;
							lmvW[usingBlock] = referWidthIni - ((referenceBlockSize - currentBlockSize) / 2);
							lmvH[usingBlock] = referHeightIni - ((referenceBlockSize - currentBlockSize) / 2);
						}

					}
					firFlag++;
					if (counting % ((referenceBlockSize - currentBlockSize) + 1) == 0)
					{
						referHeightIni++;
						referWidthIni = 0;
					}
					else
					{
						referWidthIni++;
					}
				}
				usingBlock++;
			}
			cvReleaseImage(&reference12);
			cvReleaseImage(&current12);
			cvReleaseImage(&cannyImage);
			cvResetImageROI(referenceGrayImage);
			cvResetImageROI(currentGrayImage);
		}
		frameCount++;

		int resultWidth = 0;
		int resultHeigh = 0;
		int shiftWidth = 0;
		int shiftHeight = 0;

		if (usingBlock > 2)
		{
			int temp = 0;
			for (int i = 0; i<usingBlock; i++)
			{
				for (int j = 0; j<(usingBlock - 1); j++)
				{
					if (lmvH[j] > lmvH[j + 1])
					{
						temp = lmvH[j];
						lmvH[j] = lmvH[j + 1];
						lmvH[j + 1] = temp;
					}
				}
			}

			temp = 0;

			for (int i = 0; i<usingBlock; i++)
			{
				for (int j = 0; j<(usingBlock - 1); j++)
				{
					if (lmvW[j] > lmvW[j + 1])
					{
						temp = lmvW[j];
						lmvW[j] = lmvW[j + 1];
						lmvW[j + 1] = temp;
					}
				}
			}

			resultWidth = 0;
			resultHeigh = 0;
			/*
			for(int i=1 ; i < (usingBlock-1) ; i++)
			{
			resultWidth = resultWidth + lmvW[i];
			}

			for(int i=1 ; i < (usingBlock-1) ; i++)
			{
			resultHeigh = resultHeigh + lmvH[i];
			}
			shiftWidth= resultWidth / (usingBlock - 2);
			shiftHeight = resultHeigh / (usingBlock - 2);*/

			for (int i = 0; i < (usingBlock); i++)
			{
				resultWidth = resultWidth + lmvW[i];
			}

			for (int i = 0; i < (usingBlock); i++)
			{
				resultHeigh = resultHeigh + lmvH[i];
			}
			shiftWidth = resultWidth / (usingBlock);
			shiftHeight = resultHeigh / (usingBlock);


		}
		else if (usingBlock < 2 && usingBlock > 0)
		{
			for (int i = 0; i < 12; i++)
			{
				shiftWidth = resultWidth + lmvW[i];
				shiftHeight = resultHeigh + lmvH[i];
			}

		}
		else if (usingBlock == 0)
		{
			shiftWidth = 0;
			shiftHeight = 0;
		}

		fprintf(fp, "%d\n", shiftWidth);
		fprintf(fp2, "%d\n", shiftHeight);
		fprintf(fp3, "Motion Vector : (%d, %d)", shiftWidth, shiftHeight);
		fprintf(fp3, "frame%d ~ frame%d �Ϸ�\n\n", frameNum, frameNum + 1);
		frameNum++;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		int accShiftWidth = 0;
		int accShiftHeight = 0;
		int averageGMVH;
		int averageGMVW;

		if(gmvCount <= 2)
		{
		gmvWidth[gmvCount] = shiftWidth;
		gmvHeight[gmvCount] = shiftHeight;
		gmvCount++;
		}
		else
		{
		for(int i = 0 ; i<=1 ; i++)
		{
		gmvWidth[i] = gmvWidth[i+1];
		gmvHeight[i] = gmvHeight[i+1];
		}
		gmvWidth[2] = shiftWidth;
		gmvHeight[2] = shiftHeight;
		}

		for(int i = 0 ; i <= 2 ; i++)
		{
		accShiftWidth = accShiftWidth + gmvWidth[i];
		accShiftHeight = accShiftHeight + gmvHeight[i];
		}
		averageGMVW = accShiftWidth / gmvCount;
		averageGMVH = accShiftHeight / gmvCount;

		//		shiftWidth = averageGMVW;
		//	shiftHeight = averageGMVH;

		shiftWidth = averageGMVW - shiftWidth;
		shiftHeight = averageGMVH - shiftHeight;

			fprintf(fp, "GMV(%d, %d)\n", shiftWidth, shiftHeight);


		//	fprintf(fp, "���� ����� ���� : %d\n", usingBlock);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CvPoint2D32f ptSource[4], ptPrespective[4];

		stabilizationImage = cvCreateImage(cvGetSize(currentImage), currentImage->depth, currentImage->nChannels);

		if (shiftHeight > 0 && shiftWidth > 0)
		{
			ptSource[0] = cvPoint2D32f(0, 0);
			ptSource[1] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), 0);
			ptSource[2] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), ((currentImage->height) - abs(shiftHeight)));
			ptSource[3] = cvPoint2D32f(0, ((currentImage->height) - abs(shiftHeight)));

			ptPrespective[0] = cvPoint2D32f(abs(shiftWidth), abs(shiftHeight));
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, abs(shiftHeight));
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(abs(shiftWidth), stabilizationImage->height);
		}
		else if (shiftHeight < 0 && shiftWidth < 0)
		{
			ptSource[0] = cvPoint2D32f(abs(shiftWidth), abs(shiftHeight));
			ptSource[1] = cvPoint2D32f(currentImage->width, abs(shiftHeight));
			ptSource[2] = cvPoint2D32f(currentImage->width, currentImage->height);
			ptSource[3] = cvPoint2D32f(abs(shiftWidth), currentImage->height);

			ptPrespective[0] = cvPoint2D32f(0, 0);
			ptPrespective[1] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), 0);
			ptPrespective[2] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), ((stabilizationImage->height) - abs(shiftHeight)));
			ptPrespective[3] = cvPoint2D32f(0, ((stabilizationImage->height) - abs(shiftHeight)));
		}
		else if (shiftHeight > 0 && shiftWidth < 0)
		{
			ptSource[0] = cvPoint2D32f(abs(shiftWidth), 0);
			ptSource[1] = cvPoint2D32f(currentImage->width, 0);
			ptSource[2] = cvPoint2D32f(currentImage->width, ((currentImage->height) - abs(shiftHeight)));
			ptSource[3] = cvPoint2D32f(abs(shiftWidth), ((currentImage->height) - abs(shiftHeight)));

			ptPrespective[0] = cvPoint2D32f(0, abs(shiftHeight));
			ptPrespective[1] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), abs(shiftHeight));
			ptPrespective[2] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(0, stabilizationImage->height);
		}
		else if (shiftHeight < 0 && shiftWidth > 0)
		{
			ptSource[0] = cvPoint2D32f(0, abs(shiftHeight));
			ptSource[1] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), abs(shiftHeight));
			ptSource[2] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), currentImage->height);
			ptSource[3] = cvPoint2D32f(0, currentImage->height);

			ptPrespective[0] = cvPoint2D32f(abs(shiftWidth), 0);
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, 0);
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, ((stabilizationImage->height) - abs(shiftHeight)));
			ptPrespective[3] = cvPoint2D32f(abs(shiftWidth), ((stabilizationImage->height) - abs(shiftHeight)));
		}
		else if (shiftHeight == 0 && shiftWidth > 0)
		{
			ptSource[0] = cvPoint2D32f(0, 0);
			ptSource[1] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), 0);
			ptSource[2] = cvPoint2D32f(((currentImage->width) - abs(shiftWidth)), currentImage->height);
			ptSource[3] = cvPoint2D32f(0, currentImage->height);

			ptPrespective[0] = cvPoint2D32f(abs(shiftWidth), 0);
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, 0);
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(abs(shiftWidth), stabilizationImage->height);
		}
		else if (shiftHeight == 0 && shiftWidth < 0)
		{
			ptSource[0] = cvPoint2D32f(abs(shiftWidth), 0);
			ptSource[1] = cvPoint2D32f(currentImage->width, 0);
			ptSource[2] = cvPoint2D32f(currentImage->width, currentImage->height);
			ptSource[3] = cvPoint2D32f(abs(shiftWidth), currentImage->height);

			ptPrespective[0] = cvPoint2D32f(0, 0);
			ptPrespective[1] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), 0);
			ptPrespective[2] = cvPoint2D32f(((stabilizationImage->width) - abs(shiftWidth)), stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(0, stabilizationImage->height);
		}

		else if (shiftHeight > 0 && shiftWidth == 0)
		{
			ptSource[0] = cvPoint2D32f(0, 0);
			ptSource[1] = cvPoint2D32f(currentImage->width, 0);
			ptSource[2] = cvPoint2D32f(currentImage->width, ((currentImage->height) - abs(shiftHeight)));
			ptSource[3] = cvPoint2D32f(0, ((currentImage->height) - abs(shiftHeight)));

			ptPrespective[0] = cvPoint2D32f(0, abs(shiftHeight));
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, abs(shiftHeight));
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(0, stabilizationImage->height);
		}
		else if (shiftHeight < 0 && shiftWidth == 0)
		{
			ptSource[0] = cvPoint2D32f(0, abs(shiftHeight));
			ptSource[1] = cvPoint2D32f(currentImage->width, abs(shiftHeight));
			ptSource[2] = cvPoint2D32f(currentImage->width, currentImage->height);
			ptSource[3] = cvPoint2D32f(0, currentImage->height);

			ptPrespective[0] = cvPoint2D32f(0, 0);
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, 0);
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, (currentImage->height - abs(shiftHeight)));
			ptPrespective[3] = cvPoint2D32f(0, ((stabilizationImage->height) - abs(shiftHeight)));
		}
		else
		{
			ptSource[0] = cvPoint2D32f(0, 0);
			ptSource[1] = cvPoint2D32f(currentImage->width, 0);
			ptSource[2] = cvPoint2D32f(currentImage->width, currentImage->height);
			ptSource[3] = cvPoint2D32f(0, currentImage->height);

			ptPrespective[0] = cvPoint2D32f(0, 0);
			ptPrespective[1] = cvPoint2D32f(stabilizationImage->width, 0);
			ptPrespective[2] = cvPoint2D32f(stabilizationImage->width, stabilizationImage->height);
			ptPrespective[3] = cvPoint2D32f(0, stabilizationImage->height);
		}
		float newm[9];
		CvMat mat = cvMat(3, 3, CV_32F, newm);  // ��ȯ��� �ʱ�ȭ
		cvWarpPerspectiveQMatrix(ptSource, ptPrespective, &mat);  // ��ȯ��� ���ϱ�

		// �̹��� ����
		cvWarpPerspective(currentImage, stabilizationImage, &mat, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

		CString stabilizationPath;
		stabilizationPath.Format("c:\\RESULT\\%d.bmp", stabilizationCount);
		cvSaveImage(stabilizationPath, stabilizationImage);
		stabilizationCount++;


		cvReleaseImage(&referenceGrayImage);
		cvReleaseImage(&currentGrayImage);
		cvReleaseImage(&cannyImage);
	}
	cvReleaseImage(&stabilizationImage);
	/*cvReleaseImage(&referenceGrayImage );
	cvReleaseImage(&currentGrayImage );
	cvReleaseImage(&slidingGrayImage );
	*/
	AfxMessageBox("�Ϸ�!!");
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
}
