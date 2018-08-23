// Window Programming 기말 프로젝트
//                              2015. 1학기
//                              장성우, 목태수
//--------------------------------------------------------------------

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "resource.h"
#include "inc/fmod.hpp" 
#pragma comment (lib, "fmodex_vc.lib")  


#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" )
#define PI 3.141592

using namespace std;

typedef struct _Point
{
	int x;
	int y;
}Point;

//Obstacle
Point Obstacle[2];
HBITMAP hObstacleBit;

int player1HP = 100;
int player2HP = 100;


enum SOUONDKIND
{ //enum 만들어서 배열처럼 음악 관리하면 쉬움 이런식임.
	SD_Background, //0번
	SD_Effect1,
	SD_Effect2,
	SD_End //17번
};



/// 층돌체크 범위(반지름)
double CheckRange = 50.0;

FMOD_SYSTEM *g_System; //사운드 시스템 생성 하는 부분이고
FMOD_SOUND *g_Sound[SD_End]; //사운드 설정하는부분
FMOD_CHANNEL *g_Channel[SD_End]; //채널 설정하는부분이고





LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
void CALLBACK TimerProc1(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

HINSTANCE g_hInst;

LPCTSTR lpszClass = "Aqua Fight";

HWND hWnd;

HDC mem1dc, mem2dc, mem3dc, mem4dc, mem5dc, mem6dc;
HBITMAP hBit1, hPlayer1Bit, hPlayer2Bit, hLeftMissileBit, hBit4, hBit5, hBit6, oldBit1, oldBit2;
HBITMAP hBackgroundBit;
HDC hdc, memdc;

struct Missile{
	float missileAngle;
	float calibrateLineMissileAngleX;
	float calibrateLineMissileAngleY;
	bool Shoot;
	bool calibrateMissileTrigger;

	int MissileAnimationX;
	int MissileAnimationY;
	int MissilePositionX;
	int MissilePositionY;

	int efftectPositionX;
	int efftectPositionY;
	int efftectAnimationX;
	int efftectAnimationY;

	int firePositionX;
	int firePositionY;
	int fireAnimationX;
	int fireAnimationY;

	float missileAngle2;
	float calibrateLineMissileAngleX2;
	float calibrateLineMissileAngleY2;
	bool Shoot2;
	bool calibrateMissileTrigger2;

	int MissileAnimationX2;
	int MissileAnimationY2;
	double MissilePositionX2;
	double MissilePositionY2;

	int efftectPositionX2;
	int efftectPositionY2;
	int efftectAnimationX2;
	int efftectAnimationY2;

	int firePositionX2;
	int firePositionY2;
	int fireAnimationX2;
	int fireAnimationY2;


};

// player1 missile
Missile player1Missle[10];
HBITMAP hPlayer1MissleBit;
int missleNumber = 0;
int newMissleNumber = 0;
HDC missleMem1dc;

// player2 missile
Missile player2Missle[10];
HBITMAP hPlayer2MissleBit;
int missleNumber2 = 0;
int newMissleNumber2 = 0;
HDC missleMem2dc;


int yPos, xPos;
// 미사일과 움직임 sin 각도
float speedAngle = 0;
float missileAngle = 0;
float speedAngle2 = 0;

// 미사일 움직임 조정
float calibrateLineMissileAngleX = 90;
float calibrateLineMissileAngleY = 90;
bool calibrateMissileTrigger = false;

bool shootPlayer1 = false;

// Animation을 위한 변수 선언
int player1AnimationX = 0;
int player1AnimationY = 0;
int player2AnimationX = 0;
int player2AnimationY = 0;

int player1MissileAnimationX = 100;
int player1MissileAnimationY = 410;

// position
int player1PositionX = 700;
int player1PositionY = 500;
int player2PositionX = 100;
int player2PositionY = 500;

int player1MissilePositionX = 0;
int player1MissilePositionY = 0;

// 자연스러운 움직임을 위한 Trigger
int afterimageMoveTrigger = 0;
int afterimageMoveTrigger2 = 0;


// 미사일 sprite 회전 
XFORM xFormRotate;
int degree;

float volume = 0.5f;

void Init()
{
	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, SD_End, FMOD_INIT_NORMAL, NULL);


	//배경음
	FMOD_System_CreateStream(g_System, "Sound/SD_Background.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_Background]);
	FMOD_System_CreateSound(g_System, "Sound/explosion01.wav", FMOD_DEFAULT, 0, &g_Sound[SD_Effect1]);
	//FMOD_System_CreateSound(g_System, "Sound/SD_Stage2.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_Effect2]);

	// 형식은 :                g_system에, 경로, 재생 방식(계속 음악끝나면 반복), 사운드 설정 해주고
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Background], 0, &g_Channel[SD_Background]);
	
}



void Release() //마지막으로 음악 다 재생 되면 이런식으로 릴리즈해줘야 해.
{
	for (int i = 0; i < SD_End; ++i)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	//HWND hWnd;
	Init();
	MSG Message;
	WNDCLASS WndClass;
	//   HACCEL hAcc;
	//hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	g_hInst = hInstance;
	// 윈도우 클래스 구조체 값 설정
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	// 윈도우 클래스 등록
	RegisterClass(&WndClass);
	// 윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0,
		1300, 750, NULL, (HMENU)NULL, hInstance, NULL);
	// 윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// 이벤트 루프 처리
	while (1){
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)){
			if (Message.message == WM_QUIT)
				break;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return Message.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


	//HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	//   RECT memRect;
	static SIZE size;
	static BOOL Drag;
	static int timer = 0;
	int mx, my;
	static HBRUSH MyBrush, OldBrush;
	static HPEN MyPen, OldPen;
	GetClientRect(hWnd, &rect);



	
	//MyBrush = CreateSolidBrush(RGB(255, 255, 255));
	//MyPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	// 메시지 처리하기
	switch (uMsg) {

	case WM_CREATE:
		Drag = false;
		yPos = -30;

		MyPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		////Obstacle
		Obstacle[0].x = 300;
		Obstacle[0].y = 250;

		Obstacle[1].x = 700;
		Obstacle[1].y = 500;


		
		hObstacleBit = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		

		// player 1
		hPlayer1Bit = (HBITMAP)LoadBitmap(g_hInst,
			MAKEINTRESOURCE(IDB_BITMAP1));

		// player 2
		hPlayer2Bit = (HBITMAP)LoadBitmap(g_hInst,
			MAKEINTRESOURCE(IDB_BITMAP2));

		// Missile
	
		// Background
		hBackgroundBit = (HBITMAP)LoadBitmap(g_hInst,
			MAKEINTRESOURCE(IDB_BITMAP5));


		// player 1 missile
	
			hPlayer1MissleBit = (HBITMAP)LoadBitmap(g_hInst,
				MAKEINTRESOURCE(IDB_BITMAP4));
			

		for (int i = 0; i < 10; i++)
		{
			player1Missle[i].calibrateLineMissileAngleX = 90;
			player1Missle[i].calibrateLineMissileAngleY = 90;
			player1Missle[i].calibrateMissileTrigger = false;
			player1Missle[i].missileAngle = 0;
			player1Missle[i].MissileAnimationX = 100;
			player1Missle[i].MissileAnimationY = 410;
			player1Missle[i].MissilePositionX = 0;
			player1Missle[i].MissilePositionY = 0;
			player1Missle[i].Shoot = false;
			player1Missle[i].efftectAnimationX = 365;
			player1Missle[i].efftectAnimationY = 90;
			player1Missle[i].firePositionX = 0;
			player1Missle[i].firePositionY = 0;
			player1Missle[i].fireAnimationX = 365;
			player1Missle[i].fireAnimationY = 95;



			player1Missle[i].calibrateLineMissileAngleX2 = 90;
			player1Missle[i].calibrateLineMissileAngleY2 = 90;
			player1Missle[i].calibrateMissileTrigger = false;
			player1Missle[i].missileAngle2 = 0;
			player1Missle[i].MissileAnimationX2 = 5;
			player1Missle[i].MissileAnimationY2 = 140;
			player1Missle[i].MissilePositionX2 = 0;
			player1Missle[i].MissilePositionY2 = 0;
			player1Missle[i].Shoot2 = false;
			player1Missle[i].firePositionX2 = 0;
			player1Missle[i].firePositionY2 = 0;
			player1Missle[i].fireAnimationX2 = 365;
			player1Missle[i].fireAnimationY2 = 95;

		}

		// player 2 missile
	
		hPlayer2MissleBit = (HBITMAP)LoadBitmap(g_hInst,
			MAKEINTRESOURCE(IDB_BITMAP3));


		for (int i = 0; i < 10; i++)
		{
			player2Missle[i].calibrateLineMissileAngleX = 90;
			player2Missle[i].calibrateLineMissileAngleY = 90;
			player2Missle[i].calibrateMissileTrigger = false;
			player2Missle[i].missileAngle = 0;
			player2Missle[i].MissileAnimationX = 0;
			player2Missle[i].MissileAnimationY = 0;
			player2Missle[i].MissilePositionX = 0;
			player2Missle[i].MissilePositionY = 0;
			player2Missle[i].Shoot = false;
			player2Missle[i].firePositionX = 0;
			player2Missle[i].firePositionY = 0;
			player2Missle[i].fireAnimationX = 165;
			player2Missle[i].fireAnimationY = 380;


			player2Missle[i].calibrateLineMissileAngleX2 = 90;
			player2Missle[i].calibrateLineMissileAngleY2 = 90;
			player2Missle[i].calibrateMissileTrigger = false;
			player2Missle[i].missileAngle2 = 0;
			player2Missle[i].MissileAnimationX2 = 10;
			player2Missle[i].MissileAnimationY2 = 100;
			player2Missle[i].MissilePositionX2 = 0;
			player2Missle[i].MissilePositionY2 = 0;
			player2Missle[i].Shoot2 = false;
			player2Missle[i].firePositionX2 = 0;
			player2Missle[i].firePositionY2 = 0;
			player2Missle[i].fireAnimationX2 = 165;
			player2Missle[i].fireAnimationY2 = 380;
		}

		SetTimer(hWnd, 1, 10, TimerProc);
		SetTimer(hWnd, 2, 100, TimerProc1);

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			//case ID_6_3_DIALOG:
		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		Drag = TRUE;

		InvalidateRgn(hWnd, NULL, false);
		break;
	case WM_LBUTTONUP:
		Drag = FALSE;
		mx = LOWORD(lParam);
		my = HIWORD(lParam);

		InvalidateRgn(hWnd, NULL, false);
		break;
	case WM_MOUSEMOVE:

		break;

	case WM_KEYUP:
		if (wParam == VK_NUMPAD4){
			afterimageMoveTrigger = LEFT;
			cout << afterimageMoveTrigger << endl;
			break;
		}

		if (wParam == VK_NUMPAD6){
			afterimageMoveTrigger = RIGHT;
			cout << afterimageMoveTrigger << endl;
			break;
		}

		if (wParam == VK_NUMPAD5){
			afterimageMoveTrigger = DOWN;
			cout << afterimageMoveTrigger << endl;
			break;
		}

		if (wParam == VK_NUMPAD8){
			afterimageMoveTrigger = UP;
			cout << afterimageMoveTrigger << endl;
			break;
		}
		//   cout << afterimageMoveTrigger << endl;
		if (wParam == 0x46){
			afterimageMoveTrigger2 = LEFT;
			cout << afterimageMoveTrigger2 << endl;
			break;
		}

		if (wParam == 0x48){
			afterimageMoveTrigger2 = RIGHT;
			cout << afterimageMoveTrigger2 << endl;
			break;
		}

		if (wParam == 0x47){
			afterimageMoveTrigger2 = DOWN;
			cout << afterimageMoveTrigger2 << endl;
			break;
		}

		if (wParam == 0x54){
			afterimageMoveTrigger2 = UP;
			cout << afterimageMoveTrigger2 << endl;
			break;
		}


		InvalidateRect(hWnd, NULL, false);
		break;

	case WM_KEYDOWN:


		//player 1----------------------------------------------------------------------

		// 두 방향 키 
		if ((GetAsyncKeyState(VK_NUMPAD8)) && (GetAsyncKeyState(VK_NUMPAD4))){
			player1PositionX -= 50 * sin((speedAngle *(PI / 180)));
			player1PositionY -= 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if ((GetAsyncKeyState(VK_NUMPAD5)) && (GetAsyncKeyState(VK_NUMPAD4))){
			player1PositionX -= 50 * sin((speedAngle *(PI / 180)));
			player1PositionY += 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if ((GetAsyncKeyState(VK_NUMPAD5)) && (GetAsyncKeyState(VK_NUMPAD6))){
			player1PositionX += 50 * sin((speedAngle *(PI / 180)));
			player1PositionY += 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if ((GetAsyncKeyState(VK_NUMPAD8)) && (GetAsyncKeyState(VK_NUMPAD6))){
			player1PositionX += 50 * sin((speedAngle *(PI / 180)));
			player1PositionY -= 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		// 한 방향 키
		if ((GetAsyncKeyState(VK_NUMPAD4) & 0x8000)){
			player1PositionX -= 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if ((GetAsyncKeyState(VK_NUMPAD6) & 0x8000)){
			player1PositionX += 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000){
			player1PositionY -= 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}

		if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000){
			player1PositionY += 50 * sin((speedAngle *(PI / 180)));
			if (speedAngle < 90)
				speedAngle += 1;
			break;
		}


		if (GetAsyncKeyState(VK_OEM_PERIOD)){
			player1Missle[missleNumber].Shoot = true;
			player1Missle[missleNumber].calibrateMissileTrigger = true;
			player1Missle[missleNumber].MissilePositionX = player1PositionX + 20;
			player1Missle[missleNumber].MissilePositionY = player1PositionY + 10;
			player1Missle[missleNumber].missileAngle = 0;

			if (missleNumber < 9)
				missleNumber++;
			else
				missleNumber = 0;

			player1Missle[missleNumber].efftectPositionX = player1Missle[missleNumber].MissilePositionX + 50;
			player1Missle[missleNumber].efftectPositionX = player1Missle[missleNumber].MissilePositionY;

			player1Missle[missleNumber].firePositionX = player1Missle[missleNumber].MissilePositionX + 40;
			player1Missle[missleNumber].firePositionY = player1Missle[missleNumber].MissilePositionY;

			break;
		}

		if (GetAsyncKeyState(VK_OEM_COMMA)){
			player1Missle[newMissleNumber].Shoot2 = true;
			player1Missle[newMissleNumber].calibrateMissileTrigger2 = true;
			player1Missle[newMissleNumber].MissilePositionX2 = player1PositionX + 20;
			player1Missle[newMissleNumber].MissilePositionY2 = player1PositionY + 10;
			player1Missle[newMissleNumber].missileAngle2 = 0;

			//   cout <<missleNumber2 <<boolalpha<<player2Missle[missleNumber2].Shoot << endl;
			if (newMissleNumber < 9)
				newMissleNumber++;
			else
				newMissleNumber = 0;

			player1Missle[newMissleNumber].efftectPositionX2 = player1Missle[newMissleNumber].MissilePositionX2 + 50;
			player1Missle[newMissleNumber].efftectPositionX = player1Missle[newMissleNumber].MissilePositionY2;

			player1Missle[newMissleNumber].firePositionX2 = player1Missle[newMissleNumber].MissilePositionX2 + 40;
			player1Missle[newMissleNumber].firePositionY2 = player1Missle[newMissleNumber].MissilePositionY2;


			break;
		}

		//player 2----------------------------------------------------------------------

		// 두 방향 키 
		if ((GetAsyncKeyState(0x54)) && (GetAsyncKeyState(0x46))){
			player2PositionX -= 50 * sin((speedAngle2 *(PI / 180)));
			player2PositionY -= 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		if ((GetAsyncKeyState(0x47)) && (GetAsyncKeyState(0x46))){
			player2PositionX -= 50 * sin((speedAngle2 *(PI / 180)));
			player2PositionY += 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		if ((GetAsyncKeyState(0x47)) && (GetAsyncKeyState(0x48))){
			player2PositionX += 50 * sin((speedAngle2 *(PI / 180)));
			player2PositionY += 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		if ((GetAsyncKeyState(0x54)) && (GetAsyncKeyState(0x48))){
			player2PositionX += 50 * sin((speedAngle2 *(PI / 180)));
			player2PositionY -= 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		// 한 방향 키

		// F key
		if ((GetAsyncKeyState(0x46) & 0x8000)){
			player2PositionX -= 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		// H key
		if ((GetAsyncKeyState(0x48) & 0x8000)){
			player2PositionX += 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		// T key
		if (GetAsyncKeyState(0x54) & 0x8000){
			player2PositionY -= 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			break;
		}

		// G key
		if (GetAsyncKeyState(0x47) & 0x8000){
			player2PositionY += 50 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 < 90)
				speedAngle2 += 1;
			cout << speedAngle2 << endl;
			break;
		}

		if (GetAsyncKeyState(0x5A)){
			player2Missle[missleNumber2].Shoot = true;
			player2Missle[missleNumber2].calibrateMissileTrigger = true;
			player2Missle[missleNumber2].MissilePositionX = player2PositionX + 20;
			player2Missle[missleNumber2].MissilePositionY = player2PositionY + 10;
			player2Missle[missleNumber2].missileAngle = 0;

			//   cout <<missleNumber2 <<boolalpha<<player2Missle[missleNumber2].Shoot << endl;
			if (missleNumber2 < 9)
				missleNumber2++;
			else
				missleNumber2 = 0;

			player2Missle[missleNumber2].efftectPositionX = player2Missle[missleNumber2].MissilePositionX - 50;
			player2Missle[missleNumber2].efftectPositionX = player2Missle[missleNumber2].MissilePositionY;

			player2Missle[missleNumber2].firePositionX = player2Missle[missleNumber2].MissilePositionX + 40;
			player2Missle[missleNumber2].firePositionY = player2Missle[missleNumber2].MissilePositionY;

			break;
		}

		if (GetAsyncKeyState(0x58)){
			player2Missle[newMissleNumber2].Shoot2 = true;
			player2Missle[newMissleNumber2].calibrateMissileTrigger2 = true;
			player2Missle[newMissleNumber2].MissilePositionX2 = player2PositionX + 20;
			player2Missle[newMissleNumber2].MissilePositionY2 = player2PositionY + 10;
			player2Missle[newMissleNumber2].missileAngle2 = 0;

			//   cout <<missleNumber2 <<boolalpha<<player2Missle[missleNumber2].Shoot << endl;
			if (newMissleNumber2 < 9)
				newMissleNumber2++;
			else
				newMissleNumber2 = 0;

			player2Missle[newMissleNumber2].efftectPositionX2 = player2Missle[newMissleNumber2].MissilePositionX2 - 50;
			player2Missle[newMissleNumber2].efftectPositionX = player2Missle[newMissleNumber2].MissilePositionY2;

			player2Missle[newMissleNumber2].firePositionX2 = player2Missle[newMissleNumber2].MissilePositionX2 + 40;
			player2Missle[newMissleNumber2].firePositionY2 = player2Missle[newMissleNumber2].MissilePositionY2;

			break;
		}


		if (VK_ESCAPE == wParam)
			exit(1);


		if (VK_RIGHT == wParam){


			break;
		}
		if (VK_LEFT == wParam){


			break;
		}
		if (VK_UP == wParam){


			break;
		}
		if (VK_DOWN == wParam){


			break;
		}
		if (VK_DELETE == wParam){
		}
		if (VK_SPACE == wParam){

		}

		InvalidateRect(hWnd, NULL, false);
		break;



	case WM_CHAR:


		InvalidateRect(hWnd, NULL, false);
		break;

	case WM_PAINT:


		hdc = BeginPaint(hWnd, &ps);
		mem1dc = CreateCompatibleDC(hdc);
		// hBit1에는 배경과 텍스트가 출력된 비트맵이 저장, mem1dc에 설정
		oldBit1 = (HBITMAP)SelectObject(mem1dc, hBit1);
		// mem1dc에 있는 내용을 hdc에 뿌려준다.
		//TransparentBlt(hdc, 0, 0, 100, 100, mem1dc, 0, 0, 100, 100, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, mem1dc, 0, 0, SRCCOPY);
		SelectObject(mem1dc, oldBit1);
		DeleteDC(mem1dc);
		DeleteObject(oldBit1);
		EndPaint(hWnd, &ps);
		break;


		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		Release();
		DeleteObject(MyPen);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); // 나머지는 OS로
}


void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{

	RECT rect;
	GetClientRect(hWnd, &rect);

	// player 1 Missile Animation
	for (int i = 0; i < 10; i++)
	{
		if (player1Missle[i].Shoot == true){

			if (player1Missle[i].MissileAnimationX <= 470){
				player1Missle[i].MissileAnimationX += 75;

			}
			else
				player1Missle[i].MissileAnimationX = 100;

			// 미사일 이동 로직
			if (player1Missle[i].calibrateMissileTrigger == true){
				player1Missle[i].MissilePositionX += 4 * sin((player1Missle[i].calibrateLineMissileAngleX *(PI / 180)));

				if (player1Missle[i].calibrateLineMissileAngleX > 0)
					player1Missle[i].calibrateLineMissileAngleX -= 5;
				else{

					player1Missle[i].calibrateLineMissileAngleX = 90;
				}

				player1Missle[i].MissilePositionY += 7 * sin((player1Missle[i].calibrateLineMissileAngleY *(PI / 180)));
				if (player1Missle[i].calibrateLineMissileAngleY > 0)
					player1Missle[i].calibrateLineMissileAngleY -= 5;
				else{
					player1Missle[i].calibrateMissileTrigger = false;
					player1Missle[i].calibrateLineMissileAngleY = 90;
				}
			}
			else{
				player1Missle[i].MissilePositionX -= 100 * sin((player1Missle[i].missileAngle *(PI / 180)));
				if (player1Missle[i].missileAngle < 90)
					player1Missle[i].missileAngle += 0.2;
			}



			player1Missle[i].firePositionX = player1Missle[i].MissilePositionX + 40;
			player1Missle[i].firePositionY = player1Missle[i].MissilePositionY + 5;

		}

		//미사일 2


		if (player1Missle[i].Shoot2 == true){

			if (player1Missle[i].MissileAnimationX2 <= 400){
				player1Missle[i].MissileAnimationX2 += 140;

			}
			else
				player1Missle[i].MissileAnimationX2 = 5;

			// 미사일 이동 로직
			if (player1Missle[i].calibrateMissileTrigger2 == true){
				player1Missle[i].MissilePositionX2 -= 4 * sin((player1Missle[i].calibrateLineMissileAngleX2 *(PI / 180)));

				if (player1Missle[i].calibrateLineMissileAngleX2 > 0)
					player1Missle[i].calibrateLineMissileAngleX2 -= 5;
				else{

					player1Missle[i].calibrateLineMissileAngleX2 = 90;
				}

				player1Missle[i].MissilePositionY2 += 7 * sin((player1Missle[i].calibrateLineMissileAngleY2 *(PI / 180)));
				if (player1Missle[i].calibrateLineMissileAngleY2 > 0)
					player1Missle[i].calibrateLineMissileAngleY2 -= 5;
				else{
					player1Missle[i].calibrateMissileTrigger2 = false;
					player1Missle[i].calibrateLineMissileAngleY2 = 90;
				}
			}
			else{
				player1Missle[i].MissilePositionX2 -= 100 * sin((player1Missle[i].missileAngle2 *(PI / 180)));

				if (player1Missle[i].missileAngle2 >= 5)
					player1Missle[i].MissilePositionY2 -= 50 * sin(((player1Missle[i].missileAngle2 - 0.1) *(PI / 180)));


				if (player1Missle[i].missileAngle2 < 90)
					player1Missle[i].missileAngle2 += 0.2;
			}


			player1Missle[i].firePositionX2 = player1Missle[i].MissilePositionX2 + 105;
			player1Missle[i].firePositionY2 = player1Missle[i].MissilePositionY2 + 1;
		}

	}

	// player2 Missle Animation
	for (int i = 0; i < 10; i++)
	{
		// 미사일 1

		if (player2Missle[i].Shoot == true){

			if (player2Missle[i].MissileAnimationX <= 430){
				player2Missle[i].MissileAnimationX += 75;

			}
			else
				player2Missle[i].MissileAnimationX = 0;

			// 미사일 이동 로직
			if (player2Missle[i].calibrateMissileTrigger == true){
				player2Missle[i].MissilePositionX -= 4 * sin((player2Missle[i].calibrateLineMissileAngleX *(PI / 180)));

				if (player2Missle[i].calibrateLineMissileAngleX > 0)
					player2Missle[i].calibrateLineMissileAngleX -= 5;
				else{

					player2Missle[i].calibrateLineMissileAngleX = 90;
				}

				player2Missle[i].MissilePositionY += 7 * sin((player2Missle[i].calibrateLineMissileAngleY *(PI / 180)));
				if (player2Missle[i].calibrateLineMissileAngleY > 0)
					player2Missle[i].calibrateLineMissileAngleY -= 5;
				else{
					player2Missle[i].calibrateMissileTrigger = false;
					player2Missle[i].calibrateLineMissileAngleY = 90;
				}
			}
			else{
				player2Missle[i].MissilePositionX += 100 * sin((player2Missle[i].missileAngle *(PI / 180)));
				if (player2Missle[i].missileAngle < 90)
					player2Missle[i].missileAngle += 0.2;
			}


			player2Missle[i].firePositionX = player2Missle[i].MissilePositionX - 20;
			player2Missle[i].firePositionY = player2Missle[i].MissilePositionY + 5;
		}

		//미사일 2


		if (player2Missle[i].Shoot2 == true){

			if (player2Missle[i].MissileAnimationX2 <= 400){
				player2Missle[i].MissileAnimationX2 += 140;

			}
			else
				player2Missle[i].MissileAnimationX2 = 10;

			// 미사일 이동 로직
			if (player2Missle[i].calibrateMissileTrigger2 == true){
				player2Missle[i].MissilePositionX2 -= 4 * sin((player2Missle[i].calibrateLineMissileAngleX2 *(PI / 180)));

				if (player2Missle[i].calibrateLineMissileAngleX2 > 0)
					player2Missle[i].calibrateLineMissileAngleX2 -= 5;
				else{

					player2Missle[i].calibrateLineMissileAngleX2 = 90;
				}

				player2Missle[i].MissilePositionY2 += 7 * sin((player2Missle[i].calibrateLineMissileAngleY2 *(PI / 180)));
				if (player2Missle[i].calibrateLineMissileAngleY2 > 0)
					player2Missle[i].calibrateLineMissileAngleY2 -= 5;
				else{
					player2Missle[i].calibrateMissileTrigger2 = false;
					player2Missle[i].calibrateLineMissileAngleY2 = 90;
				}
			}
			else{
				player2Missle[i].MissilePositionX2 += 100 * sin((player2Missle[i].missileAngle2 *(PI / 180)));

				if (player2Missle[i].missileAngle2 >= 5)
					player2Missle[i].MissilePositionY2 -= 50 * sin(((player2Missle[i].missileAngle2 - 0.1) *(PI / 180)));


				if (player2Missle[i].missileAngle2 < 90)
					player2Missle[i].missileAngle2 += 0.2;
			}


			player2Missle[i].firePositionX2 = player2Missle[i].MissilePositionX2 - 20;
			player2Missle[i].firePositionY2 = player2Missle[i].MissilePositionY2 + 5;
		}
	}



	if (afterimageMoveTrigger > 0){

		switch (afterimageMoveTrigger)
		{
		case UP:

			player1PositionY -= 20 * sin((speedAngle *(PI / 180)));
			if (speedAngle >= 0)
				speedAngle -= 0.5;
			else
				afterimageMoveTrigger = 0;
			break;

		case DOWN:
			player1PositionY += 20 * sin((speedAngle *(PI / 180)));
			if (speedAngle >= 0)
				speedAngle -= 0.5;
			else
				afterimageMoveTrigger = 0;
			break;

		case LEFT:
			player1PositionX -= 20 * sin((speedAngle *(PI / 180)));
			if (speedAngle >= 0)
				speedAngle -= 0.5;
			else
				afterimageMoveTrigger = 0;
			break;

			break;
		case RIGHT:
			player1PositionX += 20 * sin((speedAngle *(PI / 180)));
			if (speedAngle >= 0)
				speedAngle -= 0.5;
			else
				afterimageMoveTrigger = 0;
			break;

		default:
			break;
		}
	}

	if (afterimageMoveTrigger2 > 0){
		switch (afterimageMoveTrigger2)
		{
		case UP:
			player2PositionY -= 20 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 >= 0)
				speedAngle2 -= 0.5;
			else
				afterimageMoveTrigger2 = 0;
			break;

		case DOWN:
			player2PositionY += 20 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 >= 0)
				speedAngle2 -= 0.5;
			else
				afterimageMoveTrigger2 = 0;
			break;

		case LEFT:
			player2PositionX -= 20 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 >= 0)
				speedAngle2 -= 0.5;
			else
				afterimageMoveTrigger2 = 0;
			break;

			break;
		case RIGHT:
			player2PositionX += 20 * sin((speedAngle2 *(PI / 180)));
			if (speedAngle2 >= 0)
				speedAngle2 -= 0.5;
			else
				afterimageMoveTrigger2 = 0;
			break;

		default:
			break;
		}

	}



	hdc = GetDC(hWnd);

	if (hBit1 == NULL) // hBit1을 hdc와 호환되게 만들어준다.
		hBit1 = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	// hdc에서 mem1dc를 호환되도록 만들어준다.
	mem1dc = CreateCompatibleDC(hdc);
	// mem1dc에서 mem2dc를 호환이 되도록 만들어준다.
	mem2dc = CreateCompatibleDC(mem1dc);
	mem3dc = CreateCompatibleDC(mem1dc);

	mem5dc = CreateCompatibleDC(mem1dc);
	mem6dc = CreateCompatibleDC(mem1dc);
	missleMem1dc = CreateCompatibleDC(mem1dc);
	missleMem2dc = CreateCompatibleDC(mem1dc);
	
	// player 1 missle 

		
		oldBit1 = (HBITMAP)SelectObject(missleMem1dc, hPlayer1MissleBit);
	

	// player 2 missle 
	
		
	oldBit1 = (HBITMAP)SelectObject(missleMem2dc, hPlayer2MissleBit);

	//Obstacle

		
	oldBit1 = (HBITMAP)SelectObject(mem6dc, hObstacleBit); 
	

	/// 미사일과 캐릭터 충돌체크

	/// 1 Player미사일과 2Player 충돌체크
	for (int i = 0; i < 10; ++i)
	{
		if (sqrt((player1Missle[i].MissilePositionX - player2PositionX) * (player1Missle[i].MissilePositionX - player2PositionX) +
			(player1Missle[i].MissilePositionY - player2PositionY - 30) * (player1Missle[i].MissilePositionY - player2PositionY - 30)) < CheckRange)
		{
			printf("Crash!!!\n");
			player1Missle[i].Shoot = FALSE;
			player2PositionX -= 5;
			player2HP -= 1;
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Effect1], 0, &g_Channel[SD_Background]);

		}
		if (sqrt((player1Missle[i].MissilePositionX2 - player2PositionX) * (player1Missle[i].MissilePositionX2 - player2PositionX) +
			(player1Missle[i].MissilePositionY2 - player2PositionY - 30) * (player1Missle[i].MissilePositionY2 - player2PositionY - 30)) < CheckRange)
		{
			printf("Crash!!!\n");
			player1Missle[i].Shoot2 = FALSE;
			player2PositionX -= 5;
			player2HP -= 1;
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Effect1], 0, &g_Channel[SD_Background]);

		}

		if (sqrt((player2Missle[i].MissilePositionX - player1PositionX) * (player2Missle[i].MissilePositionX - player1PositionX) +
			(player2Missle[i].MissilePositionY - player1PositionY - 30) * (player2Missle[i].MissilePositionY - player1PositionY - 30)) < CheckRange)
		{
			printf("Crash!!!\n");
			player2Missle[i].Shoot = FALSE;
			player1PositionX += 5;
			player1HP -= 1;
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Effect1], 0, &g_Channel[SD_Background]);

		}

		if (sqrt((player2Missle[i].MissilePositionX2 - player1PositionX) * (player2Missle[i].MissilePositionX2 - player1PositionX) +
			(player2Missle[i].MissilePositionY2 - player1PositionY - 30) * (player2Missle[i].MissilePositionY2 - player1PositionY - 30)) < CheckRange)
		{
			printf("Crash!!!\n");
			player2Missle[i].Shoot2 = FALSE;
			player1PositionX += 5;
			player1HP -= 1;
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Effect1], 0, &g_Channel[SD_Background]);

		}
	}
	
	///캐릭터와 장애물 충돌체크
	
	///player1

	//장애물 위와 충돌체크 
	
	if (player1PositionX + 100 >= Obstacle[0].x  && player1PositionX <= Obstacle[0].x + 100 && player1PositionY + 100 >= Obstacle[0].y)
	{
		player1PositionY -= 20;
	}
	//아래랑은 할 필요 없음

	if (player1PositionX > Obstacle[0].x + 100)
	{

		//왼쪽 오른쪽과 충돌체크
		if (player1PositionY + 100 >= Obstacle[0].y && player1PositionY <= Obstacle[0].y + 500 && (player1PositionX <= Obstacle[0].x + 100))
		{
			player1PositionX += 20;
		}
	}
	else if (player1PositionX + 100 < Obstacle[0].x)
	{
		if (player1PositionY + 100 >= Obstacle[0].y && player1PositionY <= Obstacle[0].y + 500 && player1PositionX + 100 >= Obstacle[0].x)
		{
			player1PositionX -= 20;
		}
	}

	if (player1PositionX + 100 >= Obstacle[1].x  && player1PositionX <= Obstacle[1].x + 100 && player1PositionY + 100 >= Obstacle[1].y)
	{
		player1PositionY -= 20;
	}
	//아래랑은 할 필요 없음

	if (player1PositionX > Obstacle[1].x + 100)
	{
		//왼쪽 오른쪽과 충돌체크
		if (player1PositionY + 100 >= Obstacle[1].y && player1PositionY <= Obstacle[1].y + 250 && (player1PositionX <= Obstacle[1].x + 100))
		{
			player1PositionX += 20;
		}
	}
	else if (player1PositionX + 100 < Obstacle[1].x)
	{
		if (player1PositionY + 100 >= Obstacle[1].y && player1PositionY <= Obstacle[1].y + 250 && player1PositionX + 100 >= Obstacle[1].x)
		{
			player1PositionX -= 20;
		}
	}

	///player2
	if (player2PositionX + 100 >= Obstacle[0].x  && player2PositionX <= Obstacle[0].x + 100 && player2PositionY + 100 >= Obstacle[0].y)
	{
		player2PositionY -= 20;
	}

	if (player2PositionX > Obstacle[0].x + 100 && player2PositionY)
	{

		//왼쪽 오른쪽과 충돌체크
		if (player2PositionY + 100 >= Obstacle[0].y && player2PositionY <= Obstacle[0].y + 500 && (player2PositionX <= Obstacle[0].x + 100))
		{
			printf("!!!\n");
			player2PositionX += 20;

		}
	}
	else if (player2PositionX + 100 < Obstacle[0].x)
	{
		if (player2PositionY + 100 >= Obstacle[0].y && player2PositionY <= Obstacle[0].y + 500 && player2PositionX + 100 >= Obstacle[0].x)
		{

			player2PositionX -= 20;

		}

	}
	if (player2PositionX + 100 >= Obstacle[1].x  && player2PositionX <= Obstacle[1].x + 100 && player2PositionY + 100 >= Obstacle[1].y)
	{
		player2PositionY -= 20;
	}

	if (player2PositionX > Obstacle[1].x + 100 && player2PositionY)
	{

		//왼쪽 오른쪽과 충돌체크
		if (player2PositionY + 100 >= Obstacle[1].y && player2PositionY <= Obstacle[1].y + 250 && (player2PositionX <= Obstacle[1].x + 100))
		{
			printf("!!!\n");
			player2PositionX += 20;

		}
	}
	else if (player2PositionX + 100 < Obstacle[1].x)
	{
		if (player2PositionY + 100 >= Obstacle[1].y && player2PositionY <= Obstacle[1].y + 250 && player2PositionX + 100 >= Obstacle[1].x)
		{
			player2PositionX -= 20;
		}
	}
	// mem2dc의 비트맵을 mem1dc에 옮기고, mem1dc를 hdc로 옮기려고 함
	oldBit1 = (HBITMAP)SelectObject(mem1dc, hBit1); // mem1dc에는 hBit1
	oldBit2 = (HBITMAP)SelectObject(mem2dc, hPlayer1Bit); // mem2dc에는 hBit2
	oldBit2 = (HBITMAP)SelectObject(mem3dc, hPlayer2Bit); // mem2dc에는 hBit2

	oldBit1 = (HBITMAP)SelectObject(mem5dc, hBackgroundBit); // mem2dc에는 hBit2
	oldBit1 = (HBITMAP)SelectObject(mem6dc, Obstacle);

	// Background
	StretchBlt(mem1dc, 0, 0, rect.right, rect.bottom, mem5dc, 0, 0, 800, 600, SRCCOPY);

	///Obstacle
	TransparentBlt(mem1dc, Obstacle[0].x, Obstacle[0].y, 100, 500, mem6dc,
		0, 0, 100, 250, RGB(255, 255, 255));
	TransparentBlt(mem1dc, Obstacle[1].x, Obstacle[1].y, 100, 250, mem6dc,
		0, 0, 100, 250, RGB(255, 255, 255));

	if (player1HP <= 0 )
	{
		player1PositionY += 5;

	}
	if (player2HP <= 0)
	{
		player2PositionY += 5;
	}

	// player 1
	TransparentBlt(mem1dc, player1PositionX, player1PositionY, 100, 100, mem2dc,
		player1AnimationX, 0, 180, 250, RGB(255, 128, 128));

	// player 2
	TransparentBlt(mem1dc, player2PositionX, player2PositionY, 100, 100, mem3dc,
		player2AnimationX, 0, 225, 250, RGB(255, 255, 255));
	/// Obstacle

	// player 1 missle 
	for (int i = 0; i < 10; i++)
	{
		if (player1Missle[i].Shoot == true){
			TransparentBlt(mem1dc, player1Missle[i].MissilePositionX, player1Missle[i].MissilePositionY, 50, 50, missleMem1dc,
				player1Missle[i].MissileAnimationX, player1Missle[i].MissileAnimationY, 75, 50, RGB(63, 143, 255));
			TransparentBlt(mem1dc, player1Missle[i].firePositionX, player1Missle[i].firePositionY, 30, 30, missleMem1dc,
				player1Missle[i].fireAnimationX, player1Missle[i].fireAnimationY, 30, 25, RGB(63, 143, 255));
		}
		if (player1Missle[i].Shoot2 == true){

			TransparentBlt(mem1dc, player1Missle[i].MissilePositionX2, player1Missle[i].MissilePositionY2, 120, 50, missleMem1dc,
				player1Missle[i].MissileAnimationX2, player1Missle[i].MissileAnimationY2, 130, 50, RGB(63, 143, 255));
			TransparentBlt(mem1dc, player1Missle[i].firePositionX2, player1Missle[i].firePositionY2, 30, 30, missleMem1dc,
				player1Missle[i].fireAnimationX2, player1Missle[i].fireAnimationY2, 30, 25, RGB(63, 143, 255));
		}


	}

	// player 2 missle 

	for (int i = 0; i < 10; i++)
	{

		if (player2Missle[i].Shoot == true){

			/*TransparentBlt(mem1dc, 30,30, 50, 50, missleMem2dc[i],
			player2Missle[i].MissileAnimationX, player2Missle[i].MissileAnimationY, 75, 50, RGB(63, 143, 255));
			*/
			cout << player2Missle[i].MissilePositionX << endl;
			TransparentBlt(mem1dc, player2Missle[i].MissilePositionX, player2Missle[i].MissilePositionY, 50, 50, missleMem2dc,
				player2Missle[i].MissileAnimationX, player2Missle[i].MissileAnimationY, 75, 50, RGB(63, 143, 255));
			TransparentBlt(mem1dc, player2Missle[i].firePositionX, player2Missle[i].firePositionY, 30, 30, missleMem2dc,
				player2Missle[i].fireAnimationX, player2Missle[i].fireAnimationY, 30, 25, RGB(63, 143, 255));
		}
		if (player2Missle[i].Shoot2 == true){

			TransparentBlt(mem1dc, player2Missle[i].MissilePositionX2, player2Missle[i].MissilePositionY2, 120, 50, missleMem2dc,
				player2Missle[i].MissileAnimationX2, player2Missle[i].MissileAnimationY2, 130, 50, RGB(63, 143, 255));
			TransparentBlt(mem1dc, player2Missle[i].firePositionX2, player2Missle[i].firePositionY2, 30, 30, missleMem2dc,
				player2Missle[i].fireAnimationX2, player2Missle[i].fireAnimationY2, 30, 25, RGB(63, 143, 255));
		}
	}

	

	SetBkMode(mem1dc, TRANSPARENT);
	// 저장한 비트맵 핸들값을 DC에 원상복귀, 생성된 MDC 삭제
	/*SelectObject(mem5dc, oldBit2);
	SelectObject(mem4dc, oldBit2);
	SelectObject(mem3dc, oldBit1);
	SelectObject(mem2dc, oldBit2);*/
	SelectObject(mem1dc, oldBit1);

	//for (int i = 0; i < 10; i++)
	//{
	//	DeleteDC(missleMem1dc[i]);

	//}
	DeleteDC(missleMem1dc);
	
	DeleteDC(missleMem2dc);
	
	DeleteDC(mem1dc);
	DeleteDC(mem2dc);
	DeleteDC(mem3dc);

	DeleteDC(mem5dc);
	DeleteDC(mem6dc);
	
	InvalidateRgn(hWnd, NULL, false);
	ReleaseDC(hWnd, hdc);
}

void CALLBACK TimerProc1(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{

	// player 캐릭터 애니메이션
	if (player1AnimationX < 360)
		player1AnimationX += 183;
	else
		player1AnimationX = 0;

	if (player2AnimationX < 675)
		player2AnimationX += 225;
	else
		player2AnimationX = 0;


	// player 1 Missile Animation
	for (int i = 0; i < 10; i++)
	{
		if (player1Missle[i].Shoot == true){

			if (player1Missle[i].missileAngle >= 5 && player1Missle[i].missileAngle <= 10)
				player1Missle[i].fireAnimationX = 330;
			if (player1Missle[i].missileAngle >= 15 && player1Missle[i].missileAngle <= 20)
				player1Missle[i].fireAnimationX = 295;
			if (player1Missle[i].missileAngle >= 20)
				player1Missle[i].fireAnimationX = 260;

			/*   if (player1Missle[i].fireAnimationX >= 294)
			player1Missle[i].fireAnimationX -= 35;
			else
			player1Missle[i].fireAnimationX = 365;*/

		}
	}

	// player 2 Missile Animation
	for (int i = 0; i < 10; i++)
	{
		if (player2Missle[i].Shoot == true){

			if (player2Missle[i].missileAngle >= 5 && player2Missle[i].missileAngle <= 10)
				player2Missle[i].fireAnimationX = 165;
			if (player2Missle[i].missileAngle >= 15 && player2Missle[i].missileAngle <= 20)
				player2Missle[i].fireAnimationX = 200;
			if (player2Missle[i].missileAngle >= 20)
				player2Missle[i].fireAnimationX = 235;

			/*   if (player1Missle[i].fireAnimationX >= 294)
			player1Missle[i].fireAnimationX -= 35;
			else
			player1Missle[i].fireAnimationX = 365;*/

		}
	}


	// player 1 Missile Animation
	for (int i = 0; i < 10; i++)
	{
		if (player1Missle[i].Shoot2 == true){

			if (player1Missle[i].missileAngle2 >= 5 && player1Missle[i].missileAngle2 <= 10)
				player1Missle[i].fireAnimationX = 330;
			if (player1Missle[i].missileAngle2 >= 15 && player1Missle[i].missileAngle2 <= 20)
				player1Missle[i].fireAnimationX2 = 295;
			if (player1Missle[i].missileAngle2 >= 20)
				player1Missle[i].fireAnimationX2 = 260;

			/*   if (player1Missle[i].fireAnimationX >= 294)
			player1Missle[i].fireAnimationX -= 35;
			else
			player1Missle[i].fireAnimationX = 365;*/

		}
	}

	// player 2 Missile Animation
	for (int i = 0; i < 10; i++)
	{
		if (player2Missle[i].Shoot2 == true){

			if (player2Missle[i].missileAngle2 >= 5 && player2Missle[i].missileAngle2 <= 10)
				player2Missle[i].fireAnimationX2 = 165;
			if (player2Missle[i].missileAngle2 >= 15 && player2Missle[i].missileAngle2 <= 20)
				player2Missle[i].fireAnimationX2 = 200;
			if (player2Missle[i].missileAngle2 >= 20)
				player2Missle[i].fireAnimationX2 = 235;

			/*   if (player1Missle[i].fireAnimationX >= 294)
			player1Missle[i].fireAnimationX -= 35;
			else
			player1Missle[i].fireAnimationX = 365;*/

		}
	}


}