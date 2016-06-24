
// Path planningDlg.cpp : ��@��
//

#include "stdafx.h"
#include "Path planning.h"
#include "Path planningDlg.h"
#include "afxdialogex.h"
#include "fstream"
#include <windows.h>
#include "Voronoi.cpp"
#include "queue"
#include "CvvImage.cpp"
#include "direct.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPathplanningDlg ��ܤ��



CPathplanningDlg::CPathplanningDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PATHPLANNING_DIALOG, pParent)
	, m_coner_count(0)
	, m_total_time(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPathplanningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_show, m_show);
	DDX_Text(pDX, IDC_STATIC_coner, m_coner_count);
	DDV_MinMaxInt(pDX, m_coner_count, 0, 99999);
	DDX_Text(pDX, IDC_STATIC_time, m_total_time);
	DDX_Control(pDX, IDC_STATIC_show2, m_show2);
}

BEGIN_MESSAGE_MAP(CPathplanningDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Start, &CPathplanningDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CPathplanningDlg �T���B�z�`��

BOOL CPathplanningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
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

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w

	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

void CPathplanningDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CPathplanningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CPathplanningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void DoEvents()
{
	MSG msg;
	BOOL result;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		result = ::GetMessage(&msg, NULL, 0, 0);
		if (result == 0) // WM_QUIT
		{
			::PostQuitMessage(msg.wParam);
			break;
		}
		else if (result == -1)
		{
			// Handle errors/exit application, etc.
		}
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

int d[2000];       // �����_�I��U���I���̵u���|����
int parent[2000];  // �����U���I�b�̵u���|��W�����ˬO��
bool visit[2000];  // �����U���I�O���O�w�b�̵u���|�𤧤�
int w[2000][2000];    // �@�i���v������
vector <int> show_path;

void CPathplanningDlg::OnBnClickedButtonStart()
{

	CWnd* CW_vo = (CWnd *)GetDlgItem(IDC_STATIC_show2);
	CDC* pDC2 = CW_vo->GetWindowDC();


	remove("���|��X.txt");

	IplConvKernel *pKernel_small = NULL;
	IplConvKernel *pKernel_small2 = NULL;
	pKernel_small = cvCreateStructuringElementEx(11, 11, 6, 6, CV_SHAPE_RECT, NULL);
	vector<vector<bool>>  sca_image;  //�Y�ϫ�G�Ȥƪ����G
	vector <int> path_optimization;
	vector <Point> save_coner;
	vector <CPoint> all_point_map;
	vector <CvPoint2D64f> all_point_map_original;
	int corner_count = 0;
	int line_count = 0;  //���h��VD�u�q
	int new_input_index = 0;
	bool image_change = 0;
	double Data[8000];
	//	template <double T, size_t N>
	CElement* pElement = 0;
	CvPoint2D32f *point2 = 0;
	CvPoint2D64f savepoint1[3000] = { 0.0 }, savepoint2[3000] = { 0.0 };
	CvPoint2D64f new_savepoint1[3000] = { 0.0 }, new_savepoint2[3000] = { 0.0 };
	LARGE_INTEGER tStart, tEnd, ts;

	IplImage * pGrayImg = NULL;
	IplImage * draw_data = NULL;
	IplImage * check_change = NULL;
	IplImage * show_data = NULL; //�Y�p�Q�����x�}
	IplImage * read_data_old = NULL;
	IplImage * read_data = NULL;
	IplImage * resize_data = NULL;


	read_data = cvLoadImage("path\\L0.png", 0);
	read_data_old = cvCreateImage(cvGetSize(read_data), read_data->depth, 1);
	pGrayImg = cvCreateImage(cvGetSize(read_data), read_data->depth, 1);
	draw_data = cvCreateImage(cvGetSize(read_data), read_data->depth, 3);
	check_change = cvCreateImage(cvGetSize(read_data), read_data->depth, 1);
	show_data = cvCreateImage(cvSize(40, 40), IPL_DEPTH_8U, 1);

	resize_data = cvCreateImage(cvSize(880, 880), read_data->depth, 3);

	char read_name[100];
	char write_name[100];
	int photo_conunt = 0;

	CvPoint start_point, end_point;
	start_point.x = 140;  //���|�_�l�P���I�A�аѷӹϤ����w
	start_point.y = 399;
	end_point.x = 399;
	end_point.y = 20;

	remove("photo2");
	_mkdir("photo2");

	while (true)
	{
		QueryPerformanceFrequency(&ts);
		QueryPerformanceCounter(&tStart);

		sprintf_s(read_name, "path\\L%d.png", photo_conunt);
		sprintf_s(write_name, "photo2\\R%d.png", photo_conunt);
		fstream in_image0(read_name, ios::in);
		photo_conunt++;

		if (!in_image0)
			break;

		read_data = cvLoadImage(read_name, 0);


		cvAbsDiff(read_data, read_data_old, check_change);
		cvResize(read_data, read_data_old, CV_INTER_NN);
		cvResize(check_change, show_data, CV_INTER_NN);

		image_change = 0;
		for (int i = 0; i < show_data->height; i++)
		{
			for (int j = 0; j < show_data->width; j++)
			{
				int something = ((uchar *)(show_data->imageData + i*show_data->widthStep))[j];  //�����X�ݡA����
//				int something = cvGet2D(show_data, i, j).val[0];  //�����X�ݡA���C
				if (something != 0)
				{
					image_change = 1;
					break;
				}
			}
			if (image_change)
				break;
			else
				image_change = 0;
		}



		if (1)
		{
			//-------------------------�M���ƾ�------------------------------
			save_coner.clear();
			sca_image.clear();
			path_optimization.clear();
			all_point_map.clear();
			all_point_map_original.clear();
			show_path.clear();
			memset((unsigned char*)draw_data->imageData, 0, draw_data->imageSize);
			memset(d, 0, sizeof(d));
			memset(parent, 0, sizeof(parent));
			memset(visit, 0, sizeof(visit));
			memset(w, 0, sizeof(w));

			//---------------------------------------------------------------------


			cvResize(read_data, pGrayImg, CV_INTER_NN);//Ū�¥ռv���Ϊ�
	//		cvCvtColor(read_data, pGrayImg, CV_RGB2GRAY);  //Ū�m��v���Ϊ�
#if 0
			cvErode(pGrayImg, pGrayImg, pKernel_small, 2);  //�I�k���ۤ�(�]���O�թ�)
			cvDilate(pGrayImg, pGrayImg, pKernel_small, 1);  //���Ȫ��ۤ�
#else
			cvDilate(pGrayImg, pGrayImg, pKernel_small, 1);  //�I�k
			cvErode(pGrayImg, pGrayImg, pKernel_small, 2);  //����
#endif	

			cvCvtColor(pGrayImg, draw_data, CV_GRAY2RGB);
			//			cvSaveImage("���s�q����Ϊ�.bmp", pGrayImg);

						//�ƭȭn�̾��Y�p���v�P���Ipixel�ƨM�w


			cvResize(pGrayImg, show_data, CV_INTER_NN);


			//��J�Ϥ��A��X�G�ȸ��
			binarization(show_data, sca_image);
			//��J�G�ȸ�ơA��X���I
			find_coner(sca_image, save_coner, 2);
			//�N���I�ഫ���ǳƭn��JVoronoi�B�⪺�榡
			trans2Voronoi(sca_image, save_coner, Data, 4);
			//�p��U�qVoronoi�A��J���I��ƻP��ɡA��X��ӯx�}
			Voronoi_calculate(Data, show_data->width, show_data->height, savepoint1, savepoint2, line_count);
			//�p��s�qVoronoi�A�ݧ�
			Generalized_Voronoi(sca_image, savepoint1, savepoint2, line_count, new_input_index, new_savepoint1, new_savepoint2);
			//VD�I�|�}�H�A�N�䭫�s�E�X
			Match_point(line_count, new_input_index, new_savepoint1, new_savepoint2, 2);
			//Dijkstra���|�j�M�A��J�I�s����T��ƶq
			Dijkstra_path_planning(start_point, end_point, new_savepoint1, new_savepoint2, new_input_index, all_point_map, all_point_map_original);
			//���|�u�ơA��J�G�ȸ�T�P�쥻���|
			Path_Optimization(sca_image, all_point_map_original, path_optimization);



			//-------------------------------------------ø��---------------------------------------

			for (int i = 0; i < save_coner.size(); i++)  //���I��
			{
				cvLine(draw_data, cvPoint(save_coner[i].x * 10, save_coner[i].y * 10), cvPoint(save_coner[i].x * 10, save_coner[i].y * 10), CV_RGB(0, 250, 250), 8);
			}
			for (int i = 0; i < line_count; i++)   //VD��
			{
				cvLine(draw_data, cvPoint(savepoint1[i].x * 10, savepoint1[i].y * 10), cvPoint(savepoint2[i].x * 10, savepoint2[i].y * 10), CV_RGB(0, 0, 255), 1);
			}
			for (int i = 0; i < new_input_index; i++)  //GVD��
			{
				cvLine(draw_data, cvPoint(new_savepoint1[i].x * 10, new_savepoint1[i].y * 10), cvPoint(new_savepoint2[i].x * 10, new_savepoint2[i].y * 10), CV_RGB(250, 100, 100), 2);
			}

			for (int path_index = 0; path_index < show_path.size() - 1; path_index++) //�e�X���|��
			{
				cvLine(draw_data, cvPoint(all_point_map[show_path[path_index]].x, all_point_map[show_path[path_index]].y), cvPoint(all_point_map[show_path[path_index + 1]].x, all_point_map[show_path[path_index + 1]].y), CV_RGB(0, 0, 255), 3);
			}

			for (int path_opt = 0; path_opt < path_optimization.size() - 1; path_opt++) //�e�X���|�u�ƹ�
			{

				cvLine(draw_data, cvPoint(all_point_map[path_optimization[path_opt]].x, all_point_map[path_optimization[path_opt]].y), cvPoint(all_point_map[path_optimization[path_opt + 1]].x, all_point_map[path_optimization[path_opt + 1]].y), CV_RGB(0, 150, 0), 3);

				cvCircle(draw_data, cvPoint(all_point_map[path_optimization[path_opt]].x, all_point_map[path_optimization[path_opt]].y), 7, CV_RGB(0, 150, 0), 2);

			}

			cvSaveImage(write_name, draw_data);
			//			m_show2.SetWindowPos(&wndTop, 10, 10, draw_data->width, draw_data->height, SWP_SHOWWINDOW);
			cvResize(draw_data, resize_data);

			CvvImage show1;
			show1.CopyOf(resize_data);
			show1.Show(*pDC2, 0, 0, resize_data->width, resize_data->height);

			//-------------------------------------------ø��---------------------------------------

//  			IplImage * itest2 = NULL;
//  			IplImage * itest = NULL;
//  			itest2 = cvCreateImage(cvSize(40, 40), IPL_DEPTH_8U, 1);
//  			itest = cvCreateImage(cvSize(40, 40), IPL_DEPTH_8U, 3);
//  			cvResize(read_data, itest2, CV_INTER_NN);
//  			cvCvtColor(itest2, itest, CV_GRAY2RGB);
//  
//  			for (int i = 0; i < save_coner.size(); i++)  //���I��
//  			{
//  				cvLine(itest, cvPoint(save_coner[i].x , save_coner[i].y ), cvPoint(save_coner[i].x, save_coner[i].y), CV_RGB(0, 250, 250), 1);
//  			}
//  			for (int i = 0; i < line_count; i++)   //VD��
//  			{
//  				cvLine(itest, cvPoint(savepoint1[i].x, savepoint1[i].y), cvPoint(savepoint2[i].x, savepoint2[i].y), CV_RGB(250, 200, 100), 1);
//  			}
//  			for (int i = 0; i < new_input_index; i++)  //GVD��
//  			{
//  				cvLine(itest, cvPoint(new_savepoint1[i].x, new_savepoint1[i].y), cvPoint(new_savepoint2[i].x, new_savepoint2[i].y), CV_RGB(250, 100, 100), 1);
//  			}
//  
//  			for (int path_index = 0; path_index < show_path.size() - 1; path_index++) //�e�X���|��
//  			{
//  				cvLine(itest, cvPoint(all_point_map[show_path[path_index]].x, all_point_map[show_path[path_index]].y), cvPoint(all_point_map[show_path[path_index + 1]].x, all_point_map[show_path[path_index + 1]].y), CV_RGB(0, 0, 255), 1);
//  			}
//  
//  			for (int path_opt = 0; path_opt < path_optimization.size() - 1; path_opt++) //�e�X���|�u�ƹ�
//  			{
//  
//  				cvLine(itest, cvPoint(all_point_map[path_optimization[path_opt]].x, all_point_map[path_optimization[path_opt]].y), cvPoint(all_point_map[path_optimization[path_opt + 1]].x, all_point_map[path_optimization[path_opt + 1]].y), CV_RGB(0, 150, 0), 1);
//  
//  				cvCircle(itest, cvPoint(all_point_map[path_optimization[path_opt]].x, all_point_map[path_optimization[path_opt]].y), 7, CV_RGB(0, 150, 0), 1);
//  
//  			}
//  
//  //			cvSaveImage("��X.png", itest);
//  			cvSaveImage(write_name, itest);
//  			cvReleaseImage(&itest);

			//-------------------------------------------ø��---------------------------------------
		}



		memset((unsigned char*)show_data->imageData, 0, show_data->imageSize);
		cvReleaseImage(&read_data);



		QueryPerformanceCounter(&tEnd);

		m_total_time = 1000 / ((tEnd.QuadPart - tStart.QuadPart) * 1000 / (double)(ts.QuadPart));
		m_coner_count = Data[0] + 1; //��ܨ��I�ƶq
		UpdateData(FALSE);



		DoEvents();
	}


}


void CPathplanningDlg::binarization(IplImage * i_show_data, vector<vector<bool>> &o_sca_image2)
{
	vector<bool> sca_image1;

	for (int y = 0; y < i_show_data->height - 1; y++)
	{
		for (int x = 0; x < i_show_data->width - 1; x++)
		{
			int temp_of_image = cvGetReal2D(i_show_data, y, x);

			if (temp_of_image == 255)
				sca_image1.push_back(1);
			else
				sca_image1.push_back(0);

			if (x == i_show_data->width - 2)
				sca_image1.push_back(1);
		}
		o_sca_image2.push_back(sca_image1);
		sca_image1.clear();
	}

	for (int x = 0; x < i_show_data->width; x++)
	{
		sca_image1.push_back(1);
	}

	o_sca_image2.push_back(sca_image1);
	sca_image1.clear();

}

void CPathplanningDlg::find_coner(vector<vector<bool>> i_sca_image, vector <Point> &o_save_coner, int i_Interpolation)
{
	int result_out;
	int Laplacian_mask[3][3] = { { 0, -1, 0 },{ -1, 4, -1 },{ 0, -1, 0 } };
	int Interpolation = 0;

	for (int y = 1; y < i_sca_image.size() - 1; y++)
	{
		for (int x = 1; x < i_sca_image[0].size() - 1; x++)
		{

			result_out =
				Laplacian_mask[0][0] * i_sca_image[y - 1][x - 1] +
				Laplacian_mask[0][1] * i_sca_image[y - 1][x] +
				Laplacian_mask[0][2] * i_sca_image[y - 1][x + 1] +
				Laplacian_mask[1][0] * i_sca_image[y][x - 1] +
				Laplacian_mask[1][1] * i_sca_image[y][x] +
				Laplacian_mask[1][2] * i_sca_image[y][x + 1] +
				Laplacian_mask[2][0] * i_sca_image[y + 1][x - 1] +
				Laplacian_mask[2][1] * i_sca_image[y + 1][x] +
				Laplacian_mask[2][2] * i_sca_image[y + 1][x + 1];


			if (result_out == 1)
			{
				Interpolation++;
				if (Interpolation == i_Interpolation)
				{
					o_save_coner.push_back(cvPoint(x, y));
					Interpolation = 0;
				}
			}
			else
				Interpolation = 0;


			if (Laplacian_mask[0][1] * i_sca_image[y - 1][x] + Laplacian_mask[2][1] * i_sca_image[y + 1][x] == -2)  //�o�����u
				continue;
			if (Laplacian_mask[1][0] * i_sca_image[y][x - 1] + Laplacian_mask[1][2] * i_sca_image[y][x + 1] == -2)  //�o����u
				continue;

			if (result_out == 2 || result_out == 3)
				o_save_coner.push_back(cvPoint(x, y));
		}
	}


	for (int x = 1; x < i_sca_image[0].size() - 1; x++)
	{
		for (int y = 1; y < i_sca_image.size() - 1; y++)
		{

			result_out =
				Laplacian_mask[0][0] * i_sca_image[y - 1][x - 1] +
				Laplacian_mask[0][1] * i_sca_image[y - 1][x] +
				Laplacian_mask[0][2] * i_sca_image[y - 1][x + 1] +
				Laplacian_mask[1][0] * i_sca_image[y][x - 1] +
				Laplacian_mask[1][1] * i_sca_image[y][x] +
				Laplacian_mask[1][2] * i_sca_image[y][x + 1] +
				Laplacian_mask[2][0] * i_sca_image[y + 1][x - 1] +
				Laplacian_mask[2][1] * i_sca_image[y + 1][x] +
				Laplacian_mask[2][2] * i_sca_image[y + 1][x + 1];


			if (result_out == 1)
			{
				Interpolation++;
				if (Interpolation == i_Interpolation)
				{
					o_save_coner.push_back(cvPoint(x, y));
					Interpolation = 0;
				}
			}
			else
			{
				Interpolation = 0;
			}

		}
	}

}

void CPathplanningDlg::trans2Voronoi(vector<vector<bool>> i_sca_image, vector<Point> i_save_coner, double(&o_Data)[8000], int i_Interpolation2)
{
	int input_Data = 0;
	int jump_count = 0;
	jump_count = (i_sca_image.size() + 1) / i_Interpolation2 + 1;

	o_Data[0] = i_save_coner.size();
	for (input_Data = 1; input_Data < i_save_coner.size() + 1; input_Data++)
	{
		//		cvLine(RGB_show_data, cvPoint(save_coner[input_Data - 1].x, save_coner[input_Data - 1].y),cvPoint(save_coner[input_Data - 1].x, save_coner[input_Data - 1].y), CV_RGB(0, 255, 0), 1);
		o_Data[2 * input_Data - 1] = i_save_coner[input_Data - 1].x;
		o_Data[2 * input_Data] = i_save_coner[input_Data - 1].y;
	}



	for (int i = 0; i <= i_sca_image.size() + 1; i = i + i_Interpolation2)
	{
		for (int j = 0; j <= i_sca_image[0].size() + 1; j = j + i_Interpolation2)
		{
			if (i == 0 || i == i_sca_image.size() || j == 0 || j == i_sca_image[0].size())
			{

				o_Data[2 * input_Data - 1] = i-1;
				o_Data[2 * input_Data] = j ;
				input_Data++;
				o_Data[0]++;
				//				cvLine(RGB_show_data, cvPoint(i, j),cvPoint(i, j), CV_RGB(200, 200, 0), 1);
			}
		}
	}
}

void CPathplanningDlg::Voronoi_calculate(double i_Data[8000], int x_boundary, int y_boundary, CvPoint2D64f(&o_savepoint1)[3000], CvPoint2D64f(&o_savepoint2)[3000], int &o_line_count)
{
	remove("��lVD�y�п�X.txt");
	fstream app_VD_output2("��lVD�y�п�X.txt", ios::app);

	CWnd* CW_vo = (CWnd *)GetDlgItem(IDC_STATIC_show);
	CDC* pDC = CW_vo->GetWindowDC();
	CVoronoi* vor;
	vor = new CVoronoi();
	POSITION aPos;
	Site** EdgeSite;

	double *x1, *x2, *y1, *y2, let00 = 0, pxmax = x_boundary, pymax = -y_boundary, temp_y1, temp_x1, temp_x2, temp_y2, pymax_invers = y_boundary;
	int i_line_count = 0, line_count2 = 0;
	int pos;

	CPen aPen;
	aPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	pDC->SelectObject(&aPen);
	vor->SetPoints(i_Data);
	vor->DrawEdges(pDC, 0, 1);
	CList<Edge, Edge&> *VorEdgeList = vor->GetEdges();
	CList<Edge, Edge&> *VorLineList = vor->GetLines();
	if (!VorEdgeList->IsEmpty())
	{
		aPos = VorEdgeList->GetHeadPosition();
		do {
			Edge& aEdge = VorEdgeList->GetNext(aPos);
			pos = aEdge.edgenbr;
			x1 = &aEdge.ep[0]->coord.x;
			y1 = &aEdge.ep[0]->coord.y;
			x2 = &aEdge.ep[1]->coord.x;
			y2 = &aEdge.ep[1]->coord.y;

			if (aEdge.ep[0] && aEdge.ep[1])
			{
				app_VD_output2 << *x1 << ", " << *y1 << " �� " << *x2 << ", " << *y2 << ",�� " << line_count2 << endl;
				line_count2++;
			}

			if (!aEdge.ep[0] || *x1 < let00 || *y1 < let00)
			{
				if (aEdge.b != 0)
				{
					x1 = &let00;
					temp_y1 = (aEdge.c - aEdge.a*(*x1)) / aEdge.b;
					y1 = &temp_y1;
				}
				else
				{
					temp_x1 = aEdge.c / aEdge.a;
					x1 = &temp_x1;
					y1 = &let00;
				}
			}

			if (!aEdge.ep[1] || *y2 < let00 || *x2 < let00)
			{
				if (aEdge.b != 0)
				{
					x2 = &pxmax;
					temp_y2 = (aEdge.c - aEdge.a*(*x2)) / aEdge.b;
					y2 = &temp_y2;
				}
				else
				{
					temp_x2 = aEdge.c / aEdge.a;
					x2 = &temp_x2;
					y2 = &pymax;
				}
			}

			if (!*y1)
			{
				//				app_VD_output << (int)*x1 << ", " << (int)*y1 << " �� " << (int)*x2 << ", " << (int)*y2 << "--------y1�S��---------" << endl;
				y1 = &pymax_invers;
			}

			if ((int)*x1 == (int)*x2 && (int)*y1 == (int)*y2)
				continue;

			if (*y2 < 0)
				*y2 = 0;

			pDC->MoveTo((int)*x1, (int)*y1);
			pDC->LineTo((int)*x2, (int)*y2);


			o_savepoint1[i_line_count].x = (*x1);
			o_savepoint1[i_line_count].y = (*y1);
			o_savepoint2[i_line_count].x = (*x2);
			o_savepoint2[i_line_count].y = (*y2);
			i_line_count++;

		} while (aPos);

	}

	o_line_count = i_line_count;

	app_VD_output2.close();
	ReleaseDC(pDC);
	delete vor;

}

void CPathplanningDlg::Generalized_Voronoi(vector<vector<bool>> i_sca_image, CvPoint2D64f i_savepoint1[3000], CvPoint2D64f i_savepoint2[3000], int i_line_count, int &o_new_input_index, CvPoint2D64f(&o_new_savepoint1)[3000], CvPoint2D64f(&o_new_savepoint2)[3000])
{
	remove("�s�qVD�y�п�X.txt");
	fstream app_VD_output("�s�qVD�y�п�X.txt", ios::app);

	int cheak_point = 0, line_distant, new_input_index = 0;
	CvPoint2D64f cutout_point[2000];

	for (cheak_point = 0; cheak_point < i_line_count; cheak_point++)
	{
		if (i_savepoint1[cheak_point].x == 0 && i_savepoint1[cheak_point].y == 0 && i_savepoint2[cheak_point].x == 0 && i_savepoint2[cheak_point].y == 0)
			continue;

		line_distant = sqrt(pow(i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x, 2) + pow(i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y, 2));
		//		line_distant++;

		int cutout = 0;
		for (cutout = 0; cutout < line_distant; cutout++)
		{
			//�p��׽u�W���I

			double addddd = (sqrt(pow((i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x), 2)) / (double)line_distant)*(double)cutout;
			double addddd2 = (sqrt(pow((i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y), 2)) / (double)line_distant)*(double)cutout;

			if (i_savepoint1[cheak_point].x >= i_savepoint2[cheak_point].x && i_savepoint1[cheak_point].y >= i_savepoint2[cheak_point].y)
			{
				cutout_point[cutout].x = i_savepoint2[cheak_point].x + (sqrt(pow((i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x), 2)) / (double)line_distant)*(double)cutout;
				cutout_point[cutout].y = i_savepoint2[cheak_point].y + (sqrt(pow((i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y), 2)) / (double)line_distant)*(double)cutout;
			}

			if (i_savepoint2[cheak_point].x >= i_savepoint1[cheak_point].x && i_savepoint1[cheak_point].y >= i_savepoint2[cheak_point].y)
			{
				cutout_point[cutout].x = i_savepoint1[cheak_point].x + (sqrt(pow((i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x), 2)) / (double)line_distant)*(double)cutout;
				cutout_point[cutout].y = i_savepoint1[cheak_point].y - (sqrt(pow((i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y), 2)) / (double)line_distant)*(double)cutout;
			}

			if (i_savepoint1[cheak_point].x >= i_savepoint2[cheak_point].x && i_savepoint2[cheak_point].y >= i_savepoint1[cheak_point].y)
			{
				cutout_point[cutout].x = i_savepoint2[cheak_point].x + (sqrt(pow((i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x), 2)) / (double)line_distant)*(double)cutout;
				cutout_point[cutout].y = i_savepoint1[cheak_point].y - (sqrt(pow((i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y), 2)) / (double)line_distant)*(double)cutout;
			}

			if (i_savepoint2[cheak_point].x >= i_savepoint1[cheak_point].x && i_savepoint2[cheak_point].y >= i_savepoint1[cheak_point].y)
			{
				cutout_point[cutout].x = i_savepoint1[cheak_point].x + (sqrt(pow((i_savepoint1[cheak_point].x - i_savepoint2[cheak_point].x), 2)) / (double)line_distant)*(double)cutout;
				cutout_point[cutout].y = i_savepoint1[cheak_point].y + (sqrt(pow((i_savepoint1[cheak_point].y - i_savepoint2[cheak_point].y), 2)) / (double)line_distant)*(double)cutout;
			}
		}

		// 		if (cutout < 2)
		// 		{
		// 			if (i_sca_image[round(cutout_point[0].y)][round(cutout_point[0].x)] == 0 ||
		// 				i_sca_image[round(cutout_point[0].y + 1)][round(cutout_point[0].x)] == 0 ||
		// 				i_sca_image[round(cutout_point[0].y - 1)][round(cutout_point[0].x)] == 0 ||
		// 				i_sca_image[round(cutout_point[0].y)][round(cutout_point[0].x - 1)] == 0 ||
		// 				 i_sca_image[round(cutout_point[0].y)][round(cutout_point[0].x + 1)] == 0/**/)
		// 			{
		// 				o_new_savepoint1[new_input_index] = i_savepoint1[cheak_point];
		// 				o_new_savepoint2[new_input_index] = i_savepoint2[cheak_point];
		// 				new_input_index++;
		// 			}
		// 
		// 		}

		if (!line_distant)
		{
			o_new_savepoint1[new_input_index] = i_savepoint1[cheak_point];
			o_new_savepoint2[new_input_index] = i_savepoint2[cheak_point];

			app_VD_output << o_new_savepoint1[new_input_index].x * (double)10.0 << ", " << o_new_savepoint1[new_input_index].y * (double)10.0 << " �� " << o_new_savepoint2[new_input_index].x * (double)10.0 << ", " << o_new_savepoint2[new_input_index].y * (double)10.0 << ", �� " << new_input_index << endl;
			new_input_index++;
		}

		for (int i = 0; i < cutout - 0; i++)
		{
			if ((round(cutout_point[i].y) > i_sca_image.size() - 1) || (round(cutout_point[i].x) > i_sca_image.size() - 1) || (round(cutout_point[i].y) < 1 || (round(cutout_point[i].x) < 1)))
			{
				o_new_savepoint1[new_input_index] = i_savepoint1[cheak_point];
				o_new_savepoint2[new_input_index] = i_savepoint2[cheak_point];

				app_VD_output << o_new_savepoint1[i].x * (double)10.0 << ", " << o_new_savepoint1[i].y * (double)10.0 << " �� " << o_new_savepoint2[i].x * (double)10.0 << ", " << o_new_savepoint2[i].y * (double)10.0 << ", �� " << new_input_index << endl;
				new_input_index++;
				continue;
			}

			if (i_sca_image[round(cutout_point[i].y)][round(cutout_point[i].x)] == 1 /* ||
				i_sca_image[round(cutout_point[i].y + 1)][round(cutout_point[i].x)] == 1 ||
				i_sca_image[round(cutout_point[i].y - 1)][round(cutout_point[i].x)] == 1||
				i_sca_image[round(cutout_point[i].y)][round(cutout_point[i].x - 1)] == 1 ||
				 i_sca_image[round(cutout_point[i].y)][round(cutout_point[i].x + 1)] == 1*/)
			{
				break;
			}

			if (i == cutout - 1)
			{
				o_new_savepoint1[new_input_index] = i_savepoint1[cheak_point];
				o_new_savepoint2[new_input_index] = i_savepoint2[cheak_point];

				app_VD_output << o_new_savepoint1[i].x * (double)10.0 << ", " << o_new_savepoint1[i].y * (double)10.0 << " �� " << o_new_savepoint2[i].x * (double)10.0 << ", " << o_new_savepoint2[i].y * (double)10.0 << ", �� " << new_input_index << endl;
				new_input_index++;
			}
		}
	}
	o_new_input_index = new_input_index;
}

void CPathplanningDlg::Match_point(int i_line_count, int i_new_input_index, CvPoint2D64f(&io_new_savepoint1)[3000], CvPoint2D64f(&io_new_savepoint2)[3000], float near_dis)
{
	int small_loop1, small_loop2, small_index = 0;
	double small_dis1, small_dis2, small_dis3;
	CvPoint2D64f temp_point;
	vector <CvPoint2D64f> center_point, center_point2;

	for (small_loop1 = 0; small_loop1 < i_line_count; small_loop1++)
	{
		for (small_loop2 = 0; small_loop2 < i_line_count; small_loop2++)
		{
			if (io_new_savepoint1[small_loop1].x == io_new_savepoint2[small_loop2].x&&
				io_new_savepoint1[small_loop1].y == io_new_savepoint2[small_loop2].y)
				continue;


			small_dis1 = sqrt(pow(io_new_savepoint1[small_loop1].x - io_new_savepoint2[small_loop2].x, 2) + pow(io_new_savepoint1[small_loop1].y - io_new_savepoint2[small_loop2].y, 2));
			if (small_dis1 < near_dis)
			{
				temp_point.x = (io_new_savepoint1[small_loop1].x + io_new_savepoint2[small_loop2].x) / 2;
				temp_point.y = (io_new_savepoint1[small_loop1].y + io_new_savepoint2[small_loop2].y) / 2;
				//io_new_savepoint2[small_loop2] = io_new_savepoint1[small_loop1];
				center_point.push_back(cvPoint2D64f(temp_point.x, temp_point.y));
			}
		}
	}


	for (small_loop1 = 0; small_loop1 < i_line_count; small_loop1++)
	{
		for (small_loop2 = 0; small_loop2 < i_line_count; small_loop2++)
		{
			small_dis1 = sqrt(pow(io_new_savepoint1[small_loop1].x - io_new_savepoint1[small_loop2].x, 2) + pow(io_new_savepoint1[small_loop1].y - io_new_savepoint1[small_loop2].y, 2));
			small_dis2 = sqrt(pow(io_new_savepoint2[small_loop1].x - io_new_savepoint2[small_loop2].x, 2) + pow(io_new_savepoint2[small_loop1].y - io_new_savepoint2[small_loop2].y, 2));
			if (small_dis1 < near_dis)
			{
				temp_point.x = (io_new_savepoint1[small_loop1].x + io_new_savepoint1[small_loop2].x) / 2;
				temp_point.y = (io_new_savepoint1[small_loop1].y + io_new_savepoint1[small_loop2].y) / 2;
			}
			if (small_dis2 < near_dis)
			{
				temp_point.x = (io_new_savepoint2[small_loop1].x + io_new_savepoint2[small_loop2].x) / 2;
				temp_point.y = (io_new_savepoint2[small_loop1].y + io_new_savepoint2[small_loop2].y) / 2;
			}
		}
	}

	center_point2.push_back(center_point[0]);

	for (int i = 1; i < center_point.size(); i++)
	{

		for (int j = 0; j < center_point2.size(); j++)
		{
			small_dis3 = sqrt(pow(center_point[i].x - center_point2[j].x, 2) + pow(center_point[i].y - center_point2[j].y, 2));

			if (i == j)
				continue;

			if (small_dis3 < near_dis)
			{
				//				center_point2.push_back(center_point[i]);
				small_index = 0;
				break;
			}
			small_index = 1;
		}

		if (small_index == 1)
		{
			small_index = 0;
			center_point2.push_back(center_point[i]);
		}

	}

	for (int i = 0; i < i_new_input_index; i++)
	{

		for (int j = 0; j < center_point2.size(); j++)
		{
			small_dis1 = sqrt(pow(io_new_savepoint1[i].x - center_point2[j].x, 2) + pow(io_new_savepoint1[i].y - center_point2[j].y, 2));
			small_dis2 = sqrt(pow(io_new_savepoint2[i].x - center_point2[j].x, 2) + pow(io_new_savepoint2[i].y - center_point2[j].y, 2));

			if (small_dis1 < near_dis)
			{
				io_new_savepoint1[i] = center_point2[j];
			}

			if (small_dis2 < near_dis)
			{
				io_new_savepoint2[i] = center_point2[j];
			}
		}
	}
}

void CPathplanningDlg::Dijkstra_path_planning(CvPoint i_robot_start, CvPoint  i_robot_end, CvPoint2D64f i_new_savepoint1[3000], CvPoint2D64f i_new_savepoint2[3000], int i_new_input_index, vector <CPoint> &o_all_point_map, vector <CvPoint2D64f> &o_all_point_map_original)
{
	remove("�Ҧ��y�нs��.txt");
	fstream app_path_num("�Ҧ��y�нs��.txt", ios::app);

	vector <CPoint> CPoint_savepoint1;
	vector <CPoint> CPoint_savepoint2;

	for (int i = 0; i < i_new_input_index; i++)
	{
		CPoint_savepoint1.push_back(CPoint(round(i_new_savepoint1[i].x * 10), round(i_new_savepoint1[i].y * 10)));
		CPoint_savepoint2.push_back(CPoint(round(i_new_savepoint2[i].x * 10), round(i_new_savepoint2[i].y * 10)));
	}

	int loop1, loop2;
	bool onestime;
	int put_index = 0;

	//����J�Ĥ@�աA�H�K�ˬd��vector�ɨS�F��
	o_all_point_map.push_back(CPoint_savepoint1[0]);
	o_all_point_map_original.push_back(i_new_savepoint1[0]);
	app_path_num << o_all_point_map[put_index].x << ", " << o_all_point_map[put_index].y << " �� " << put_index << endl;
	put_index++;

	for (int i = 0; i < i_new_input_index; i++)
	{
		onestime = 1;
		for (int j = 0; j < o_all_point_map.size(); j++)
		{
			if (o_all_point_map[j] == CPoint_savepoint1[i])
				onestime = 0;
		}
		if (onestime == 1)
		{
			o_all_point_map.push_back(CPoint_savepoint1[i]);
			o_all_point_map_original.push_back(i_new_savepoint1[i]); // ��l���]�ƦC�@��
			app_path_num << o_all_point_map[put_index].x << ", " << o_all_point_map[put_index].y << " �� " << put_index << endl;
			put_index++;
		}
	}

	for (int i = 0; i < i_new_input_index; i++)
	{
		onestime = 1;
		for (int j = 0; j < o_all_point_map.size(); j++)
		{
			if (o_all_point_map[j] == CPoint_savepoint2[i])
				onestime = 0;
		}
		if (onestime == 1)
		{
			o_all_point_map.push_back(CPoint_savepoint2[i]);
			o_all_point_map_original.push_back(i_new_savepoint2[i]); // ��l���]�ƦC�@��
			app_path_num << o_all_point_map[put_index].x << ", " << o_all_point_map[put_index].y << " �� " << put_index << endl;
			put_index++;
		}
	}

	int savetemp_index1, savetemp_index2;

	for (loop1 = 0; loop1 < i_new_input_index; loop1++)
	{
		for (loop2 = 0; loop2 < put_index; loop2++)
		{

			if (CPoint_savepoint1[loop1] == o_all_point_map[loop2])
				savetemp_index1 = loop2;

			if (CPoint_savepoint2[loop1] == o_all_point_map[loop2])
				savetemp_index2 = loop2;

			if (w[loop1][loop2] == 0)
				w[loop1][loop2] = 50000;

		}

		w[savetemp_index1][savetemp_index2] = sqrt(pow(CPoint_savepoint1[loop1].x - CPoint_savepoint2[loop1].x, 2) + pow(CPoint_savepoint1[loop1].y - CPoint_savepoint2[loop1].y, 2));
		w[savetemp_index2][savetemp_index1] = w[savetemp_index1][savetemp_index2];
	}

	fstream app_jumppath_output("���|��X.txt", ios::app);

	int the_point_index[2];
	int serch_point[4];
	serch_point[2] = 100000;
	serch_point[3] = 100000;
	for (int serch = 0; serch < put_index; serch++)
	{

		serch_point[0] = sqrt(pow((o_all_point_map[serch].x - i_robot_start.x), 2) + pow((o_all_point_map[serch].y - i_robot_start.y), 2));
		serch_point[1] = sqrt(pow((o_all_point_map[serch].x - i_robot_end.x), 2) + pow((o_all_point_map[serch].y - i_robot_end.y), 2));


		if (serch_point[2] > serch_point[0])
		{
			serch_point[2] = serch_point[0];
			the_point_index[0] = serch;
		}
		if (serch_point[3] > serch_point[1])
		{
			serch_point[3] = serch_point[1];
			the_point_index[1] = serch;
		}
	}

	dijkstra(the_point_index[0], o_all_point_map.size());
	find_path(the_point_index[1]);

}

void CPathplanningDlg::Path_Optimization(vector<vector<bool>> i_sca_image, vector<CvPoint2D64f> i_all_point_map_original, vector<int>& o_path_optimization)
{

	int path_inside, break_index, line_distant;
	int cheak_x, cheak_y, cheak_index = 0;
	CvPoint2D64f cutout_point[2000];

	o_path_optimization.push_back(show_path[0]);

	for (int i = 0; i < show_path.size() - 1; i++)
	{
		for (int j = i + 1; j < show_path.size() - 1; j++)
		{

			line_distant = sqrt(pow(i_all_point_map_original[show_path[i]].x - i_all_point_map_original[show_path[j]].x, 2) + pow(i_all_point_map_original[show_path[i]].y - i_all_point_map_original[show_path[j]].y, 2));

			int cutout = 0;
			for (cutout = 0; cutout < line_distant; cutout++)
			{
				//�p��׽u�W���I
				if (i_all_point_map_original[show_path[i]].x >= i_all_point_map_original[show_path[j]].x && i_all_point_map_original[show_path[i]].y >= i_all_point_map_original[show_path[j]].y)
				{
					cutout_point[cutout].x = i_all_point_map_original[show_path[j]].x + (sqrt(pow((i_all_point_map_original[show_path[i]].x - i_all_point_map_original[show_path[j]].x), 2)) / (double)line_distant)*(double)cutout;
					cutout_point[cutout].y = i_all_point_map_original[show_path[j]].y + (sqrt(pow((i_all_point_map_original[show_path[i]].y - i_all_point_map_original[show_path[j]].y), 2)) / (double)line_distant)*(double)cutout;
				}

				if (i_all_point_map_original[show_path[i]].x >= i_all_point_map_original[show_path[j]].x && i_all_point_map_original[show_path[j]].y >= i_all_point_map_original[show_path[i]].y)
				{
					cutout_point[cutout].x = i_all_point_map_original[show_path[j]].x + (sqrt(pow((i_all_point_map_original[show_path[i]].x - i_all_point_map_original[show_path[j]].x), 2)) / (double)line_distant)*(double)cutout;
					cutout_point[cutout].y = i_all_point_map_original[show_path[j]].y - (sqrt(pow((i_all_point_map_original[show_path[i]].y - i_all_point_map_original[show_path[j]].y), 2)) / (double)line_distant)*(double)cutout;
				}

				if (i_all_point_map_original[show_path[j]].x >= i_all_point_map_original[show_path[i]].x && i_all_point_map_original[show_path[i]].y >= i_all_point_map_original[show_path[j]].y)
				{
					cutout_point[cutout].x = i_all_point_map_original[show_path[i]].x + (sqrt(pow((i_all_point_map_original[show_path[i]].x - i_all_point_map_original[show_path[j]].x), 2)) / (double)line_distant)*(double)cutout;
					cutout_point[cutout].y = i_all_point_map_original[show_path[i]].y - (sqrt(pow((i_all_point_map_original[show_path[i]].y - i_all_point_map_original[show_path[j]].y), 2)) / (double)line_distant)*(double)cutout;
				}

				if (i_all_point_map_original[show_path[j]].x >= i_all_point_map_original[show_path[i]].x && i_all_point_map_original[show_path[j]].y >= i_all_point_map_original[show_path[i]].y)
				{
					cutout_point[cutout].x = i_all_point_map_original[show_path[i]].x + (sqrt(pow((i_all_point_map_original[show_path[i]].x - i_all_point_map_original[show_path[j]].x), 2)) / (double)line_distant)*(double)cutout;
					cutout_point[cutout].y = i_all_point_map_original[show_path[i]].y + (sqrt(pow((i_all_point_map_original[show_path[i]].y - i_all_point_map_original[show_path[j]].y), 2)) / (double)line_distant)*(double)cutout;
				}
			}

			for (int cheak_pixel = 0; cheak_pixel < cutout; cheak_pixel++)
			{
				cheak_x = round(cutout_point[cheak_pixel].x);
				cheak_y = round(cutout_point[cheak_pixel].y);

				if ((cheak_y > i_sca_image.size() - 2) || (cheak_x > i_sca_image.size() - 2) || (cheak_y < 1 || (cheak_x < 1)))
					continue;

				if (i_sca_image[cheak_y][cheak_x] == 1 ||
					i_sca_image[cheak_y + 1][cheak_x] == 1 ||
					i_sca_image[cheak_y - 1][cheak_x] == 1 ||
					i_sca_image[cheak_y][cheak_x + 1] == 1 ||
					i_sca_image[cheak_y][cheak_x - 1] == 1 ||
					i_sca_image[cheak_y - 1][cheak_x - 1] == 1 ||
					i_sca_image[cheak_y + 1][cheak_x + 1] == 1 ||
					i_sca_image[cheak_y - 1][cheak_x + 1] == 1 ||
					i_sca_image[cheak_y + 1][cheak_x - 1] == 1/**/
					)
				{
					cheak_index = j - 1;  // �����I�����W�@�I
					break;
				}

			}

			if (cheak_index)
			{
				o_path_optimization.push_back(show_path[cheak_index]);
				break;
			}

		}
		if (cheak_index == 0)
		{
			i = show_path.size();
			o_path_optimization.push_back(show_path[show_path.size() - 1]);
		}
		else
		{
			i = cheak_index;
			cheak_index = 0;
		}

	}
}

void CPathplanningDlg::find_path(int x)   // �L�X�Ѱ_�I��x�I���̵u���|
{
	fstream app_truepath_output("���|��X.txt", ios::app);

	if (x != parent[x]) // ���⤧�e�����|���L�X��
		find_path(parent[x]);

	app_truepath_output << "x = " << x << " parent[x] = " << parent[x] << endl;
	app_truepath_output.close();
	show_path.push_back(x);
}

void CPathplanningDlg::dijkstra(int source, int node_num)
{
	for (int i = 0; i < node_num; i++) visit[i] = false;   // initialize
	for (int i = 0; i < node_num; i++) d[i] = 100000000;

	d[source] = 0;   //�Od[a]�O�_�I��a�I���̵u���|���סA�_�I�]���s�A��L�I���O�Ū�
	parent[source] = source;  // �����U���I�b�̵u���|��W�����ˬO��

	for (int k = 0; k < node_num; k++)
	{
		int a = -1, b = -1, min = 100000000;
		for (int i = 0; i < node_num; i++)
			if (!visit[i] && d[i] < min)
			{
				a = i;  // �O���o�@����
				min = d[i];
			}

		if (a == -1) break;     // �_�I���s�q���̵u���|���w�䧹
		if (min == 1e9) break;  // ���s�q�Y�O�̵u���|���׵L����
		visit[a] = true;

		// �H��ab�i��relaxation
		for (b = 0; b < node_num; b++)
			if (!visit[b] && d[a] + w[a][b] < d[b])
			{
				d[b] = d[a] + w[a][b];
				parent[b] = a;
			}
	}
}
