#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <ctype.h>


/*확장 아스키 사용 특수키의 경우는 2BYTE의 크기를 가진다. getch함수를 두번 호출하여 그 값을 1byte씩 읽어들임
처음에 하위 1byte를 읽고 두번째 상위 1byte를 읽는다. 2byte 크기의 아스키 코드값의 구조는 하위 1byte이 코드값이 0이다.*/
#define LEFT 75 //방향키 왼쪽 0 75 
#define RIGHT 77 //방향키 오른쪽 0 77
#define UP 72 //방향키 위 0 72
#define DOWN 80 //방향키 아래 0 80
#define ESC 27 //esc 0 27

//
#define BX 5  
#define BY 1 // BX,BY는 매크로 게임판의 좌상단 좌표이다.
#define BW 10 
#define BH 20 // BW,BH는 각각 게임판의 폭과 높이 값이다.(외부 벽 제외 값)
// 위에 4줄을 통하여 매크로 게임판은 5,1에서 시작하여 10의 폭과 20의 높이를 가진다.

#define clrscr() system("cls") //화면을 지우는 함수를 쉽게 쓰기 위하여 system("cls");함수를 clrscr();함수로 치환하였다.
#define randomize() srand((unsigned int)time(NULL))// 난수의 시드 값을 바꾸는 과정을 간단히 하기 위하여 srand((unsigned int)time(NULL))을 randomize()로 치환시켰다.
#define random(min, max) ( (rand() % ((max) - (min) + 1) ) + (min)) //범위 지정을 통한 난수 발생을 위한 함수


void DrawScreen(); // 화면 전체를 그리는 함수
void DrawBoard(); //벽돌을 그리는 함수
BOOL ProcessKey(); //키 입력을 처리하며 벽돌이 바닥에 닿았을 때 TRUE반환
void PrintBrick(BOOL Show);// 이동중인 벽돌을 삭제, 출력하는 함수 전역변수 brick,rot,nx,ny를 참조
int GetAround(int x, int y, int b, int r); // 벽돌 주변을 검사하여 벽돌이 이동 및 회전가능성을 조사하는 함수
BOOL MoveDown();  //벽돌을 한칸 아래로 이동시킨다. 벽돌이 바닥에 닿는다면 TESTFULL함수 호출 후 TRUE를 리턴
void TestFull(); //수평을 다 채워진 줄을 찾아 삭제
void setcursortype(CURSORTYPE_TYPE);// 커서의 형태를 지정해 주는 함수 이 프로그램에서는 커서를 안보이게 한다.
void gotoxy(int x, int y); // 커서의  위치 이동을 위한 함수
void delay(int x); //Sleep함수와 똑같고 가시성을 위하여 정의, 프로그램의 진행을 지연시키는 함수


typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
typedef struct point {
	int x, y;
}Point;//
Point Shape[7][4][4] = {
   {  { {0,0},{1,0},{2,0},{-1,0}   }, { {0,0},{0,1},{0,-1},{0,-2}  }, { {0,0},{1,0},{2,0},{-1,0}   }, { {0,0},{0,1},{0,-1},{0,-2}  } },
   /*[0][0][0] ~ [0][0][3]         [0][1][0] ~ [0][1][3]            [0][2][0] ~ [0][2][3]         [0][3][0] ~ [0][3][3]      */
  {  { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1}, {1,1}   } },
  /*[1][0][0] ~ [1][0][3]         [1][1][0] ~ [1][1][3]            [1][2][0] ~ [1][2][3]         [1][3][0] ~ [1][3][3]      */
{  { {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} }, { {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} } },
/*[2][0][0]   ~ [2][0][3]         [2][1][0] ~ [2][1][3]         [2][2][0] ~ [2][2][3]            [2][3][0] ~ [2][3][3]      */
{  { {0,0},{-1,-1},{0,-1},{1,0}  },{ {0,0},{-1,0},{-1,1},{0,-1} }, { {0,0},{-1,-1},{0,-1},{1,0} }, { {0,0},{-1,0},{-1,1},{0,-1} } },
/*[3][0][0] ~ [3][0][3]         [3][1][0] ~ [3][1][3]         [3][2][0] ~ [3][2][3]            [3][3][0] ~ [3][3][3]      */
{  { {0,0},{-1,0},{1,0},{-1,-1} }, { {0,0},{0,-1},{0,1},{-1,1}  }, { {0,0},{-1,0},{1,0},{1,1}   }, { {0,0},{0,-1},{0,1},{1,-1}  } },
/*[4][0][0] ~ [4][0][3]         [4][1][0] ~ [4][1][3]         [4][2][0] ~ [4][2][3]            [4][3][0] ~ [4][3][3]      */
{  { {0,0},{1,0},{-1,0},{1,-1}  }, { {0,0},{0,1},{0,-1},{-1,-1} }, { {0,0},{1,0},{-1,0},{-1,1}  }, { {0,0},{0,-1},{0,1},{1,1}   } },
/*[5][0][1] ~ [5][0][3]         [5][1][0] ~ [5][1][3]         [5][2][0] ~ [5][2][3]            [5][3][0] ~ [5][3][3]      */
{  { {0,0},{-1,0},{1,0},{0,1}   }, { {0,0},{0,-1},{0,1},{1,0}   }, { {0,0},{-1,0},{1,0},{0,-1}  }, { {0,0},{-1,0},{0,-1},{0,1}  } },
/*[6][0][1] ~ [6][0][3]         [6][1][0] ~ [6][1][3]         [6][2][0] ~ [6][2][3]            [6][3][0] ~ [6][3][3]      */
}; //블럭의 모양 구조체 배열

enum { EMPTY, BRICK, WALL }; // 각각 0,1,2로 게임판의 상태를 표시, arTile배열을 쉽게 사용하기 위한 열거형 
char* arTile[] = { ". ","■","□" }; // 공백과 벽, 벽돌을 그리기 위해 필요한 문자형 포인터 변수
int board[BW + 2][BH + 2]; // 전체 게임판의 상태에 쓰이는 배열로써 BW,BH는 각각 외부 벽을 제외한 값이라 +2를 해주어 외부벽 좌표까지 넓힘
int nx, ny; // 이동중인 벽돌의 현재좌표 
int brick, rot; // 이동중인 벽돌의 식별번호이자 회전값

int main()
{

	int nFrame, nStay; //Frame(속도), Stay(타이머)
	int x, y;

	setcursortype(NOCURSOR); // 커서 지우기
	randomize(); // 난수 시드 값 할당
	clrscr();// 화면 지우기
	for (x = 0; x < BW + 2; x++) { //가로의 사이즈
		for (y = 0; y < BH + 2; y++) { //세로의 사이즈
			board[x][y] = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY; //맨아래 ll 맨위 ll 왼쪽 끝 ll 오른쪽 끝 ll 벽 : 빈공간
		}
	}
	DrawScreen(); //화면 전체를 그리는 함수, 게임판과 게임이름, 벽까지 한번에 그린다.
	nFrame = 20; //프레임 셋팅

	for (; 1;) { //1단계를 실행함
		brick = random(0, 6); // 블럭모양 배열에서 랜덤으로 추출
		nx = BW / 2; // x좌표, 보드의 가운데(중간)
		ny = 3; //y 좌표
		rot = 0; //회전을 안한다.
		PrintBrick(TRUE); //블럭을 출력(TURE)

		if (GetAround(nx, ny, brick, rot) != EMPTY) break; //블럭의 주변을 검사한다.
		nStay = nFrame; //타이머 재설정
		for (; 2;) { // 2단계 실행
			if (--nStay == 0) { //타이머 설정을 종료함.
				nStay = nFrame; //타이머 설정을 다시한다.
				if (MoveDown()) break; //바닥으로 내린다.
			}
			if (ProcessKey()) break; //일반 키
			delay(1000 / 20); //화면의 재생률을 지연한다.
		}
	} //게임종료(즉, Loop문 종료함.)
	clrscr(); //화면 클리어
	gotoxy(30, 12); puts("G A M E  O V E R"); //30. 12에 문자열 출력
	setcursortype(NORMALCURSOR); //커서 복구
	delay(5); //delay설정


	return 0;
}

void setcursortype(CURSORTYPE_TYPE)
{

	CONSOLE_CURSOR_INFO CurInfo;

	switch (CURSORTYPE_TYPE) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void DrawScreen()
{
	int x, y;

	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			gotoxy(BX + x * 2, BY + y); //좌표 재설정
			puts(arTile[board[x][y]]);
		}
	}
	//각각의 x, y좌표에 문자열 출력
	gotoxy(50, 3); puts("권택준,황상천");
	gotoxy(50, 5); puts("좌우:이동, 위:회전, 아래:내림");
	gotoxy(50, 6); puts("스페이스바 : hard drop");
}

void DrawBoard() // 게임을 진행하는 구역을 그리는 함수
{
	int x, y;

	for (x = 1; x < BW + 1; x++) {
		for (y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[board[x][y]]);
		}
	}
}

BOOL ProcessKey() //Key입력 처리 함수
{
	int ch, trot;

	if (_kbhit()) {
		ch = _getch();
		if (ch == 0xE0 || ch == 0) {
			ch = _getch();
			switch (ch) {
			case LEFT: //왼쪽이 비어 있을 때.
				if (GetAround(nx - 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE); //현재 블럭을 없앰.
					nx--; //x좌표를 이동함.
					PrintBrick(TRUE); //다시 블럭을 나타냄.
				}
				break;
			case RIGHT: //오른쪽이 비어 있을 때
				if (GetAround(nx + 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx++;
					PrintBrick(TRUE);
				}
				break;
			case UP: //회전 3회시에 원위치한다.
				trot = (rot == 3 ? 0 : rot + 1);
				if (GetAround(nx, ny, brick, trot) == EMPTY) {
					PrintBrick(FALSE);
					rot = trot;
					PrintBrick(TRUE);
				}
				break;
			case DOWN: //내리기 기능
				if (MoveDown()) { //함수 호출
					return TRUE;
				}
				break;
			}
		}
		else {
			switch (ch) {
			case ' ': //SpaceBar기능
				while (MoveDown() == FALSE) { ; }
				return TRUE;
			}
		}
	}
	return FALSE;
}

void PrintBrick(BOOL Show) //블럭을 그려주는 함수
{
	int i;

	for (i = 0; i < 4; i++) { // 현재위치 + 블럭좌표 + 이동값을 계산한 결과 x, y좌표가 이동함.
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
		puts(arTile[Show ? BRICK : EMPTY]);
	} // show가 TRUE면 블럭을 아니면 빈공간.
}

int GetAround(int x, int y, int b, int r)
{
	int i, k = EMPTY; //k=max(EMPTY, 보드[b블럭, r회전, i의 x, y좌표]
		  //0부터 4까지 조사함. 즉, 하나의 블럭 주변을 검사한다.
	for (i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown() //블럭을 내리는 기능의 함수
{
	if (GetAround(nx, ny + 1, brick, rot) != EMPTY) { //주위에 블럭이 있을 경우
		TestFull(); //블럭라인 검사 후 종료
		return TRUE;
	}
	PrintBrick(FALSE); //가린다.
	ny++; //ny(화면상의 좌표) 증가
	PrintBrick(TRUE); //보이게 한다.
	return FALSE;
}

void TestFull() //블럭 라인 검사 함수
{
	int i, x, y, ty;

	for (i = 0; i < 4; i++) { //MoveDown에서 호출된 상태의 블럭을 기록한다.
		board[nx + Shape[brick][rot][i].x][ny + Shape[brick][rot][i].y] = BRICK;
	}
	//쌓여있는 블럭을 검색한다.
	for (y = 1; y < BH + 1; y++) {
		for (x = 1; x < BW + 1; x++) {
			if (board[x][y] != BRICK) break; //블럭이 아니면 넘어감.
		}
		if (x == BW + 1) { //한줄이 다 블럭일 경우에 (for문의 특성이다.)
			for (ty = y; ty > 1; ty--) { //맨위의 라인부터 해당한다.
				for (x = 1; x < BW + 1; x++) {
					board[x][ty] = board[x][ty - 1];
				}
			}
			DrawBoard(); //게임 진행하는 위치(구역)을 다시 그려준다.
			delay(200); //지연되는 delay (200)
		}
	}
}

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}
void delay(int x) {
	Sleep(x);
}
