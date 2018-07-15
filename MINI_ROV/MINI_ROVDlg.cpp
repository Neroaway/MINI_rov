
// MINI_ROVDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MINI_ROV.h"
#include "MINI_ROVDlg.h"
#include "afxdialogex.h"
#include"CvvImage.h"
#include<opencv2\opencv.hpp>


/***************C������C++����֮��.h�ĵ���*************/
extern"C"
{
#include "hidsdi.h"
#include "setupapi.h"
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI RecvKeypress(LPVOID lpParameter);	//����һ���Ӵ���������������������յ�����Ϸ����ָ��
int nJoyStickNO = -1;								//����ȫ�ֱ�������Ųٿ��ֱ����豸���


CvCapture* capture;		//����CvCapture�ṹ���ָ�������������ͼ�񲶻����Ϣ
CRect rect;				//����һ�����������
CDC *pDC;				//����һ����ʾͼ��ָ�����
HDC hDC;				//ͼ���豸ָ��
CWnd *pwnd;				//window������ʾ


BOOL  b_exit_System = FALSE;					//�˳�ϵͳ��־
CCommend_Control m_Commend_Control;				//�����򴮿ڷ���������Ķ���/
//int nLScrollBar_Value=8;						//�������󻬿��ֵ����
//int nRScrollBar_Value=8;						//�������һ����ֵ����

int g_nOperType_Choice=1;						//��������ѡ��Ĭ��Ϊ��Ϸ���ֱ�����
int g_nUI_Operate_Value=7;						//��ò��������˶����Ϳ��Ƶ�ȫ�ֱ�����1-ǰ����2-���ˣ�3-��ת��4-��ת��5-�ϸ���6-��Ǳ����7-ץȡ��8-�ͷţ�9-ֹͣ��

/**********************��������******************************/
/**************ÿ�������PWM���Ǵ�PWM_LOW_VALUE-PWM_MOST_VALUE,PWM����PWM_STOPʱΪֹͣ��*************************/
/**************�涨PWM_STOP-PWM_MOST_VALUEΪ�����ת���ٶ�������**************************************/
/**************�涨PWM_STOP-PWM_LOW_VALUEΪ�����ת���ٶ�������**************************************/
/**************�涨�����ֵԽ�󣬽Ƕ�Խ�󣬷�����֮*****************************************/

int g_nSlider_Value=PWM_STOP;						//Slider�������ĳ�ʼֵ
int g_nLeft_Motor_Value=PWM_STOP;					//�����ٶȳ�ʼֵ
int g_nRight_Motor_Value=PWM_STOP;					//�ҵ���ٶȳ�ʼֵ
int g_nVertical_Motor_Value=PWM_STOP;				//���Ƶ���ٶȳ�ʼֵ
int g_nSteering_Engine_Value=PWM_STOP;				//����ٶȳ�ʼֵ

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

JoystickAPP Joystickapp;						//������Ϸ���ֱ�����
int nJoyStick_Xpos_R = ROCKER_MIDDLE;//������ҡ��X��
int nJoyStick_Ypos_R = ROCKER_MIDDLE;//������ҡ��Y��
int nJoyStick_Xpos_L = ROCKER_MIDDLE;//������ҡ��X��
int nJoyStick_Ypos_L = ROCKER_MIDDLE;//������ҡ��Y��

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CMINI_ROVDlg �Ի���




CMINI_ROVDlg::CMINI_ROVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMINI_ROVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CMINI_ROVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFOR_DISP, m_listInfor_Disp);
	DDX_Control(pDX, IDC_COMBOX_DEVCHOICE, m_ComBoxDevChoice);
	DDX_Control(pDX, IDC_SCROLLBAR_L_Y, m_ScrollBar_L_Y);
	DDX_Control(pDX, IDC_SCROLLBAR_R_Y, m_ScrollBar_R_Y);
	DDX_Control(pDX, IDC_SCROLLBAR_L_X, m_ScrollBar_L_X);
	DDX_Control(pDX, IDC_SCROLLBAR_R_X, m_ScrollBar_R_X);
	DDX_Control(pDX, IDC_COMBO_OPERTYPE, m_ComBoxOperType);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_Slider_Speed);
	DDX_Control(pDX, IDC_BUTTON3, Button_loose);
	DDX_Control(pDX, IDC_BUTTON2, butten_catch);
}

BEGIN_MESSAGE_MAP(CMINI_ROVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH_DEV, &CMINI_ROVDlg::OnBnClickedBtnSearchDev)
	ON_BN_CLICKED(IDCANCEL, &CMINI_ROVDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMINI_ROVDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_EXIT_SYSTEM, &CMINI_ROVDlg::OnBnClickedBtnExitSystem)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CMINI_ROVDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_CONNCTDEV, &CMINI_ROVDlg::OnBnClickedBtnConnctdev)
	ON_BN_CLICKED(IDC_BTN_OPEN_VIDEO, &CMINI_ROVDlg::OnBnClickedBtnOpenVideo)
	ON_BN_CLICKED(IDC_BTN_CLOSE_VIDEO, &CMINI_ROVDlg::OnBnClickedBtnCloseVideo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_FIBRECOM_INIT, &CMINI_ROVDlg::OnBnClickedBtnFibrecomInit)
	ON_WM_VSCROLL()

	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_COMEUP, &CMINI_ROVDlg::OnBnClickedBtnComeup)
	ON_BN_CLICKED(IDC_BTNDIVE, &CMINI_ROVDlg::OnBnClickedBtndive)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMINI_ROVDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTNRORWARD, &CMINI_ROVDlg::OnBnClickedBtnrorward)
	ON_BN_CLICKED(IDC_BTNBACK, &CMINI_ROVDlg::OnBnClickedBtnback)
	ON_BN_CLICKED(IDC_BTNTURNLEFT, &CMINI_ROVDlg::OnBnClickedBtnturnleft)
	ON_BN_CLICKED(IDC_BTNRIGHT, &CMINI_ROVDlg::OnBnClickedBtnright)
	ON_CBN_SELENDOK(IDC_COMBO_OPERTYPE, &CMINI_ROVDlg::OnCbnSelendokComboOpertype)
  //	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPEED, &CMINI_ROVDlg::OnCustomdrawSliderSpeed)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTNCAPTURE, &CMINI_ROVDlg::OnBnClickedBtncapture)
	ON_BN_CLICKED(IDC_BTNREALESE, &CMINI_ROVDlg::OnBnClickedBtnrealese)
	ON_BN_CLICKED(IDC_BTN_R2, &CMINI_ROVDlg::OnBnClickedBtnR2)
	ON_BN_CLICKED(IDC_BTN_L1, &CMINI_ROVDlg::OnBnClickedBtnL1)
	//�Ĵ��룬�°����
	ON_BN_CLICKED(IDC_BUTTON4, &CMINI_ROVDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CMINI_ROVDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMINI_ROVDlg ��Ϣ�������

BOOL CMINI_ROVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������  
	pwnd = GetDlgItem(IDC_STATIC_VIDEO_DISP);   
	pDC =pwnd->GetDC();   
	hDC= pDC->GetSafeHdc();  
	pwnd->GetClientRect(&rect);

 
	/***********************��ʼ��������*****************/
//	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_SCROLLBAR_L_Y);
	m_ScrollBar_L_Y.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_L_Y.SetScrollPos(PWM_STOP);			// ������������ĳ�ʼλ��Ϊ16����Ϊ�ײ� 

	m_ScrollBar_L_X.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_L_X.SetScrollPos(PWM_STOP);			// ������������ĳ�ʼλ��Ϊ16����Ϊ�ײ� 

	m_ScrollBar_R_Y.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_R_Y.SetScrollPos(PWM_STOP);			// ������������ĳ�ʼλ��Ϊ16����Ϊ�ײ� 

	m_ScrollBar_R_X.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_R_X.SetScrollPos(PWM_STOP);			// ������������ĳ�ʼλ��Ϊ16����Ϊ�ײ� 

	//m_Slider_Speed.SetRange(PWM_LOW_VALUE,PWM_MOST_VALUE);			//��ʼ������PWM_STOPΪֹͣ
	//m_Slider_Speed.SetTicFreq(PWM_STOP);

	m_ComBoxOperType.InsertString(-1,"ͼ�ν������ģʽ");
	m_ComBoxOperType.InsertString(-1,"�ֱ����Ʋ���ģʽ");	//����豸��ѡ���
	m_ComBoxOperType.SetCurSel(1);						//��ʼ������ģʽβ��Ϸ���ֱ�����
	Func_Com_Init();									//��ʼ����λ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMINI_ROVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMINI_ROVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMINI_ROVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMINI_ROVDlg::OnBnClickedBtnSearchDev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	m_pathArray.RemoveAll();										//ɾ�����·�������е�����
	m_ComBoxDevChoice.ResetContent();								//���ѡ����е�����
	m_listInfor_Disp.InsertString(-1, "���ڲ����豸�����Ժ� ...");	//��listbox�����һ��д������

	GUID HidGuid;													//����һ��GUID�Ľṹ�屣��HID�豸�Ľӿ���GUID
	HidD_GetHidGuid(&HidGuid);										//����HidD_GetHidGuid������ȡHID�豸��GUID

	HDEVINFO hDevInfoSet;											//���巵���豸��Ϣ���Ͼ��
	hDevInfoSet = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE); //ʹ����Ϻ�Ҫ��SetupDiDestroyDeviceInfoList����
	if (INVALID_HANDLE_VALUE == hDevInfoSet)
	{
		MessageBox("�豸��Ϣ���Ͼ������ֵ����", "����", MB_OK|MB_ICONWARNING);
		return;
	}
	/***********************���USB�豸�������Ϣ******************************/
	DWORD USBDevNO = 0;							//��USB�豸���б�ţ���ʼ�����Ϊ0
	SP_DEVICE_INTERFACE_DATA DevInterfaceData;	//�����豸�������ӿ���Ϣ
	DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);	
	BOOL bSuccess = TRUE;

	while(bSuccess)
	{
		bSuccess = SetupDiEnumDeviceInterfaces(hDevInfoSet, NULL, &HidGuid, USBDevNO, &DevInterfaceData);
		if(!bSuccess)
		{
			m_listInfor_Disp.InsertString(-1, " ");			//��ListBox�в�������
			m_listInfor_Disp.InsertString(-1, "USB�豸�Ѿ��������,�����������ز���...");			//��ListBox�в�������
			break;
		}

		CString strTemp;//��ʱCString����
		strTemp.Format("USB�豸---%d:", USBDevNO+1);
		m_listInfor_Disp.InsertString(-1, " ");				//��ListBox�в������
		m_listInfor_Disp.InsertString(-1, strTemp);			//��ListBox�в�������

		DWORD RequiredSize = 0;								//���USB�豸��ϸ��Ϣ����
		SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, NULL, 0, &RequiredSize, NULL);	//��ȡ��Ϣ�ɹ��������ȡ�豸��ϸ��Ϣ
		PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetailData;												//����һ��ָ���豸��ϸ��Ϣ�Ľṹ��ָ��
		pDevDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(RequiredSize);						//�����СΪRequiredSize���������������豸��ϸ��Ϣ
		if (NULL == pDevDetailData)
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);			//���øú��������豸�ӿڵ���ϸ��Ϣ
			m_listInfor_Disp.InsertString(-1, "malloc error");
			USBDevNO++;											//�豸��������
			continue;
		}
		pDevDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, pDevDetailData, RequiredSize, NULL, NULL)) //�ٴε��û�ȡ�豸��ϸ��Ϣ
		{		
			SetupDiDestroyDeviceInfoList(hDevInfoSet);	//�����豸��Ϣ���ϣ������ͷ����й������ڴ�
			free(pDevDetailData);						//�ͷŸղ����ٵ��ڴ�
			m_listInfor_Disp.InsertString(-1, "SetupDiGetDeviceInterfaceDetail error");
			USBDevNO++;
			continue;
		}
		CString strDevicePath;						//�������豸��·��
		strDevicePath= pDevDetailData->DevicePath;	//���豸·�����Ƴ���������������ڴ�
		m_pathArray.Add(strDevicePath);				//�Ѽ�⵽���豸·����ŵ�CStringArray
		free(pDevDetailData);						//�ͷ��ڴ�

		//ʹ��CreateFile������ȡ�豸�����ԣ�����VID��PID���汾�ŵ�
		HANDLE hDevHandle = CreateFile(strDevicePath, NULL, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hDevHandle)
		{		
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			m_listInfor_Disp.InsertString(-1, "CreateFile error");
			USBDevNO++;
			continue;
		}
		else
		{
			HIDD_ATTRIBUTES DevAttributes; //����HIDD_ATTRIBUTES�ṹ�屣���豸����
			DevAttributes.Size = sizeof(HIDD_ATTRIBUTES);
			if (!HidD_GetAttributes(hDevHandle, &DevAttributes)) //��ȡ�豸����
			{
				CloseHandle(hDevHandle); //�رոոմ򿪵��豸
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				m_listInfor_Disp.InsertString(-1, "HidD_GetAttributes error");
			}
			strTemp.Format("VID: %04X    PID: %04X    VNum: %04X", DevAttributes.VendorID, DevAttributes.ProductID, DevAttributes.VersionNumber);
			m_listInfor_Disp.InsertString(-1, strTemp);
			WCHAR mString[256]; 
			char Buffer[256]; 
			HidD_GetProductString(hDevHandle, mString, sizeof(mString)); //��ȡ��Ʒ����
			if (wcstombs(Buffer, mString, 256) != -1)
			{
				m_listInfor_Disp.InsertString(-1, "��Ʒ����: "+CString(Buffer));
			}
			else
			{
				m_listInfor_Disp.InsertString(-1, "HidD_GetProductString error");
			}	
			CloseHandle(hDevHandle);

			if((CString(Buffer) == "NTROLLER") || (CString(Buffer) == "LLEOLLER"))	
			{
				nJoyStickNO = USBDevNO;
				strTemp="�ٿ��ֱ�";
			}
			else
			{
				strTemp.Format("USB�豸 %d", USBDevNO+1);
			}
			m_ComBoxDevChoice.InsertString(-1, strTemp); //����豸��ѡ���
			USBDevNO++;
		}
	}
	m_listInfor_Disp.SetTopIndex(m_listInfor_Disp.GetCount()-1);
	SetupDiDestroyDeviceInfoList(hDevInfoSet);
}



void CMINI_ROVDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CDialogEx::OnCancel();
	Func_Exit_System();
}

void CMINI_ROVDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CMINI_ROVDlg::Func_Exit_System(void)//�˳�ϵͳ����
{
	if(MessageBox("ȷ��Ҫ�˳�ϵͳ��","��ʾ",MB_OKCANCEL)==IDOK)
	{
		cvReleaseCapture(&capture);
		cvReleaseCapture(&capture);
		b_exit_System=TRUE;
		OnOK();
	}	
}

void CMINI_ROVDlg::OnBnClickedBtnExitSystem()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Func_Exit_System();
}


void CMINI_ROVDlg::OnBnClickedBtnClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_listInfor_Disp.ResetContent();
}


void CMINI_ROVDlg::OnBnClickedBtnConnctdev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTitle;
	GetDlgItemText(IDC_BTN_CONNCTDEV, strTitle);
	
	int nCurSel=-1;
	if (strTitle == "�����豸")
	{
		nCurSel = m_ComBoxDevChoice.GetCurSel();//���ѡ�е�ѡ������
		if (nCurSel == CB_ERR)//ѡ�����Ϊ�գ�����ʾ����
		{
			MessageBox("��ѡ��Ҫ���ӵ��豸", "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}

		if( joyGetNumDevs() == 0 )
		{
			GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("������Ч��USB�豸");
			return;
		}
		else
		{
			memset(&joyinfo,0,sizeof(JOYINFOEX));
			joyinfo.dwSize = sizeof(JOYINFOEX);
			joyinfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;
			if( joyGetPosEx(JOYSTICKID1, &joyinfo) != JOYERR_UNPLUGGED )
			{
				Joystickapp.joystickid = JOYSTICKID1;
			}				
			else if( joyGetPosEx(JOYSTICKID2, &joyinfo) != JOYERR_UNPLUGGED )
			{
				Joystickapp.joystickid = JOYSTICKID2;
			}
				
			SetDlgItemText(IDC_BTN_CONNCTDEV, "�Ͽ�����");
			if (nJoyStickNO == nCurSel )
			{
				SetTimer(TIMER_JOYSTAICK,100,NULL); //100ms��ȡһ��
				GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("�ٿ��ֱ������ӳɹ�");
			}
			else
			{
				GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("USB�豸�����ӳɹ�");
			}
		}
	}
	else
	{	
		KillTimer(TIMER_JOYSTAICK);
		SetDlgItemText(IDC_BTN_CONNCTDEV, "�����豸");
		GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("�豸�ѶϿ�����");
	}
}

void CMINI_ROVDlg::OnBnClickedBtnOpenVideo()
{
	// TDO:�ڴ���ӿؼ�֪ͨ����������
	if(!capture)  //�鿴�Ƿ�������ͷ�豸����
	{  
		capture = cvCaptureFromCAM(VIDEO); //�������ͷ��Ƶ  
		if(!capture)
		{
			GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("δ�ҵ�����豸");
			return;
		}	
	} 
	GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("��Ƶ����ѿ���");
	SetTimer(TIMER_CAP,10,NULL); //10ms����һ��ͼ��
}


void CMINI_ROVDlg::OnBnClickedBtnCloseVideo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cvReleaseCapture(&capture);  
	GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("��Ƶ����ѹر�");
}

void CMINI_ROVDlg::OnBnClickedBtnrorward()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	UpdateData(TRUE);
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=1;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}

void CMINI_ROVDlg::OnBnClickedBtnback()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=2;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}

void CMINI_ROVDlg::OnBnClickedBtnturnleft() //�򵥵�����ת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=3;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}

void CMINI_ROVDlg::OnBnClickedBtnright()//�����Ҳת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=4;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}

void CMINI_ROVDlg::OnBnClickedBtnComeup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=5;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}


void CMINI_ROVDlg::OnBnClickedBtndive()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=6;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}


void CMINI_ROVDlg::OnBnClickedBtncapture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=7;
}


void CMINI_ROVDlg::OnBnClickedBtnrealese()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=8;
}
void CMINI_ROVDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(TIMER_UIOPERATE, 100, NULL);
	g_nUI_Operate_Value = 5;
}




void CMINI_ROVDlg::OnBnClickedButton4()
{// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(TIMER_UIOPERATE, 100, NULL);
	g_nUI_Operate_Value = 6;
}
void CMINI_ROVDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value=9;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}


void CMINI_ROVDlg::OnBnClickedBtnR2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value = 10;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}




void CMINI_ROVDlg::OnBnClickedBtnL1()
{
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//��ʼ������PWM_STOPΪֹͣ
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value = 11;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("ֹͣ");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("����");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("�ٶ�ֵ:0");
}


void CMINI_ROVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
	case TIMER_CAP:
		Timer_CapVideo();
		break;
	case TIMER_JOYSTAICK:
		Timer_JoyStick();
		break;
	case TIMER_UIOPERATE:
		Timer_UI();
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMINI_ROVDlg::Timer_CapVideo(void)
{
	IplImage* m_Frame = NULL;  
	m_Frame=cvQueryFrame(capture);  
	CvvImage m_CvvImage;  
	m_CvvImage.CopyOf(m_Frame,1);     
	if (true)  
	{  
		m_CvvImage.DrawToHDC(hDC, &rect); 
	}
}

/************���״ֵ̬����************/



void CMINI_ROVDlg::Timer_JoyStick(void)
{

	int nscrollpos_R_X=PWM_STOP;//��ҡ�˵�X�� ��������ת����ת��������λ���������ת��ת��������ʾ
	int nscrollpos_R_Y=PWM_STOP;//��ҡ�˵�Y�� ������ǰ�������ˣ�������λ�������ǰ�����˻�������ʾ

	int nscrollpos_L_X=PWM_STOP;//��ҡ�˵�X�� �����˵Ļ�е��ץȡ���ͷţ�������λ�������ץȡ�ͷŻ�������ʾ
	int nscrollpos_L_Y=PWM_STOP;//��ҡ�˵�Y�� �������ϸ���Ǳ��������λ��������ϸ���Ǳ��������ʾ

	int LX = 0, LY = 0, RX = 0, RY = 0, zypy = 0, zyxz = 0, qhpy = 0, sxfd = 0;
	int up_pwm = 0, l1_pwm = 0, l2_pwm = 0, r1_pwm = 0, r2_pwm = 0; //(-7200~7200)
	static unsigned int zhangjiao = 120; //(2000)
	joyGetPosEx(Joystickapp.joystickid, &joyinfo);

	/*****************��ֵ32511Ϊ�м�ֵ����ֹͣ����Ӧ�����PWMֵΪ1565���ң��ֱ���Сÿ�仯72����PWM��Ӧ�仯Ϊ1***********************/
	/**��Ϊ���PWM�ķ�ΧΪPWM_LOW_VALUE-PWM_MOST_VALUE����1565-PWM_LOW_VALUEʱΪ��ת����1565-PWM_MOST_VALUEʱΪ��ת�����ٶ�������**/
	/*****************��Ӧҡ����˵��ֵΪ32511-65535���֮����Ϊ��ת��ֵΪ32511-0֮��Ϊ��ת���ҵ���ٶ�������***********************/
	/**********************������㹫ʽΪ��1100+ҡ��ֵ/72=PWMֵ����ΪҪ�������ݵ�����************************************************/
	
	/******************���ڿ��ƶ����˵,�ֱ���ǰ����7��8���ֱ��������ƻ�е�ֵ�ץȡ���ͷ�***************************************/
	Joystickapp.wJoyXPos_R = WORD(joyinfo.dwZpos);//��ת����ת 
	Joystickapp.wJoyYPos_R = WORD(joyinfo.dwRpos);//ǰ��������

	Joystickapp.wJoyXPos_L = WORD(joyinfo.dwXpos);//Ԥ��
	Joystickapp.wJoyYPos_L = WORD(joyinfo.dwYpos);//�ϸ�����Ǳ



	Joystickapp.fJoyButton7Down = ((joyinfo.dwButtons & 64) != 0); //��е��ץȡ
	Joystickapp.fJoyButton8Down = ((joyinfo.dwButtons & 128) != 0);//��е���ͷ�
	Joystickapp.fJoyButton5Down = ((joyinfo.dwButtons & 64)!= 0);   //���2
	Joystickapp.fJoyButton6Down = ((joyinfo.dwButtons & 128) != 0);  //���2
	

	/**************************************************************************/


	/***********************ҡ������ֱ�ӷ��͸���λ������Χ1100~2010 ��ֵ1551**************************************/

	nscrollpos_R_X=Joystickapp.wJoyXPos_R/72+PWM_LOW_VALUE;			//�����ҡ�˵�X������
	nscrollpos_R_Y=Joystickapp.wJoyYPos_R/72+PWM_LOW_VALUE;			//�����ҡ�˵�Y������

	nscrollpos_L_X=Joystickapp.wJoyXPos_L/72+PWM_LOW_VALUE;			//�����ҡ�˵�X������
	nscrollpos_L_Y=Joystickapp.wJoyYPos_L/72+PWM_LOW_VALUE;			//�����ҡ�˵�Y������



	LX = nscrollpos_L_X - ROCKER_MIDDLE;
	LY = nscrollpos_L_Y - ROCKER_MIDDLE;
	RX = nscrollpos_R_X - ROCKER_MIDDLE;
	RY = nscrollpos_R_Y - ROCKER_MIDDLE;

	if (LX>10) //����  zypy
	{
		zyxz = LX - 10;
	}
	else if (LX<-10) //����
	{
		zyxz = LX + 10;
	}
	else //ֹͣ
	{
		zyxz = 0;
	}

	if (LY>10)//�½�   LY
	{

		sxfd = LY - 10;
	}
	else
	if (LY<-10)//���� 
	{
		sxfd = LY + 10;
	}
	else //ֹͣ
	{
		sxfd = 0;
	}

	if (RX>10)//��ת   zyxz
	{
		zypy = RX - 10;
	}
	else if (RX<-10) //��ת
	{
		zypy = RX + 10;
	}
	else //ֹͣ
	{
		zypy = 0;
	}

	if (RY>10) //����  qhpy
	{
		qhpy = RY - 10;
	}
	else if (RY<-10) //ǰ��
	{
		qhpy = RY + 10;
	}
	else //ֹͣ
	{
		qhpy = 0;
	}
	up_pwm = sxfd*MAGNIFICATION; l1_pwm = MAGNIFICATION*(-zypy - zyxz + qhpy); l2_pwm = MAGNIFICATION*(zypy - zyxz + qhpy); r1_pwm = MAGNIFICATION*(-zypy - zyxz - qhpy); r2_pwm = MAGNIFICATION*(-zypy + zyxz + qhpy);
	if (up_pwm>PWM_MAX) up_pwm = PWM_MAX;if (up_pwm<-PWM_MAX) up_pwm = -PWM_MAX;
	if (l1_pwm>PWM_MAX) l1_pwm = PWM_MAX; if (l1_pwm<-PWM_MAX) l1_pwm = -PWM_MAX;
	if (l2_pwm>PWM_MAX) l2_pwm = PWM_MAX; if (l2_pwm<-PWM_MAX) l2_pwm = -PWM_MAX;
	if (r1_pwm>PWM_MAX) r1_pwm = PWM_MAX; if (r1_pwm<-PWM_MAX) r1_pwm = -PWM_MAX;
	if (r2_pwm>PWM_MAX) r2_pwm = PWM_MAX; if (r2_pwm<-PWM_MAX) r2_pwm = -PWM_MAX;

	if(Joystickapp.fJoyButton7Down == TRUE)		//�ж�7���Ƿ���
	{
		if(Joystickapp.fJoyButton8Down != TRUE)	 //8��δ����
		{

			zhangjiao += 20;
			if (zhangjiao>250)
			{
				zhangjiao = 250;
			}
			Button_loose.SetState(false);
		}
		else
		{
			Button_loose.SetState(true);
		}
		butten_catch.SetState(true);
	}
	else 
	{
		if(Joystickapp.fJoyButton8Down == TRUE)		//8������
		{
			zhangjiao -= 20;
			if (zhangjiao<120)
			{
				zhangjiao = 120;
			}
			Button_loose.SetState(true);
		}
		else
		{
			Button_loose.SetState(false);
		}
		butten_catch.SetState(false);
	}

	if (Joystickapp.fJoyButton5Down == TRUE)		//�ж�5���Ƿ���
	{
		if (Joystickapp.fJoyButton6Down != TRUE)	 //6��δ����
		{

			zhangjiao += 20;
			if (zhangjiao>250)
			{
				zhangjiao = 250;
			}
			Button_loose.SetState(false);
		}
		else
		{
			Button_loose.SetState(true);
		}
		butten_catch.SetState(true);
	}
	else
	{
		if (Joystickapp.fJoyButton6Down == TRUE)		//6������
		{
			zhangjiao -= 20;
			if (zhangjiao<120)
			{
				zhangjiao = 120;
			}
			Button_loose.SetState(true);
		}
		else
		{
			Button_loose.SetState(false);
		}
		butten_catch.SetState(false);
	}



	Func_Send_Data( up_pwm,  l1_pwm,  l2_pwm,  r1_pwm,  r2_pwm,  zhangjiao);

	m_ScrollBar_L_Y.SetScrollPos(nscrollpos_L_Y);	// ���������������ʾ��ǰ��ҡ�˵�Yֵ  
	m_ScrollBar_L_X.SetScrollPos(nscrollpos_L_X);	// ����ߺỬ������ʾ��ǰ��ҡ�˵�Xֵ
	m_ScrollBar_R_Y.SetScrollPos(nscrollpos_R_Y);	// ���ұ�����������ʾ��ǰ��ҡ�˵�Yֵ   
	m_ScrollBar_R_X.SetScrollPos(nscrollpos_R_X);	// ���ұߺỬ������ʾ��ǰ��ҡ�˵�Xֵ 
	
}






//int nJoyStick_Xpos_R = ROCKER_MIDDLE;//������ҡ��X��
//int nJoyStick_Ypos_R = ROCKER_MIDDLE;//������ҡ��Y��
//int nJoyStick_Xpos_L = ROCKER_MIDDLE;//������ҡ��X��
//int nJoyStick_Ypos_L = ROCKER_MIDDLE;//������ҡ��Y��
//bool vertical = false;


void CMINI_ROVDlg::Timer_UI(void)//ֵ����1565������������С��1565������ת���������
{
	int LX = 0, LY = 0, RX = 0, RY = 0, zypy = 0, zyxz = 0, qhpy = 0, sxfd = 0;
	int up_pwm = 0, l1_pwm = 0, l2_pwm = 0, r1_pwm = 0, r2_pwm = 0; //(-7200~7200)
	static unsigned int zhangjiao = 120; //(2000)
	switch(g_nUI_Operate_Value)
	{
	case 1://ǰ�� ֵԽС�ٶ�Խ��
		{
			/************** ��Ϊ�����ֵ�Ǵ�PWM_STOP��ʼ�ģ������תʱ�������ֵ�����󣬶�Ӧ��PWMֵӦ�ü�С���������ת�ٶ�******/
			   nJoyStick_Ypos_R = ROCKER_MIDDLE-g_nSlider_Value;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			  

		}
		break;
	case 2://���� ֵԽ���ٶ�Խ��
		{
			
			   nJoyStick_Ypos_R = ROCKER_MIDDLE + g_nSlider_Value;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			  
		}
		break;
	case 3://���� �ҵ����ת���������򣬼򵥲���ת��
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE ;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE- g_nSlider_Value;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			 
		}
		break;
	case 4://���� �ҵ����ת��������ת���򵥲���ת��
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE + g_nSlider_Value;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			  
		}
		break;
	case 5://�ϸ�
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE ;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE- g_nSlider_Value;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			   
		}
		break;
	case 6://�³�
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE + g_nSlider_Value;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			   
		}
		break;
	case 7://ץ��
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE ;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			   zhangjiao += 20;
			   if (zhangjiao>250)
			   {
				   zhangjiao = 250;
			   }
		}
		break;
	case 8://�ɿ�
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
			   zhangjiao -= 20;
			   if (zhangjiao<120)
			   {
				   zhangjiao = 120;
			   }
		}
		break;
	case 9://ֹͣ
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//���ֹͣת��
		}
		break;
		case 10://��ת
		{
					nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
					nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
					nJoyStick_Ypos_L = ROCKER_MIDDLE;							//���Ƶ��ֹͣת��
					nJoyStick_Xpos_L = ROCKER_MIDDLE + g_nSlider_Value;							//���ֹͣת��
		}
		break;
		case 11://��ת
		{
					nJoyStick_Ypos_R = ROCKER_MIDDLE;						//�ҵ��PWMֵ
					nJoyStick_Xpos_R = ROCKER_MIDDLE;						//����PWMֵ
					nJoyStick_Ypos_L = ROCKER_MIDDLE ;							//���Ƶ��ֹͣת��
					nJoyStick_Xpos_L = ROCKER_MIDDLE- g_nSlider_Value;							//���ֹͣת��
		}
		break;
	default:
		break;
	}


	LX = nJoyStick_Xpos_L - ROCKER_MIDDLE;
	LY = nJoyStick_Ypos_L - ROCKER_MIDDLE;
	RX = nJoyStick_Xpos_R - ROCKER_MIDDLE;
	RY = nJoyStick_Ypos_R - ROCKER_MIDDLE;

	if (LX>10) //����  zypy
	{
		zypy = LX - 10;
	}
	else if (LX<-10) //����
	{
		zypy = LX + 10;
	}
	else //ֹͣ
	{
		zypy = 0;
	}

	if (LY>10)//�½�   LY
	{

		sxfd = LY - 10;
	}
	else
	if (LY<-10)//���� 
	{
		sxfd = LY + 10;
	}
	else //ֹͣ
	{
		sxfd = 0;
	}

	if (RX>10)//��ת   zyxz
	{
		zyxz = RX - 10;
	}
	else if (RX<-10) //��ת
	{
		zyxz = RX + 10;
	}
	else //ֹͣ
	{
		zyxz = 0;
	}

	if (RY>10) //����  qhpy
	{
		qhpy = RY - 10;
	}
	else if (RY<-50) //ǰ��
	{
		qhpy = RY + 10;
	}
	else //ֹͣ
	{
		qhpy = 0;
	}
	up_pwm = sxfd*MAGNIFICATION; l1_pwm = MAGNIFICATION*(-zypy - zyxz + qhpy); l2_pwm = MAGNIFICATION*(zypy - zyxz + qhpy); r1_pwm = MAGNIFICATION*(-zypy - zyxz - qhpy); r2_pwm = MAGNIFICATION*(-zypy + zyxz + qhpy);
	if (up_pwm>PWM_MAX) up_pwm = PWM_MAX; if (up_pwm<-PWM_MAX) up_pwm = -PWM_MAX;
	if (l1_pwm>PWM_MAX) l1_pwm = PWM_MAX; if (l1_pwm<-PWM_MAX) l1_pwm = -PWM_MAX;
	if (l2_pwm>PWM_MAX) l2_pwm = PWM_MAX; if (l2_pwm<-PWM_MAX) l2_pwm = -PWM_MAX;
	if (r1_pwm>PWM_MAX) r1_pwm = PWM_MAX; if (r1_pwm<-PWM_MAX) r1_pwm = -PWM_MAX;
	if (r2_pwm>PWM_MAX) r2_pwm = PWM_MAX; if (r2_pwm<-PWM_MAX) r2_pwm = -PWM_MAX;


	Func_Send_Data(up_pwm, l1_pwm, l2_pwm, r1_pwm, r2_pwm, zhangjiao);
}


/*********************************************ͨ��Э�麯��˵��**************************************************************/
/****************************************nscrollpos_L_Y��ҡ��y��ֵ��nscrollpos_L_X��ҡ��x��ֵ*******************************/
/****************************************nscrollpos_R_Y��ҡ��y��ֵ��nscrollpos_R_X��ҡ��x��ֵ*******************************/
/******************************************************END******************************************************************/
void CMINI_ROVDlg::Func_Send_Data(unsigned int up_pwm, unsigned int l1_pwm, unsigned int l2_pwm, unsigned int r1_pwm, unsigned int r2_pwm, unsigned char zhangjiao)
{
	char Data_Buf[16] = { 0 };				//�򴮿ڷ�����������

	BYTE up_pwm_most_bit;			
	BYTE up_pwm_low_bit;	

	BYTE l1_pwm_most_bit;
	BYTE l1_pwm_low_bit;
	BYTE l2_pwm_most_bit;
	BYTE l2_pwm_low_bit;
	BYTE r1_pwm_most_bit;
	BYTE r1_pwm_low_bit;
	BYTE r2_pwm_most_bit;
	BYTE r2_pwm_low_bit;


	up_pwm_most_bit = up_pwm / 256;
	up_pwm_low_bit = up_pwm % 256;

	l1_pwm_most_bit = l1_pwm / 256;
	l1_pwm_low_bit = l1_pwm % 256;

	l2_pwm_most_bit = l2_pwm / 256;
	l2_pwm_low_bit = l2_pwm % 256;

	r1_pwm_most_bit = r1_pwm / 256;
	r1_pwm_low_bit = r1_pwm % 256;

	r2_pwm_most_bit = r2_pwm / 256;
	r2_pwm_low_bit= r2_pwm % 256;

	Data_Buf[0] = up_pwm_most_bit;		//��ҡ��y���λ
	Data_Buf[1] = up_pwm_low_bit;		//��ҡ��y���λ
	Data_Buf[2] = up_pwm_most_bit - up_pwm_low_bit; //У��
	Data_Buf[3] = l1_pwm_most_bit;		//��ҡ��x���λ
	Data_Buf[4] = l1_pwm_low_bit;		//��ҡ��x���λ
	Data_Buf[5] = l1_pwm_most_bit - l1_pwm_low_bit; //У��
	Data_Buf[6] = l2_pwm_most_bit;	//��ҡ��y���λ
	Data_Buf[7] = l2_pwm_low_bit;	//��ҡ��y���λ
	Data_Buf[8] = l2_pwm_most_bit - l2_pwm_low_bit; //У��
	Data_Buf[9] = r1_pwm_most_bit;	//��ҡ��x���λ
	Data_Buf[10] = r1_pwm_low_bit;	//��ҡ��x���λ
	Data_Buf[11] = r1_pwm_most_bit - r1_pwm_low_bit; //У��
	Data_Buf[12] = r2_pwm_most_bit;	//��ҡ��x���λ
	Data_Buf[13] = r2_pwm_low_bit;	//��ҡ��x���λ
	Data_Buf[14] = r2_pwm_most_bit - r2_pwm_low_bit; //У��
	Data_Buf[15] = zhangjiao;

	m_Commend_Control.Func_CommendControl_Send(Data_Buf,16);//��ڷ�������
}

/*********************�ַ�������������ж��źŴ���崮�ڷ������ݵ���Ч��**********************/
unsigned char  cFibre_rxbuf[10];        //����յ�������
int            nFibre_rxnum=0;          //�յ����ݼ���
int  characterSigTraEof(unsigned char c)            
{			
	cFibre_rxbuf[nFibre_rxnum]=c;
	nFibre_rxnum++;
	BYTE bTempCmd1;										    //����1����ʱ����
	return 0;
}


BOOL CMINI_ROVDlg::Func_Com_Init(void)
{
	BOOL  bFibre_Com_InitFlag=FALSE;//��λ�����ڳ�ʼ����־
	if(!bFibre_Com_InitFlag)
	{
		bFibre_Com_InitFlag=m_Commend_Control.Func_CommendControl_Com_Init(this->m_hWnd);		//��λ�����ڳ�ʼ��
		if(!bFibre_Com_InitFlag)
		{
			GetDlgItem(IDC_STATIC_COMSTATUS)->SetWindowTextA("���ڳ�ʼ��ʧ��,�����豸......");
			GetDlgItem(IDC_BTN_FIBRECOM_INIT)->EnableWindow(TRUE);
			return FALSE;
		}
	}

	GetDlgItem(IDC_BTN_FIBRECOM_INIT)->EnableWindow(FALSE);

	GetDlgItem(IDC_STATIC_COMSTATUS)->SetWindowTextA("���ڳ�ʼ���ɹ�");
	return TRUE;
}


void CMINI_ROVDlg::OnBnClickedBtnFibrecomInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Func_Com_Init();
}

void CMINI_ROVDlg::OnCbnSelendokComboOpertype()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	g_nOperType_Choice=m_ComBoxOperType.GetCurSel();
	switch(g_nOperType_Choice)
	{		
	case 0:
		KillTimer(TIMER_JOYSTAICK);

		SetDlgItemText(IDC_BTN_CONNCTDEV, "�����豸");
		GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("�豸�ѶϿ�����");

		GetDlgItem(IDC_BTN_CONNCTDEV)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBOX_DEVCHOICE)->EnableWindow(FALSE);

		GetDlgItem(IDC_BTN_L1)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BTN_L2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_COMEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNDIVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNTEMP2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNTEMP3)->EnableWindow(TRUE);

//		GetDlgItem(IDC_BTN_R1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_R2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNRORWARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNBACK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNTURNLEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTNRIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
		break;
	case 1:
		KillTimer(TIMER_UIOPERATE);
		SetDlgItemText(IDC_BTN_CONNCTDEV, "�����豸");
		GetDlgItem(IDC_BTN_CONNCTDEV)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBOX_DEVCHOICE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BTN_L1)->EnableWindow(FALSE);
//		GetDlgItem(IDC_BTN_L2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_COMEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNDIVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNTEMP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNTEMP3)->EnableWindow(FALSE);

//		GetDlgItem(IDC_BTN_R1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_R2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNRORWARD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNBACK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNTURNLEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTNRIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
		break;
	default :
		break;
	}
}


void CMINI_ROVDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CSliderCtrl *pSlidCtrl=(CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED);
	g_nSlider_Value=pSlidCtrl->GetPos();//ȡ�õ�ǰλ��ֵ
	CString strTemp;
	strTemp.Format("�ٶ�ֵ:%d", g_nSlider_Value);
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA(strTemp);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}






	

