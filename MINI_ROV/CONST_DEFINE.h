//��ʱ������
#define  TIMER_CAP       1			//���ڲ���ͼ��ʱ�Ķ�ʱʹ��
#define  TIMER_JOYSTAICK 2			//���ڶ�ȡ��Ϸ�ֱ�����ʱ��ʱ
#define  TIMER_UIOPERATE 3			//���ڲ���������˶�����ʱ��ʱ��������

#define  PWM_STOP			1565	//���PWM��ֵֹ
#define  PWM_REVERSE_VALUE  3130    //�����������ʱ�����ó���
#define  PWM_LOW_VALUE      1100	//���PWM��Сֵ
#define  PWM_MOST_VALUE     2000	//���PWM���ֵ


#define PWM_MAX 5000
#define MAGNIFICATION 10





#define ZERO 0
#define ROCKER_MIN 1100
#define ROCKER_MIDDLE 1551
#define ROCKER_MAX 2010
//#define  PWM_LOW_VALUE_SER  900			//���PWM��Сֵ ��Ӧ��е���ͷ�
//#define  PWM_MOST_VALUE_SER 1500		//���PWM���ֵ ��Ӧ��е��ץȡ
//#define  PWE_STOP_VLAUE_SER  1200		//�����ֵֹ

/*************************���д��ں궨��*********************************/
//�������� ���ڶ���
#define COM_COMMEND_NAME		"COM10"
#define COM_COMMEND_BAUD		9600
#define COM_COMMEND_CODE		3

//��������ͷ
#define  VIDEO 1    //ѡ���ĸ�����ͷһ���������1������Ĭ������ͷ��0 

typedef struct _JoystickAPP
{
	HWND hwndApp;
	UINT joystickid;

	WORD wJoyXPos_R; //��ҡ�˵�X��
	WORD wJoyYPos_R; //��ҡ�˵�Y��
	WORD wJoyXPos_L; //��ҡ�˵�X���ڳ�����ʱΪZ�ᣩ��
	WORD wJoyYPos_L; //��ҡ�˵�Y(�ڳ�������ΪR��)��

	BOOL fJoyButton7Down;	 //�������
	BOOL fJoyButton8Down;	//����ͷ�
	

	BOOL fJoyButton5Down;	 //�������
	BOOL fJoyButton6Down;	//����ͷ�

}JoystickAPP;
