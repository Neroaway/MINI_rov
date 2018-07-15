
// MINI_ROVDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MINI_ROV.h"
#include "MINI_ROVDlg.h"
#include "afxdialogex.h"
#include"CvvImage.h"
#include<opencv2\opencv.hpp>


/***************C语言与C++语言之间.h的调用*************/
extern"C"
{
#include "hidsdi.h"
#include "setupapi.h"
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI RecvKeypress(LPVOID lpParameter);	//定义一个视窗操作函数，用来处理接收到的游戏机的指令
int nJoyStickNO = -1;								//定义全局变量，存放操控手柄的设备编号


CvCapture* capture;		//构建CvCapture结构体的指针对象，用来保存图像捕获的信息
CRect rect;				//构建一个矩形类对象
CDC *pDC;				//构建一个显示图形指针对象
HDC hDC;				//图像设备指针
CWnd *pwnd;				//window窗体显示


BOOL  b_exit_System = FALSE;					//退出系统标志
CCommend_Control m_Commend_Control;				//定义向串口发送命令类的对象/
//int nLScrollBar_Value=8;						//定义存放左滑块的值变量
//int nRScrollBar_Value=8;						//定义存放右滑块的值变量

int g_nOperType_Choice=1;						//操作类型选择默认为游戏机手柄操作
int g_nUI_Operate_Value=7;						//获得操作界面运动类型控制的全局变量，1-前进，2-后退，3-左转，4-右转，5-上浮，6-下潜，，7-抓取，8-释放，9-停止。

/**********************参数定义******************************/
/**************每个电机的PWM都是从PWM_LOW_VALUE-PWM_MOST_VALUE,PWM等于PWM_STOP时为停止，*************************/
/**************规定PWM_STOP-PWM_MOST_VALUE为电机正转且速度逐渐增大**************************************/
/**************规定PWM_STOP-PWM_LOW_VALUE为电机反转且速度逐渐增大**************************************/
/**************规定舵机的值越大，角度越大，否则则反之*****************************************/

int g_nSlider_Value=PWM_STOP;						//Slider滑动条的初始值
int g_nLeft_Motor_Value=PWM_STOP;					//左电机速度初始值
int g_nRight_Motor_Value=PWM_STOP;					//右电机速度初始值
int g_nVertical_Motor_Value=PWM_STOP;				//垂推电机速度初始值
int g_nSteering_Engine_Value=PWM_STOP;				//舵机速度初始值

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

JoystickAPP Joystickapp;						//定义游戏机手柄对象
int nJoyStick_Xpos_R = ROCKER_MIDDLE;//定义右摇杆X轴
int nJoyStick_Ypos_R = ROCKER_MIDDLE;//定义右摇杆Y轴
int nJoyStick_Xpos_L = ROCKER_MIDDLE;//定义左摇杆X轴
int nJoyStick_Ypos_L = ROCKER_MIDDLE;//定义左摇杆Y轴

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMINI_ROVDlg 对话框




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
	//改代码，新版代码
	ON_BN_CLICKED(IDC_BUTTON4, &CMINI_ROVDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CMINI_ROVDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMINI_ROVDlg 消息处理程序

BOOL CMINI_ROVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码  
	pwnd = GetDlgItem(IDC_STATIC_VIDEO_DISP);   
	pDC =pwnd->GetDC();   
	hDC= pDC->GetSafeHdc();  
	pwnd->GetClientRect(&rect);

 
	/***********************初始化滚动条*****************/
//	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_SCROLLBAR_L_Y);
	m_ScrollBar_L_Y.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_L_Y.SetScrollPos(PWM_STOP);			// 设置左滚动条的初始位置为16，即为底部 

	m_ScrollBar_L_X.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_L_X.SetScrollPos(PWM_STOP);			// 设置左滚动条的初始位置为16，即为底部 

	m_ScrollBar_R_Y.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_R_Y.SetScrollPos(PWM_STOP);			// 设置左滚动条的初始位置为16，即为底部 

	m_ScrollBar_R_X.SetScrollRange(ROCKER_MIN, ROCKER_MAX);
	m_ScrollBar_R_X.SetScrollPos(PWM_STOP);			// 设置左滚动条的初始位置为16，即为底部 

	//m_Slider_Speed.SetRange(PWM_LOW_VALUE,PWM_MOST_VALUE);			//初始化滑条PWM_STOP为停止
	//m_Slider_Speed.SetTicFreq(PWM_STOP);

	m_ComBoxOperType.InsertString(-1,"图形界面调试模式");
	m_ComBoxOperType.InsertString(-1,"手柄控制操作模式");	//添加设备到选择框
	m_ComBoxOperType.SetCurSel(1);						//初始化操作模式尾游戏机手柄操作
	Func_Com_Init();									//初始化下位机串口

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMINI_ROVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMINI_ROVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMINI_ROVDlg::OnBnClickedBtnSearchDev()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_pathArray.RemoveAll();										//删除存放路径数组中的内容
	m_ComBoxDevChoice.ResetContent();								//清空选项框中的内容
	m_listInfor_Disp.InsertString(-1, "正在查找设备，请稍候 ...");	//向listbox的最后一行写入数据

	GUID HidGuid;													//定义一个GUID的结构体保存HID设备的接口类GUID
	HidD_GetHidGuid(&HidGuid);										//调用HidD_GetHidGuid函数获取HID设备的GUID

	HDEVINFO hDevInfoSet;											//定义返回设备信息集合句柄
	hDevInfoSet = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE); //使用完毕后要用SetupDiDestroyDeviceInfoList销毁
	if (INVALID_HANDLE_VALUE == hDevInfoSet)
	{
		MessageBox("设备信息集合句柄返回值错误", "错误", MB_OK|MB_ICONWARNING);
		return;
	}
	/***********************获得USB设备的相关信息******************************/
	DWORD USBDevNO = 0;							//对USB设备进行编号，初始化编号为0
	SP_DEVICE_INTERFACE_DATA DevInterfaceData;	//保存设备的驱动接口信息
	DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);	
	BOOL bSuccess = TRUE;

	while(bSuccess)
	{
		bSuccess = SetupDiEnumDeviceInterfaces(hDevInfoSet, NULL, &HidGuid, USBDevNO, &DevInterfaceData);
		if(!bSuccess)
		{
			m_listInfor_Disp.InsertString(-1, " ");			//向ListBox中插入内容
			m_listInfor_Disp.InsertString(-1, "USB设备已经查找完毕,请进行其他相关操作...");			//向ListBox中插入内容
			break;
		}

		CString strTemp;//临时CString变量
		strTemp.Format("USB设备---%d:", USBDevNO+1);
		m_listInfor_Disp.InsertString(-1, " ");				//向ListBox中插入空行
		m_listInfor_Disp.InsertString(-1, strTemp);			//向ListBox中插入内容

		DWORD RequiredSize = 0;								//存放USB设备详细信息变量
		SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, NULL, 0, &RequiredSize, NULL);	//获取信息成功则继续获取设备详细信息
		PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetailData;												//定义一个指向设备详细信息的结构体指针
		pDevDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(RequiredSize);						//分配大小为RequiredSize缓冲区用来保存设备详细信息
		if (NULL == pDevDetailData)
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);			//调用该函数返回设备接口的详细信息
			m_listInfor_Disp.InsertString(-1, "malloc error");
			USBDevNO++;											//设备数量递增
			continue;
		}
		pDevDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, pDevDetailData, RequiredSize, NULL, NULL)) //再次调用获取设备详细信息
		{		
			SetupDiDestroyDeviceInfoList(hDevInfoSet);	//销毁设备信息集合，并且释放所有关联的内存
			free(pDevDetailData);						//释放刚才销毁的内存
			m_listInfor_Disp.InsertString(-1, "SetupDiGetDeviceInterfaceDetail error");
			USBDevNO++;
			continue;
		}
		CString strDevicePath;						//定义存放设备的路径
		strDevicePath= pDevDetailData->DevicePath;	//将设备路径复制出来后销毁申请的内存
		m_pathArray.Add(strDevicePath);				//把检测到的设备路径存放到CStringArray
		free(pDevDetailData);						//释放内存

		//使用CreateFile函数获取设备的属性，包括VID、PID、版本号等
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
			HIDD_ATTRIBUTES DevAttributes; //定义HIDD_ATTRIBUTES结构体保存设备属性
			DevAttributes.Size = sizeof(HIDD_ATTRIBUTES);
			if (!HidD_GetAttributes(hDevHandle, &DevAttributes)) //获取设备属性
			{
				CloseHandle(hDevHandle); //关闭刚刚打开的设备
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				m_listInfor_Disp.InsertString(-1, "HidD_GetAttributes error");
			}
			strTemp.Format("VID: %04X    PID: %04X    VNum: %04X", DevAttributes.VendorID, DevAttributes.ProductID, DevAttributes.VersionNumber);
			m_listInfor_Disp.InsertString(-1, strTemp);
			WCHAR mString[256]; 
			char Buffer[256]; 
			HidD_GetProductString(hDevHandle, mString, sizeof(mString)); //获取产品名称
			if (wcstombs(Buffer, mString, 256) != -1)
			{
				m_listInfor_Disp.InsertString(-1, "产品名称: "+CString(Buffer));
			}
			else
			{
				m_listInfor_Disp.InsertString(-1, "HidD_GetProductString error");
			}	
			CloseHandle(hDevHandle);

			if((CString(Buffer) == "NTROLLER") || (CString(Buffer) == "LLEOLLER"))	
			{
				nJoyStickNO = USBDevNO;
				strTemp="操控手柄";
			}
			else
			{
				strTemp.Format("USB设备 %d", USBDevNO+1);
			}
			m_ComBoxDevChoice.InsertString(-1, strTemp); //添加设备到选择框
			USBDevNO++;
		}
	}
	m_listInfor_Disp.SetTopIndex(m_listInfor_Disp.GetCount()-1);
	SetupDiDestroyDeviceInfoList(hDevInfoSet);
}



void CMINI_ROVDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
//	CDialogEx::OnCancel();
	Func_Exit_System();
}

void CMINI_ROVDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CMINI_ROVDlg::Func_Exit_System(void)//退出系统函数
{
	if(MessageBox("确定要退出系统？","提示",MB_OKCANCEL)==IDOK)
	{
		cvReleaseCapture(&capture);
		cvReleaseCapture(&capture);
		b_exit_System=TRUE;
		OnOK();
	}	
}

void CMINI_ROVDlg::OnBnClickedBtnExitSystem()
{
	// TODO: 在此添加控件通知处理程序代码
	Func_Exit_System();
}


void CMINI_ROVDlg::OnBnClickedBtnClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_listInfor_Disp.ResetContent();
}


void CMINI_ROVDlg::OnBnClickedBtnConnctdev()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTitle;
	GetDlgItemText(IDC_BTN_CONNCTDEV, strTitle);
	
	int nCurSel=-1;
	if (strTitle == "连接设备")
	{
		nCurSel = m_ComBoxDevChoice.GetCurSel();//获得选中的选项框序号
		if (nCurSel == CB_ERR)//选项框中为空，则提示错误
		{
			MessageBox("请选择将要连接的设备", "提示", MB_OK|MB_ICONWARNING);
			return;
		}

		if( joyGetNumDevs() == 0 )
		{
			GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("连接无效的USB设备");
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
				
			SetDlgItemText(IDC_BTN_CONNCTDEV, "断开连接");
			if (nJoyStickNO == nCurSel )
			{
				SetTimer(TIMER_JOYSTAICK,100,NULL); //100ms读取一次
				GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("操控手柄已连接成功");
			}
			else
			{
				GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("USB设备已连接成功");
			}
		}
	}
	else
	{	
		KillTimer(TIMER_JOYSTAICK);
		SetDlgItemText(IDC_BTN_CONNCTDEV, "连接设备");
		GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("设备已断开连接");
	}
}

void CMINI_ROVDlg::OnBnClickedBtnOpenVideo()
{
	// TDO:在此添加控件通知处理程序代码
	if(!capture)  //查看是否有摄像头设备存在
	{  
		capture = cvCaptureFromCAM(VIDEO); //获得摄像头视频  
		if(!capture)
		{
			GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("未找到监控设备");
			return;
		}	
	} 
	GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("视频监控已开启");
	SetTimer(TIMER_CAP,10,NULL); //10ms捕获一次图像
}


void CMINI_ROVDlg::OnBnClickedBtnCloseVideo()
{
	// TODO: 在此添加控件通知处理程序代码
	cvReleaseCapture(&capture);  
	GetDlgItem(IDC_STATIC_VIDEOSTATUS)->SetWindowTextA("视频监控已关闭");
}

void CMINI_ROVDlg::OnBnClickedBtnrorward()
{
	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=1;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}

void CMINI_ROVDlg::OnBnClickedBtnback()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=2;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}

void CMINI_ROVDlg::OnBnClickedBtnturnleft() //简单的向左差动转弯
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=3;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}

void CMINI_ROVDlg::OnBnClickedBtnright()//简单向右差动转弯
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=4;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}

void CMINI_ROVDlg::OnBnClickedBtnComeup()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=5;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}


void CMINI_ROVDlg::OnBnClickedBtndive()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=6;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}


void CMINI_ROVDlg::OnBnClickedBtncapture()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=7;
}


void CMINI_ROVDlg::OnBnClickedBtnrealese()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(TIMER_UIOPERATE,100,NULL);
	g_nUI_Operate_Value=8;
}
void CMINI_ROVDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(TIMER_UIOPERATE, 100, NULL);
	g_nUI_Operate_Value = 5;
}




void CMINI_ROVDlg::OnBnClickedButton4()
{// TODO: 在此添加控件通知处理程序代码
	SetTimer(TIMER_UIOPERATE, 100, NULL);
	g_nUI_Operate_Value = 6;
}
void CMINI_ROVDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value=9;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}


void CMINI_ROVDlg::OnBnClickedBtnR2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value = 10;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}




void CMINI_ROVDlg::OnBnClickedBtnL1()
{
	m_Slider_Speed.SetRange(ZERO, ROCKER_MIDDLE - ROCKER_MIN);			//初始化滑条PWM_STOP为停止
	m_Slider_Speed.SetTicFreq(ZERO);
	m_Slider_Speed.SetPos(ZERO);
	g_nSlider_Value = ZERO;
	g_nUI_Operate_Value = 11;
	GetDlgItem(IDC_STATIC_START)->SetWindowTextA("停止");
	GetDlgItem(IDC_STATIC_END)->SetWindowTextA("快速");
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA("速度值:0");
}


void CMINI_ROVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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

/************舵机状态值定义************/



void CMINI_ROVDlg::Timer_JoyStick(void)
{

	int nscrollpos_R_X=PWM_STOP;//右摇杆的X轴 机器人左转、右转，且在上位机软件的左转右转滑块上显示
	int nscrollpos_R_Y=PWM_STOP;//右摇杆的Y轴 机器人前进、后退，且在上位机软件的前进后退滑块上显示

	int nscrollpos_L_X=PWM_STOP;//左摇杆的X轴 机器人的机械手抓取与释放，且在上位机软件的抓取释放滑块上显示
	int nscrollpos_L_Y=PWM_STOP;//左摇杆的Y轴 机器人上浮下潜，且在上位机软件的上浮下潜滑块上显示

	int LX = 0, LY = 0, RX = 0, RY = 0, zypy = 0, zyxz = 0, qhpy = 0, sxfd = 0;
	int up_pwm = 0, l1_pwm = 0, l2_pwm = 0, r1_pwm = 0, r2_pwm = 0; //(-7200~7200)
	static unsigned int zhangjiao = 120; //(2000)
	joyGetPosEx(Joystickapp.joystickid, &joyinfo);

	/*****************柄值32511为中间值，即停止，对应电调的PWM值为1565左右，手柄大小每变化72，则PWM对应变化为1***********************/
	/**因为电调PWM的范围为PWM_LOW_VALUE-PWM_MOST_VALUE，当1565-PWM_LOW_VALUE时为正转，，1565-PWM_MOST_VALUE时为反转，且速度逐渐增大**/
	/*****************对应摇杆来说，值为32511-65535这个之间则为反转，值为32511-0之间为正转，且电机速度逐渐增大***********************/
	/**********************电调计算公式为：1100+摇杆值/72=PWM值，即为要发送数据的数据************************************************/
	
	/******************对于控制舵机来说,手柄正前方的7和8键分别用来控制机械手的抓取和释放***************************************/
	Joystickapp.wJoyXPos_R = WORD(joyinfo.dwZpos);//左转、右转 
	Joystickapp.wJoyYPos_R = WORD(joyinfo.dwRpos);//前进、后退

	Joystickapp.wJoyXPos_L = WORD(joyinfo.dwXpos);//预留
	Joystickapp.wJoyYPos_L = WORD(joyinfo.dwYpos);//上浮、下潜



	Joystickapp.fJoyButton7Down = ((joyinfo.dwButtons & 64) != 0); //机械手抓取
	Joystickapp.fJoyButton8Down = ((joyinfo.dwButtons & 128) != 0);//机械手释放
	Joystickapp.fJoyButton5Down = ((joyinfo.dwButtons & 64)!= 0);   //舵机2
	Joystickapp.fJoyButton6Down = ((joyinfo.dwButtons & 128) != 0);  //舵机2
	

	/**************************************************************************/


	/***********************摇杆数据直接发送给下位机，范围1100~2010 中值1551**************************************/

	nscrollpos_R_X=Joystickapp.wJoyXPos_R/72+PWM_LOW_VALUE;			//获得右摇杆的X轴数据
	nscrollpos_R_Y=Joystickapp.wJoyYPos_R/72+PWM_LOW_VALUE;			//获得右摇杆的Y轴数据

	nscrollpos_L_X=Joystickapp.wJoyXPos_L/72+PWM_LOW_VALUE;			//获得左摇杆的X轴数据
	nscrollpos_L_Y=Joystickapp.wJoyYPos_L/72+PWM_LOW_VALUE;			//获得左摇杆的Y轴数据



	LX = nscrollpos_L_X - ROCKER_MIDDLE;
	LY = nscrollpos_L_Y - ROCKER_MIDDLE;
	RX = nscrollpos_R_X - ROCKER_MIDDLE;
	RY = nscrollpos_R_Y - ROCKER_MIDDLE;

	if (LX>10) //右移  zypy
	{
		zyxz = LX - 10;
	}
	else if (LX<-10) //左移
	{
		zyxz = LX + 10;
	}
	else //停止
	{
		zyxz = 0;
	}

	if (LY>10)//下降   LY
	{

		sxfd = LY - 10;
	}
	else
	if (LY<-10)//上升 
	{
		sxfd = LY + 10;
	}
	else //停止
	{
		sxfd = 0;
	}

	if (RX>10)//左转   zyxz
	{
		zypy = RX - 10;
	}
	else if (RX<-10) //右转
	{
		zypy = RX + 10;
	}
	else //停止
	{
		zypy = 0;
	}

	if (RY>10) //后退  qhpy
	{
		qhpy = RY - 10;
	}
	else if (RY<-10) //前进
	{
		qhpy = RY + 10;
	}
	else //停止
	{
		qhpy = 0;
	}
	up_pwm = sxfd*MAGNIFICATION; l1_pwm = MAGNIFICATION*(-zypy - zyxz + qhpy); l2_pwm = MAGNIFICATION*(zypy - zyxz + qhpy); r1_pwm = MAGNIFICATION*(-zypy - zyxz - qhpy); r2_pwm = MAGNIFICATION*(-zypy + zyxz + qhpy);
	if (up_pwm>PWM_MAX) up_pwm = PWM_MAX;if (up_pwm<-PWM_MAX) up_pwm = -PWM_MAX;
	if (l1_pwm>PWM_MAX) l1_pwm = PWM_MAX; if (l1_pwm<-PWM_MAX) l1_pwm = -PWM_MAX;
	if (l2_pwm>PWM_MAX) l2_pwm = PWM_MAX; if (l2_pwm<-PWM_MAX) l2_pwm = -PWM_MAX;
	if (r1_pwm>PWM_MAX) r1_pwm = PWM_MAX; if (r1_pwm<-PWM_MAX) r1_pwm = -PWM_MAX;
	if (r2_pwm>PWM_MAX) r2_pwm = PWM_MAX; if (r2_pwm<-PWM_MAX) r2_pwm = -PWM_MAX;

	if(Joystickapp.fJoyButton7Down == TRUE)		//判断7键是否按下
	{
		if(Joystickapp.fJoyButton8Down != TRUE)	 //8键未按下
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
		if(Joystickapp.fJoyButton8Down == TRUE)		//8键按下
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

	if (Joystickapp.fJoyButton5Down == TRUE)		//判断5键是否按下
	{
		if (Joystickapp.fJoyButton6Down != TRUE)	 //6键未按下
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
		if (Joystickapp.fJoyButton6Down == TRUE)		//6键按下
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

	m_ScrollBar_L_Y.SetScrollPos(nscrollpos_L_Y);	// 在左边竖滑块上显示当前左摇杆的Y值  
	m_ScrollBar_L_X.SetScrollPos(nscrollpos_L_X);	// 在左边横滑块上显示当前左摇杆的X值
	m_ScrollBar_R_Y.SetScrollPos(nscrollpos_R_Y);	// 在右边竖滑块上显示当前右摇杆的Y值   
	m_ScrollBar_R_X.SetScrollPos(nscrollpos_R_X);	// 在右边横滑块上显示当前右摇杆的X值 
	
}






//int nJoyStick_Xpos_R = ROCKER_MIDDLE;//定义右摇杆X轴
//int nJoyStick_Ypos_R = ROCKER_MIDDLE;//定义右摇杆Y轴
//int nJoyStick_Xpos_L = ROCKER_MIDDLE;//定义左摇杆X轴
//int nJoyStick_Ypos_L = ROCKER_MIDDLE;//定义左摇杆Y轴
//bool vertical = false;


void CMINI_ROVDlg::Timer_UI(void)//值大于1565，则电机反正，小于1565则电机正转，舵机除外
{
	int LX = 0, LY = 0, RX = 0, RY = 0, zypy = 0, zyxz = 0, qhpy = 0, sxfd = 0;
	int up_pwm = 0, l1_pwm = 0, l2_pwm = 0, r1_pwm = 0, r2_pwm = 0; //(-7200~7200)
	static unsigned int zhangjiao = 120; //(2000)
	switch(g_nUI_Operate_Value)
	{
	case 1://前进 值越小速度越大
		{
			/************** 因为滑块的值是从PWM_STOP开始的，电机正转时，滑块的值在增大，对应的PWM值应该减小才能提高正转速度******/
			   nJoyStick_Ypos_R = ROCKER_MIDDLE-g_nSlider_Value;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			  

		}
		break;
	case 2://后退 值越大速度越大
		{
			
			   nJoyStick_Ypos_R = ROCKER_MIDDLE + g_nSlider_Value;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			  
		}
		break;
	case 3://左移 右电机正转，左电机反向，简单差速转向
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE ;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE- g_nSlider_Value;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			 
		}
		break;
	case 4://右移 右电机反转，左电机正转，简单差速转向
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE + g_nSlider_Value;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			  
		}
		break;
	case 5://上浮
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE ;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE- g_nSlider_Value;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			   
		}
		break;
	case 6://下沉
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE + g_nSlider_Value;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			   
		}
		break;
	case 7://抓紧
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE ;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			   zhangjiao += 20;
			   if (zhangjiao>250)
			   {
				   zhangjiao = 250;
			   }
		}
		break;
	case 8://松开
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
			   zhangjiao -= 20;
			   if (zhangjiao<120)
			   {
				   zhangjiao = 120;
			   }
		}
		break;
	case 9://停止
		{
			   nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
			   nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
			   nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
			   nJoyStick_Xpos_L = ROCKER_MIDDLE;							//舵机停止转动
		}
		break;
		case 10://右转
		{
					nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
					nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
					nJoyStick_Ypos_L = ROCKER_MIDDLE;							//垂推电机停止转动
					nJoyStick_Xpos_L = ROCKER_MIDDLE + g_nSlider_Value;							//舵机停止转动
		}
		break;
		case 11://左转
		{
					nJoyStick_Ypos_R = ROCKER_MIDDLE;						//右电机PWM值
					nJoyStick_Xpos_R = ROCKER_MIDDLE;						//左电机PWM值
					nJoyStick_Ypos_L = ROCKER_MIDDLE ;							//垂推电机停止转动
					nJoyStick_Xpos_L = ROCKER_MIDDLE- g_nSlider_Value;							//舵机停止转动
		}
		break;
	default:
		break;
	}


	LX = nJoyStick_Xpos_L - ROCKER_MIDDLE;
	LY = nJoyStick_Ypos_L - ROCKER_MIDDLE;
	RX = nJoyStick_Xpos_R - ROCKER_MIDDLE;
	RY = nJoyStick_Ypos_R - ROCKER_MIDDLE;

	if (LX>10) //右移  zypy
	{
		zypy = LX - 10;
	}
	else if (LX<-10) //左移
	{
		zypy = LX + 10;
	}
	else //停止
	{
		zypy = 0;
	}

	if (LY>10)//下降   LY
	{

		sxfd = LY - 10;
	}
	else
	if (LY<-10)//上升 
	{
		sxfd = LY + 10;
	}
	else //停止
	{
		sxfd = 0;
	}

	if (RX>10)//左转   zyxz
	{
		zyxz = RX - 10;
	}
	else if (RX<-10) //右转
	{
		zyxz = RX + 10;
	}
	else //停止
	{
		zyxz = 0;
	}

	if (RY>10) //后退  qhpy
	{
		qhpy = RY - 10;
	}
	else if (RY<-50) //前进
	{
		qhpy = RY + 10;
	}
	else //停止
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


/*********************************************通信协议函数说明**************************************************************/
/****************************************nscrollpos_L_Y左摇杆y轴值，nscrollpos_L_X左摇杆x轴值*******************************/
/****************************************nscrollpos_R_Y右摇杆y轴值，nscrollpos_R_X右摇杆x轴值*******************************/
/******************************************************END******************************************************************/
void CMINI_ROVDlg::Func_Send_Data(unsigned int up_pwm, unsigned int l1_pwm, unsigned int l2_pwm, unsigned int r1_pwm, unsigned int r2_pwm, unsigned char zhangjiao)
{
	char Data_Buf[16] = { 0 };				//向串口发送数据数组

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

	Data_Buf[0] = up_pwm_most_bit;		//左摇杆y轴高位
	Data_Buf[1] = up_pwm_low_bit;		//左摇杆y轴低位
	Data_Buf[2] = up_pwm_most_bit - up_pwm_low_bit; //校验
	Data_Buf[3] = l1_pwm_most_bit;		//左摇杆x轴高位
	Data_Buf[4] = l1_pwm_low_bit;		//左摇杆x轴低位
	Data_Buf[5] = l1_pwm_most_bit - l1_pwm_low_bit; //校验
	Data_Buf[6] = l2_pwm_most_bit;	//右摇杆y轴高位
	Data_Buf[7] = l2_pwm_low_bit;	//右摇杆y轴低位
	Data_Buf[8] = l2_pwm_most_bit - l2_pwm_low_bit; //校验
	Data_Buf[9] = r1_pwm_most_bit;	//右摇杆x轴高位
	Data_Buf[10] = r1_pwm_low_bit;	//右摇杆x轴低位
	Data_Buf[11] = r1_pwm_most_bit - r1_pwm_low_bit; //校验
	Data_Buf[12] = r2_pwm_most_bit;	//右摇杆x轴高位
	Data_Buf[13] = r2_pwm_low_bit;	//右摇杆x轴低位
	Data_Buf[14] = r2_pwm_most_bit - r2_pwm_low_bit; //校验
	Data_Buf[15] = zhangjiao;

	m_Commend_Control.Func_CommendControl_Send(Data_Buf,16);//向口发送数据
}

/*********************字符处理程序，用于判断信号处理板串口返回数据的有效性**********************/
unsigned char  cFibre_rxbuf[10];        //存放收到的数据
int            nFibre_rxnum=0;          //收到数据计数
int  characterSigTraEof(unsigned char c)            
{			
	cFibre_rxbuf[nFibre_rxnum]=c;
	nFibre_rxnum++;
	BYTE bTempCmd1;										    //定义1个临时变量
	return 0;
}


BOOL CMINI_ROVDlg::Func_Com_Init(void)
{
	BOOL  bFibre_Com_InitFlag=FALSE;//下位机串口初始化标志
	if(!bFibre_Com_InitFlag)
	{
		bFibre_Com_InitFlag=m_Commend_Control.Func_CommendControl_Com_Init(this->m_hWnd);		//下位机串口初始化
		if(!bFibre_Com_InitFlag)
		{
			GetDlgItem(IDC_STATIC_COMSTATUS)->SetWindowTextA("串口初始化失败,请检查设备......");
			GetDlgItem(IDC_BTN_FIBRECOM_INIT)->EnableWindow(TRUE);
			return FALSE;
		}
	}

	GetDlgItem(IDC_BTN_FIBRECOM_INIT)->EnableWindow(FALSE);

	GetDlgItem(IDC_STATIC_COMSTATUS)->SetWindowTextA("串口初始化成功");
	return TRUE;
}


void CMINI_ROVDlg::OnBnClickedBtnFibrecomInit()
{
	// TODO: 在此添加控件通知处理程序代码
	Func_Com_Init();
}

void CMINI_ROVDlg::OnCbnSelendokComboOpertype()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	g_nOperType_Choice=m_ComBoxOperType.GetCurSel();
	switch(g_nOperType_Choice)
	{		
	case 0:
		KillTimer(TIMER_JOYSTAICK);

		SetDlgItemText(IDC_BTN_CONNCTDEV, "连接设备");
		GetDlgItem(IDC_STATIC_DEVCON)->SetWindowTextA("设备已断开连接");

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
		SetDlgItemText(IDC_BTN_CONNCTDEV, "连接设备");
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl *pSlidCtrl=(CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED);
	g_nSlider_Value=pSlidCtrl->GetPos();//取得当前位置值
	CString strTemp;
	strTemp.Format("速度值:%d", g_nSlider_Value);
	GetDlgItem(IDC_STATIC_SPEED_VALUE)->SetWindowTextA(strTemp);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}






	

