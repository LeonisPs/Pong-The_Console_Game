#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

#define MaxX 75
#define MaxY 25
#define Paddle 219 //█
#define Ball 254   //■
#define Wall 124   //|
#define Space 255  // 

char map[MaxX*MaxY];

struct Obj {
	char sprite;
	float X;
	float Y;
	float vX = 1.0f;
	float vY;
}
Player1{(char)Paddle,1,MaxY/2-1},
Player2{(char)Paddle,MaxX-1,MaxY/2-1},
ball   {(char)Ball,MaxX/2,MaxY/2};

int Score1 = 0;
int Score2 = 0;


struct sKeyState
{
	bool bPressed;
	bool bReleased;
	bool bHeld;
}
m_keys[256];

short m_keyOldState[256] = { 0 };
short m_keyNewState[256] = { 0 };

void KeyboardInput() 
{
	for (int i = 0; i < 256; i++)
	{
		m_keyNewState[i] = GetAsyncKeyState(i);

		m_keys[i].bPressed = false;
		m_keys[i].bReleased = false;

		if (m_keyNewState[i] != m_keyOldState[i])
		{
			if (m_keyNewState[i] & 0x8000)
			{
				m_keys[i].bPressed = !m_keys[i].bHeld;
				m_keys[i].bHeld = true;
			}
			else
			{
				m_keys[i].bReleased = true;
				m_keys[i].bHeld = false;
			}
		}

		m_keyOldState[i] = m_keyNewState[i];
	}
}

void MoveBall()
{
	//find nextPos
	int nextX = ball.X + ball.vX;
	int nextY = ball.Y + ball.vY;
	
	//hitting the Paddle
	if(map[nextY*MaxX+nextX] == (char)Paddle)
	{
		ball.vX *= -1;
		//find witch one
		if(nextX > MaxX/2)
		{
			(Player2.vY<=0.5f)? ball.vY += Player2.vY: ball.vY += 0.5f;
		}
		else
		{
			(Player1.vY<=0.5f)? ball.vY += Player1.vY: ball.vY += 0.5f;	
		}
	}
	
	//Hitting the gutters
	if(nextY < 0 || nextY >= MaxY)
		ball.vY *= -1;	
	
	//Reset Ball after score
	if (nextX <= 0 || nextX >= MaxX)
	{
		ball.X = MaxX/2;
		ball.Y = MaxY/2;
		ball.vY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX*0.5f));
		//find who scored
		(nextX == MaxX)?Score1++:Score2++;
	}
	
	//Move the Ball
	ball.X += ball.vX;
	ball.Y += ball.vY;
	
	//reset the players velocity
	Player1.vY = 0;
	Player2.vY = 0;
}

void MovePlayers()
{
	if(m_keys['W'].bHeld)
		if(Player1.Y>0)
			Player1.vY = -1;
			
	if(m_keys['S'].bHeld)
		if(Player1.Y+3<MaxY)
			Player1.vY = 1;
	
	if(m_keys[VK_UP].bHeld)
		if(Player2.Y>0)
			Player2.vY = -1;
	
	if(m_keys[VK_DOWN].bHeld)
		if(Player2.Y+3<MaxY)
			Player2.vY = 1;
	//Check if next move is out of bounds
	if(!(Player1.Y+3==MaxY && Player1.Y==0))
		Player1.Y += Player1.vY;
	if(!(Player2.Y+3==MaxY && Player2.Y==0))
		Player2.Y += Player2.vY;
	
}                   

void set_cursor(int x = 0 , int y = 0)
{
    HANDLE handle;
    COORD coordinates;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition ( handle , coordinates );
}

void ShowHideCursor(bool Flag)
{
	CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = Flag; 
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void PrintMap()
{	
	std::string title = "Player1: "+std::to_string(Score1)+" Player2: "+std::to_string(Score2);
	set_cursor(MaxX/2- title.size()/2 ,0);
	std::cout<<title;
	for(int y=0; y<MaxY; y++)
	{
		for(int x=0; x<MaxX; x++)
		{
			set_cursor(x,y+1);
			std::cout << map[y*MaxX+x];
		}
	}
}

void UpdateMap() 
{
	for(int i = 0; i < MaxX*MaxY; i++) {
		if(i%MaxX == MaxX/2 ) {
			map[i] = Wall;
		} else {
			map[i] = Space;
		}
	}
	for(int i = 0; i<3; i++) {
		map[((int)Player1.Y+i)*MaxX+(int)Player1.X] = Player1.sprite;
		map[((int)Player2.Y+i)*MaxX+(int)Player2.X] = Player2.sprite;
	}
	map[(int)ball.Y*MaxX+(int)ball.X] = ball.sprite;
}


int main() 
{
	srand (static_cast <unsigned> (time(0)));
	ball.vY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX*0.5f));
	ShowHideCursor(false);
	while(!m_keys[VK_ESCAPE].bPressed) {
		KeyboardInput();
		MovePlayers();
		MoveBall();
		UpdateMap();
		PrintMap();		
	}
	ShowHideCursor(true);
	return 0;
}
