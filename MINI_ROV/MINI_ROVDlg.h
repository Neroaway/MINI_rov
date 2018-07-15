
// MINI_ROVDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include"Com.h"
#include "Commend_Control.h"
#include <windows.h>
#include "mmsystem.h" //Joystick
#include "afxcmn.h"
#pragma comment(lib,"winmm.lib")

// CMINI_ROVDlg 对话框
class CMINI_ROVDlg : public CDialogEx
{
// 构造
public:
	CMINI_ROVDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MINI_ROV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSearchDev();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void Func_Exit_System(void);
//	CComboBox m_comboDevice;
	HANDLE hReadHandle;			//设备读取句柄
	CStringArray m_pathArray;	//存储设备路径
	BOOL m_bOpen;				//设备打开状态
	CListBox m_listInfor_Disp;	//ListBox框显示信息

	JOYINFOEX	joyinfo;		//操纵杆属性信息

	afx_msg void OnBnClickedBtnExitSystem();
	afx_msg void OnBnClickedBtnClear();
	CComboBox m_ComBoxDevChoice;
	afx_msg void OnBnClickedBtnConnctdev();
	afx_msg void OnBnClickedBtnOpenVideo();
	afx_msg void OnBnClickedBtnCloseVideo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void Timer_CapVideo(void);
	BOOL Func_Com_Init(void);
	afx_msg void OnBnClickedBtnFibrecomInit();

//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void Timer_JoyStick(void);
//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	CScrollBar m_ScrollBar_L_Y;
	CScrollBar m_ScrollBar_R_Y;
	CScrollBar m_ScrollBar_L_X;
	CScrollBar m_ScrollBar_R_X;
	CComboBox m_ComBoxOperType;
	afx_msg void OnBnClickedBtnComeup();
	afx_msg void OnBnClickedBtndive();
	afx_msg void OnBnClickedBtnStop();
	void Func_Send_Data(unsigned int up_pwm, unsigned int l1_pwm, unsigned int l2_pwm, unsigned int r1_pwm, unsigned int r2_pwm, unsigned char zhangjiao);
	afx_msg void OnBnClickedBtnrorward();
	afx_msg void OnBnClickedBtnback();
	afx_msg void OnBnClickedBtnturnleft();
	afx_msg void OnBnClickedBtnright();
	afx_msg void OnCbnSelendokComboOpertype();
	void Timer_UI(void);
	CSliderCtrl m_Slider_Speed;
//	afx_msg void OnCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtncapture();
	afx_msg void OnBnClickedBtnrealese();
	afx_msg void OnBnClickedBtnR2();
	afx_msg void OnBnClickedBtnL1();
private:
	// //松开
	CButton Button_loose;
public:
	// 抓紧
	CButton butten_catch;






	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
};
