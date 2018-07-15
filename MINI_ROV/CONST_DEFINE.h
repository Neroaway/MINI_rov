//计时器定义
#define  TIMER_CAP       1			//用于捕获图像时的定时使用
#define  TIMER_JOYSTAICK 2			//用于读取游戏手柄数据时定时
#define  TIMER_UIOPERATE 3			//用于操作界面对运动控制时定时发送数据

#define  PWM_STOP			1565	//电机PWM终止值
#define  PWM_REVERSE_VALUE  3130    //电机反响运行时计算用常数
#define  PWM_LOW_VALUE      1100	//电机PWM最小值
#define  PWM_MOST_VALUE     2000	//电机PWM最大值


#define PWM_MAX 5000
#define MAGNIFICATION 10





#define ZERO 0
#define ROCKER_MIN 1100
#define ROCKER_MIDDLE 1551
#define ROCKER_MAX 2010
//#define  PWM_LOW_VALUE_SER  900			//舵机PWM最小值 对应机械手释放
//#define  PWM_MOST_VALUE_SER 1500		//舵机PWM最大值 对应机械手抓取
//#define  PWE_STOP_VLAUE_SER  1200		//舵机终止值

/*************************所有串口宏定义*********************************/
//控制命令 串口定义
#define COM_COMMEND_NAME		"COM10"
#define COM_COMMEND_BAUD		9600
#define COM_COMMEND_CODE		3

//定义摄像头
#define  VIDEO 1    //选择哪个摄像头一般情况下是1，电脑默认摄像头是0 

typedef struct _JoystickAPP
{
	HWND hwndApp;
	UINT joystickid;

	WORD wJoyXPos_R; //右摇杆的X轴
	WORD wJoyYPos_R; //右摇杆的Y轴
	WORD wJoyXPos_L; //左摇杆的X（在程序处理时为Z轴）轴
	WORD wJoyYPos_L; //左摇杆的Y(在程序处理是为R轴)轴

	BOOL fJoyButton7Down;	 //舵机捕获
	BOOL fJoyButton8Down;	//舵机释放
	

	BOOL fJoyButton5Down;	 //舵机捕获
	BOOL fJoyButton6Down;	//舵机释放

}JoystickAPP;
