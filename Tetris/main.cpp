#include <windows.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

#include "Timer.h"
#include "pieces.h"

//Konstanter
const int SCREENH = 384;
const int SCREENW = 192;
const int LEVELROW = 24;
const int LEVELCOL = 12;
static LPCTSTR AppNamn = "Tetris";


//Spelplan
int currentLevel[LEVELROW][LEVELCOL];
int level[LEVELROW][LEVELCOL];/* = {
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							   { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
							   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
							   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
							   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
							   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
							   };
							   
*/

HWND window;
HDC device;
HDC displayDevice;
HANDLE hOld;
Pieces* p;
bool gameOver;
int score, lines = 0;
double timerPoint;

//Prototyper
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void DrawBitmap(char *filename, int x, int y);
void updateLogic();
void updateGraphic();
void updateWindow(int currentLevel[LEVELROW][LEVELCOL]);
void combineArrays();
void initGame();
void tryRotatePiece();
void movePiece();
bool isMovementAllowed(int piece[4][4]);
void drawBackText(LPCSTR text, int x, int y);
void updateScore(int scoreMultiplier);

void updatePieces(int posX, int posY, int board[4][4]);
void updateBackground(int currentLevel[LEVELROW][LEVELCOL]);

void lostGame(){
	gameOver = true;
}

//Skapa ny piece
void newPiece(){
	if (!gameOver){
		if (p != NULL){
			destroyPiece(p);
		}
		p = createPiece();
		p->direction = DOWN;
		if (!isMovementAllowed(p->thisPiece)){
			lostGame();
		}
	}
}

//Merge arrays
void combineArrays(){
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			if (p->thisPiece[i][j] > 0){
				currentLevel[p->posX + i][p->posY + j] = p->thisPiece[i][j];
			} else{
			}
		}
	}
}

//Check if rotation is possible
void tryRotatePiece(){
	if (isMovementAllowed(p->nextPiece)){
		rotatePiece(p);
	}
}

void updateScore(int scoreMultiplier){
	score += (100 * scoreMultiplier);
}


//Delete rows
void deleteRows(int filled[4]){
	//Locate the rows to be deleted, move them to array toDelete
	int scoreMultiplier = 1;
	int toDelete[LEVELROW][LEVELCOL];
	std::copy(&currentLevel[0][0], &currentLevel[0][0] + LEVELCOL * LEVELROW, &toDelete[0][0]);
	for (int i = 0; i < 4; i++){
		if (filled[i] > 0){
			for (int j = 0; j < LEVELCOL; j++){
				toDelete[filled[i]][j] = 9;
			}
		}
	}
	//Flash effect by freezing
	for (int i = 0; i < 5; i++){
		updateWindow(currentLevel);
		Sleep(50);
		updateWindow(toDelete);
		Sleep(50);
	}

	//Actual removal in the new array
	for (int i = LEVELROW; i > 0; i--){
		if (toDelete[i][0] == 9){
			for (int j = i; j > 0; j--){
				for (int k = 0; k < LEVELCOL; k++){
					toDelete[j][k] = toDelete[j - 1][k];
				}
			}
			//Move back one step on delete
			i++;
			lines++;
			scoreMultiplier *= 4;
		}
	}
	//Update main array
	std::copy(&toDelete[0][0], &toDelete[0][0] + LEVELCOL * LEVELROW, &currentLevel[0][0]);
	updateScore(scoreMultiplier);
	updateWindow(currentLevel);
}

//Check for complete rows
void checkRows(){
	int filled[] = { -1, -1, -1, -1 };
	bool completeRow;
	bool aRowIsFilled = false;
	int j, filledRow = 0;
	for (int i = 0; i < LEVELROW; i++){
		completeRow = true;
		j = 0;
		while (completeRow){
			if (currentLevel[i][j] == 0){
				completeRow = false;
			} else{
				j++;
			}
			if (j == LEVELCOL){
				break;
			}
		}
		//Saved fill rows, max 4 as "floating rows" are allowed
		if (completeRow){
			filled[filledRow] = i;
			filledRow++;
			aRowIsFilled = true;
		}
	}
	if (aRowIsFilled){
		deleteRows(filled);
	}
}

//Movement of pieces
void movePiece(){
	if (isMovementAllowed(p->thisPiece)){
		switch (p->direction)
		{
		case UP:
			p->posX += -1;
			break;
		case DOWN:
			p->posX += 1;
			Sleep(20);
			break;
		case LEFT:
			p->posY += -1;
			break;
		case RIGHT:
			p->posY += 1;
			break;
		default:
			break;
		}
	} else{
		if (p->direction == DOWN){
			//Combine piece and board arrays
			combineArrays();
			//Check for complete rows
			checkRows();
			//New piece
			newPiece();
		}
	}

}

//Check if movement is possible depending on piece direction
bool isMovementAllowed(int piece[4][4]){
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			if (piece[i][j] > 0){
				switch (p->direction)
				{
				case UP:
					break;
				case DOWN:
					//Check for other pieces
					if (currentLevel[i + 1 + p->posX][j + p->posY] > 0){
						return false;
					}
					//Check for bottom
					if (i + 1 + p->posX > 23){
						return false;
					}
					break;
				case LEFT:
					//Check for other pieces
					if (currentLevel[i + p->posX][j - 1 + p->posY] > 0){
						return false;
					}
					//Check MAX
					if (j + p->posY - 1 < 0){
						return false;
					}
					break;
				case RIGHT:
					//Check for other pieces
					if (currentLevel[i + p->posX][j + 1 + p->posY] > 0){
						return false;
					}
					//Check MAX
					if (j + p->posY + 1 > 11) {
						return false;
					}
					break;
				case ROTATE:
					//Check for other pieces and Limit
					if ((currentLevel[i + p->posX][j + p->posY] > 0) || 
						(j + p->posY - 1 < 0) ||
						(j + p->posY + 1 > 11) ){
						return false;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return true;
}


void initGame(){
	gameOver = false;
	std::copy(&level[0][0], &level[0][0] + LEVELCOL * LEVELROW, &currentLevel[0][0]);
	newPiece(); 
	score = 0;
}

//Inputhandling
//Sleeps used for feeling of "control"
void updateInput(){
	//Quit
	if (GetAsyncKeyState(VK_ESCAPE)){
		OutputDebugString("VK_ESCAPE");
		PostQuitMessage(0);
	}
	//Rotate
	if (GetAsyncKeyState(VK_UP)){
		OutputDebugString("VK_UP");
		p->direction = ROTATE;
		tryRotatePiece();
		Sleep(50);
	}
	//Rotate
	if (GetAsyncKeyState(VK_SPACE)){
		OutputDebugString("VK_SPACE");
		p->direction = ROTATE;
		tryRotatePiece();
		Sleep(50);
	}

	//Fast Fall
	if (GetAsyncKeyState(VK_DOWN)){
		OutputDebugString("VK_DOWN");
		p->direction = DOWN;
		movePiece();
	}
	//Move left
	if (GetAsyncKeyState(VK_LEFT)){
		OutputDebugString("VK_LEFT");
		p->direction = LEFT;
		movePiece();
		Sleep(20);
	}
	//Move right
	if (GetAsyncKeyState(VK_RIGHT)){
		OutputDebugString("VK_RIGHT");
		p->direction = RIGHT;
		movePiece();
		Sleep(20);
	}

	if (gameOver){
		//Restart
		if (GetAsyncKeyState(VK_RETURN)){
			OutputDebugString("ENTER");
			initGame();
		}
	}
	
}
//Drawing of bitmaps
void DrawBitmap(char *filename, int x, int y){
	HBITMAP image = (HBITMAP)LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObject(image, sizeof(BITMAP), &bm);

	HDC hdcImage = CreateCompatibleDC(displayDevice);
	SelectObject(hdcImage, image);

	BitBlt(device, x, y, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, SRCCOPY);

	DeleteDC(hdcImage);
	DeleteObject((HBITMAP)image);
}

//Draw text
void drawBackText(LPCSTR text, int y, int x){
	HFONT       font;
	LOGFONT LogFont;
	LogFont.lfWeight = 12;
	font = CreateFontIndirect(&LogFont);
	if (!gameOver){
		SetBkMode(device, TRANSPARENT);
	}
	TextOut(device, y, x, (LPCSTR)text, 14);

	DeleteObject(font);
}

//Text for updates
void updateText(){
	TCHAR buffer[30] = "";
	TCHAR intbuffer[10] = "";
	int test = 99999;
	drawBackText("score: ", 0, 0);
	drawBackText("lines", 100, 0);

	buffer[0] = '\0';
	wsprintf(buffer, TEXT("%d"), score);
	drawBackText(buffer, 40, 0);

	intbuffer[0] = '\0';
	wsprintf(intbuffer, TEXT("%d"), lines);
	drawBackText(intbuffer, 140, 0);
}

//Draw at device and then copy to displayDevice to remove flickering
void updateWindow(int inputLevel[LEVELROW][LEVELCOL]){
	displayDevice = GetDC(window);
	HBITMAP hbMem;
	device = CreateCompatibleDC(displayDevice);
	hbMem = CreateCompatibleBitmap(displayDevice, 192, 384);

	hOld = SelectObject(device, hbMem);

	updateBackground(inputLevel);
	updatePieces(p->posX, p->posY, p->thisPiece);
	
	//Game over print
	if (gameOver){
		drawBackText("game over", SCREENW / 3, SCREENH / 2);
	}
	updateText();
	BitBlt(displayDevice, 0, 0, 192, 384, device, 0, 0, SRCCOPY);

	DeleteObject(hbMem);
	DeleteDC(device);

	ReleaseDC(window, device);
	ReleaseDC(window, displayDevice);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX WindowClass;
	WindowClass.cbSize = sizeof(WNDCLASSEX);

	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
	WindowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
	WindowClass.lpszMenuName = 0;
	WindowClass.lpszClassName = AppNamn;
	WindowClass.hIconSm = 0;

	//Register our class
	return RegisterClassEx(&WindowClass);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//Adjust size on screen so workspace is SCREENW * SCREENH instead of everything including title and frames
	RECT rect = { 0, 0, SCREENW, SCREENH };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	//Create the window
	window = CreateWindow(
		AppNamn, AppNamn,
		WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0
		);

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 1;
}
//Match right bitmap to drawbitmap
void updateBackground(int inputLevel[LEVELROW][LEVELCOL]){
	for (size_t i = 0; i < LEVELROW; i++){ // Y
		for (size_t j = 0; j < LEVELCOL; j++){ // X
			switch (inputLevel[i][j])
			{
			case 0:
				DrawBitmap("white.bmp", j * 16, i * 16);
				break;
			case 1:
				DrawBitmap("red.bmp", j * 16, i * 16);
				break;
			case 2:
				DrawBitmap("orange.bmp", j * 16, i * 16);
				break;
			case 3:
				DrawBitmap("yellow.bmp", j * 16, i * 16);
				break;
			case 4:
				DrawBitmap("green.bmp", j * 16, i * 16);
				break;
			case 5:
				DrawBitmap("bblue.bmp", j * 16, i * 16);
				break;
			case 6:
				DrawBitmap("dblue.bmp", j * 16, i * 16);
				break;
			case 7:
				DrawBitmap("magneta.bmp", j * 16, i * 16);
				break;
			case 9:
				DrawBitmap("gray.bmp", j * 16, i * 16);
				break;
			default:
				break;
			}
		}
	}

}

//match color to drawbitmap
void updatePieces(int posX, int posY, int board[4][4]){
	for (int i = 0; i < 4; i++){ // Y
		for (int j = 0; j < 4; j++){ // X
			switch (board[i][j]){
			case 1:
				DrawBitmap("red.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 2:
				DrawBitmap("orange.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 3:
				DrawBitmap("yellow.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 4:
				DrawBitmap("green.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 5:
				DrawBitmap("bblue.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 6:
				DrawBitmap("dblue.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			case 7:
				DrawBitmap("magneta.bmp", (posY + j) * 16, (posX + i) * 16);
				break;
			default:
				break;
			}
		}
	}
}


void gameDown(){
	ReleaseDC(window, device);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message){
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
	case WM_DESTROY:
		OutputDebugString("Game Close");
		gameDown();
		PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND:
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow){
	MSG msg;
	Timer tmr;

	//Init calls
	initGame();
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) return 0;
	
	for (;;){
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		} else {
			updateInput();
			updateLogic();
			updateWindow(currentLevel);

			//Time for automatic "falling"
			if (tmr.elapsed() > timerPoint){
				p->direction = DOWN;
				movePiece();
				tmr.reset();
			}

		}
	}
	gameDown();
	return static_cast<int>(msg.wParam);
}

// Basic tracker of automatic speed
void updateLogic(){
	if (score < 10000){
		timerPoint = 1;
	} else if (score < 20000){
		timerPoint = 0.8;
	} else if (score < 25000){
		timerPoint = 0.6;
	} else {
		timerPoint = 0.2;
	}
}