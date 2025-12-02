#include <stdio.h>		// 标准输入输出 
#include <time.h>		// 时间 
#include <windows.h>	// Windows API 
#include <conio.h>		// 控制台输入输出 

#define HEIGHT 20		// 地图的高 
#define WIDTH 80		// 地图的宽
#define SLEEPTIME 200	// 默认爬行间隔 

struct BODY	// 定义结构体，存储单节蛇的位置坐标 
{
	int X;	// 存储X轴坐标 
	int Y;	// 存储Y轴坐标 
};

struct SNAKE	// 定义结构体，存储每节蛇的位置坐标和蛇的长度 
{
	struct BODY body[HEIGHT * WIDTH];	// 蛇的节点数组，最大为地图大小 
	int len;	// 存储蛇的长度 
} snake;	// 一个蛇对象 

struct FOOD	// 定义结构体，存储食物的位置坐标 
{
	int X;	// 存储X轴坐标 
	int Y;	// 存储Y轴坐标 
} food;	// 一个食物对象 

int score = 0;		// 初始化成绩的值为0 

int kx, ky;				// 蛇头偏移方向 

int lastX, lastY;		// 蛇尾坐标 

int sleepTime = SLEEPTIME;	// 蛇的爬行间隔时间

void initSnake();				// 初始化蛇 
int isOnSnake(int x, int y);	// 检查坐标是否在蛇身上
void initFood();				// 初始化食物 
void initUI();					// 初始化界面 
void initWall();				// 初始化墙 
void playGame();				// 游戏进行

void initSnake()		// 初始化蛇 
{
	snake.len = 5;		// 蛇的初始长度是5 
	int i;
	for(i = 0; i < snake.len; i++)
	{
		snake.body[i].X = WIDTH / 2 - i;	// 蛇头在地图中心点，蛇身依次向左排列 
		snake.body[i].Y = HEIGHT / 2;
	}
}

int isOnSnake(int x, int y)	// 检查坐标是否在蛇身上
{
	int i;
	for(i = 0; i < snake.len; i++)
	{
		if((snake.body[i].X == x && snake.body[i].Y == y) ||
		   (lastX == x && lastY == y))
			return 1;	// 在蛇身上
	}
	return 0;	// 不在蛇身上
}

void initFood()	// 初始化食物 
{
	srand(time(NULL));				// 播种随机数种子 
	do
	{
		food.X = (rand() % (WIDTH - 2)) + 1;	// 随机食物的X轴坐标，确保在边界内
		food.Y = (rand() % (HEIGHT - 2)) + 1;	// 随机食物的Y轴坐标，确保在边界内
	}while(isOnSnake(food.X, food.Y));	// 如果食物生成在蛇身上，重新生成
}

void initUI()	// 初始化界面 
{
	COORD coord;	// 光标定位 
	
	int i;
	for(i = 0; i < snake.len; i++)	// 画蛇 
	{
		coord.X = snake.body[i].X;	// 光标的X轴设置为蛇头的X轴 
		coord.Y = snake.body[i].Y;	// 光标的Y轴设置为蛇头的Y轴 
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);	// 设置光标位置 
		if(i == 0)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x72);			// 设置文本颜色为白底绿字 
			putchar('O');	// 画蛇头 
		}
		else
		{
			if(i % 2 != 0) 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x71);	// 设置文本颜色为白底蓝字 
			else
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x72);	// 设置文本颜色为白底绿字 
			putchar('o');	// 画蛇尾 
		}	
	}
	
	coord.X = lastX;		// 光标的X轴设置为蛇尾的X轴 
	coord.Y = lastY;		// 光标的Y轴设置为蛇尾的Y轴 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);		// 设置光标位置 
	putchar(' ');			// 清除蛇尾，用空格替代 
	
	coord.X = food.X;		// 光标的X轴设置为食物的X轴 
	coord.Y = food.Y;		// 光标的Y轴设置为食物的Y轴 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);		// 设置光标位置 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x74);			// 设置文本颜色为黑底红字 
	putchar('@');			// 画食物 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x70);			// 将文本颜色改回白底黑字 
}

void initWall()	// 初始化墙 
{
	int i;
	for(i = 0; i <= HEIGHT; i++)			// 行 
	{
		int j;
		for(j = 0; j <= WIDTH; j++)			// 列 
		{
			if(j == WIDTH && i != HEIGHT)
				printf("|");				// 打印右墙 
			else if(i == HEIGHT)
				printf("`");				// 打印下墙 
			else
				printf(" ");				// 其余填充空白 
		}
		printf("\n");
	}
}

void playGame()	// 游戏进行
{
	char key = 'd';	// 默认蛇向右移动 
	
	while(snake.body[0].X >= 0 && snake.body[0].X < WIDTH &&
		  snake.body[0].Y >= 0 && snake.body[0].Y < HEIGHT)	// 判断蛇是否撞墙 
	{
		initUI();	// 重画蛇与食物 
		
		char oldKey = key;	// 保存上一次的输入 
		if(kbhit())	// 不阻塞判断用户输入，有用户输入返回真，否则返回假
		{	 
			char newKey = getch();	// 接受用户输入 
			
			if(!((newKey == 'a' && oldKey == 'd') ||
			  	 (newKey == 'd' && oldKey == 'a') ||
			  	 (newKey == 'w' && oldKey == 's') ||
			  	 (newKey == 's' && oldKey == 'w')))
			{
				key = newKey;	// 非相反方向才更新 
			}
		}
		switch(key)
		{
			case 'A':				// 如果按下A 
			case 'a':				// 如果按下a 
				kx = -1, ky = 0;	// 偏移方向x轴减少，y轴不变（即向左） 
				break;
			case 'D':				// 如果按下D
			case 'd':				// 如果按下d
				kx = 1, ky = 0;		// 偏移方向x轴增加，y轴不变（即向右） 
				break;
			case 'W':				// 如果按下W
			case 'w':				// 如果按下w
				kx = 0, ky = -1;	// 偏移方向x轴不变，y轴减少（即向上） 
				break;
			case 'S':				// 如果按下S
			case 's':				// 如果按下s
				kx = 0, ky = 1;		// 偏移方向x轴不变，y轴增加（即向下） 
				break;
			default:
				break;
		}
		
		int speed = sleepTime;  		// 默认速度
		if(GetAsyncKeyState(VK_SHIFT))	// 如果按住左/右 SHIFT
		{
    		speed = sleepTime / 10;  	// 加速：时间间隔变为原来的十分之一 
    		if(speed < 30)
				speed = 30; 			// 限制最小速度
		}
		
		int i;
		for(i = 1; i < snake.len; i++)
		{
			if(snake.body[0].X == snake.body[i].X &&
			   snake.body[0].Y == snake.body[i].Y)	// 蛇头与任一蛇身碰撞 
				return;	//游戏结束 
		}
		if(snake.body[0].X == food.X && snake.body[0].Y == food.Y)	// 蛇头碰到食物 
		{
			initFood();			// 重新生成食物 
			snake.len++;		// 蛇身增长 
			score++;			// 分数增加
			sleepTime -= 5;		// 蛇加速 
			if(sleepTime < 50)
				sleepTime += 50;
		}
		
		lastX = snake.body[snake.len - 1].X;	// 记录蛇尾坐标 
		lastY = snake.body[snake.len - 1].Y;
		
		// 蛇身体移动，前一节给后一节赋值 
		for(i = snake.len - 1; i > 0; i--)
		{
			snake.body[i].X = snake.body[i - 1].X;
			snake.body[i].Y = snake.body[i - 1].Y;
		}
		// 根据用户输入修改蛇头坐标 
		snake.body[0].X += kx;
		snake.body[0].Y += ky;
		
		// 即时显示当前分数 
		COORD coord;				// 光标位置  
		coord.X = WIDTH + 1;		// 光标的X轴设置地图外右侧 
		coord.Y = 0;				// 光标的Y轴设置为首行 
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);	// 设置光标位置 
		printf("得分：%d", score);	// 打印分数 
		
		coord.X = WIDTH + 1;		// 光标的X轴设置地图外右侧 
		coord.Y = 1;				// 光标的Y轴设置为第二行行 
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);	// 设置光标位置 
		printf("          ");
		
		coord.X = WIDTH + 1;		// 光标的X轴设置地图外右侧 
		coord.Y = 1;				// 光标的Y轴设置为第二行行 
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);	// 设置光标位置 
		printf("速度：%d", (SLEEPTIME - speed) / 5 + 1);	// 打印当前速度 
		
		Sleep(speed);	// 下一次重画前暂停 
	}
}

int main()
{
	system("color 70");
	CONSOLE_CURSOR_INFO console_cursor_info;
	console_cursor_info.dwSize = sizeof(console_cursor_info);	// 光标大小等于本身 
	console_cursor_info.bVisible = 0;	// 光标不可见 
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_cursor_info);
	
	printf("########################################################################\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("#                               贪 吃 蛇                               #\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("#       游戏说明：使用WASD操作移动蛇，吃到食物分数增加，速度增快       #\n");
	printf("#                 按下Shift键可使蛇加速                                #\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("#                                                                      #\n");
	printf("########################################################################\n");
	
	system("pause");	// 暂停 
	system("cls");		// 清屏 
	
	initSnake();		// 初始化蛇 
	initFood();			// 初始化食物 
	
	initWall();			// 初始化墙 
	initUI();			// 初始化界面 
	
	playGame();			// 进行游戏 
	
	COORD coord;
	coord.X = 0;
	coord.Y = HEIGHT + 1;	// 移动光标至地图外 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf(">>>>> 游戏结束！ <<<<<\n\n");
	printf("你的成绩是%d分！", score);	// 打印成绩 
	return 0;
}
