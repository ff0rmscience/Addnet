#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<curses.h>
#define M 200
#define PI 3.14159265358979323846
enum {dummy, white, yellow, red, rwhite,ryellow,rred, black, blue, rblue, green,cyan};
typedef double real;
int H = 34, W= 10, keyboard = 0, F_code = 1, mode = 0, auto_mode = 0, left = 40;
int h[M] = {0}, f_last_plot[M][2], F_last_plot[M][2], f_last_length= 0, F_last_length=0;
real f[M] = {0};
real mean_error = 0, error_sum = 0, error = 0;
int batch_size = 10000;
real w[M][M] = {0};
real ff = 0;
real noise = 0.01;
real rate = 0.0001;
void forward(real);
void set_color(int);
void set_up_graphics();
real random_real();
void randomize_w();
void respond();
void print_w();
void print_h();
void backward(real);
real F(real);
void print_stats();
void print_special();
void print_menu();
void results();
void train();
void plot_functions(int, int, real, real, real, real, int, int);
void print_comparison();
char* F_name[30] =
{	"x                      ",
	"|sin(2*pi*x)|          ",
	"x^2                    ",
	"x > 0.5 ? 1 : 0        ",
	"5*fmod(x,0.2)          ",
	"1 - |1 - 2x|           ",
	"|sin(3*pi*x)|          ",
	"exp(-x^2)              ",
	"x < 0.5 ? 1 : 0        ",
	"sin(pi*x)              ",
	"x^3                    ",
	"(e^x - 1)/(e - 1)      ",
	"|fmod(sin(pi*x),cos(x)|",
	" ?                     ",
};


int main()
{
	set_up_graphics();
	randomize_w(-.01,.01);
	results();
	
	while (1) respond();
}

void forward(real x)
{
	//real e = 0.0000001;
	f[0] = x;
	h[0] = (int)floor(x*H);
	
	for (int i = 1; i < W; i++)
	{
		f[i] = f[i-1]+w[i-1][h[i-1]];
		if (f[i] < 0) f[i] = 0.0;
		if (f[i] > 1.0) f[i] = .999999999999999;
		h[i] = (int)floor(f[i]*H);
	}
	ff = f[W-1] +w[W-1][h[W-1]];
}

void backward(real x)
{
	for (int i = 0; i < W; i++)
	{
		w[i][h[i]] -= rate*x;
	}
}

void show_computation()
{

	for (int i = 0; i< W; i++)
	{
		move(i,left);
		printw("f[%2d] = %5.5f (%2d)  w[%2d][%2d] = %+5.5f",i,f[i], h[i], i, h[i], w[i][h[i]]);
	}
	move(W+1,left);
	printw("f     = %+.5f",ff);
	move(W+2,left);
	printw("F     = %+.5f", F(f[0]));
	move(W+3,left);    
	printw("f - F = %+.5f",ff - F(f[0]));
	
	refresh();
}
void set_up_graphics()
{
	initscr();
	noecho();
	start_color();
	raw();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);
	init_pair(white, COLOR_WHITE, COLOR_BLACK);
	init_pair(yellow, COLOR_YELLOW, COLOR_BLACK);
	init_pair(red, COLOR_RED, COLOR_BLACK);
	init_pair(rwhite, COLOR_BLACK, COLOR_WHITE);
	init_pair(ryellow, COLOR_BLACK, COLOR_YELLOW);
	init_pair(rred, COLOR_BLACK, COLOR_RED);
	init_pair(black, COLOR_BLACK, COLOR_BLACK);
	init_pair(blue, COLOR_BLUE, COLOR_BLACK);
	init_pair(rblue, COLOR_BLACK, COLOR_BLUE);
	init_pair(green, COLOR_GREEN, COLOR_BLACK);
	init_pair(cyan, COLOR_CYAN, COLOR_BLACK);

	
}
void set_color(int color)
{
	attron(COLOR_PAIR(color));
}

void randomize_w()
{
	for (int i = 0; i < M; i++) 
	{
		for (int j = 0; j < M; j++) w[i][j] = (1 - rand()%3)*0.1;
	}
}
real random_real()
{
	return ((real)rand() / RAND_MAX);
}
void respond()
{
	print_stats();
	keyboard = getch();
	switch(keyboard)
	{
		case 'q': exit(0); break;
		case 'f': 
				forward(random_real());
				results();
				//show_computation();
				break;
		case 'w':
				W--; clear();if (W < 1) W = 1; results();break;

		case 'W': W++;clear(); if (W > 18) W = 18;results();break;
		case 'h': H--;if (H < 1) H=1;clear();results();break;
		case 'H': H++; if(H > 50) H =50;clear();results(); break;
		case 'l': train();results();break;
		case 'c': clear();break;
		case 'd': clear(); mode++; if(mode>3) mode = 0;results();break;
		case KEY_RIGHT: rate += 0.000001;;break;
		case KEY_LEFT:  rate -= 0.000001;if (rate < 0) rate = 0;break;
		case 'r': randomize_w();results();break;
		case 'F': F_code++; if (F_code > 13) F_code = 0;results();break;
		case 'a': if (auto_mode == 0) auto_mode =1; else auto_mode = 0;results();break;
		case 'N': noise += 0.001;break;
		case 'n': noise -= 0.001;if (noise < 0) noise = 0.0; break;
		case ERR: if (auto_mode ==1) {train();results();} break;
				
	}
}
void print_w()
{
	
	real FF = F(f[0]);
	//if (FF = 1.0) FF = 0.99999999;
	int target_height = 0;

	int pW = (W<21)?W:20;

	for (int j = 0 ; j <= H; j++)
	{
		move(j,left);
		printw("\n");
	}
	set_color(rred);
	move(h[0],left);
	printw("%5.4f",f[0]);
	move(h[W-1],left+6*pW+5);
	printw("%5.4f",ff);
	target_height = (int)floor(FF*H);
	if (target_height > H-1) target_height = H-1;
	move(target_height,left+6*pW+11);
	set_color(rwhite);
	printw("%5.4f",FF);
	set_color(white);
	for (int i = 0; i < pW; i++)
	{
		for (int j = 0; j < H; j++)
		{
			move(j,left+6+i*6);
			if (h[i] != j) set_color(white);else set_color(red);
			if (fabs(w[i][j]) < 10) printw("%+2.2f", w[i][j]); else printw(" #### ");
		}
	}
	set_color(white);
	refresh();
}
void print_special()
{
	for (int i = 0; i < W; i++)
	{
		for (int j = 0; j < H; j++)
		{
			move(j,30+i*7);
			if (h[i] != j) set_color(white);else set_color(rwhite);
			printw("%+.3f", w[i][j]);
		}
	}
	set_color(white);
	refresh();
}
void print_h()
{

	set_color(white);
	real FF = F(f[0]);
	for (int j = 0 ; j < H; j++)
	{
		move(j,left);
		printw("\n");
	}
	move(h[0],left);
	printw("%.5f",f[0]);
	move(h[W-1],left+W+8);
	printw("%.5f",ff);
	move((int)floor(FF*H),left+W+16);
	set_color(rwhite);
	printw("%.5f",FF);
	set_color(white);
	for (int i = 0; i < W; i++)
	{
		for (int j = 0; j < H; j++)
		{
			move(j,left+8+i);
			if (h[i] != j) set_color(rwhite);else set_color(rred);
			printw(" ", w[i][j]);
		}
	}
	set_color(white);
	print_stats();
	refresh();
}
real F(real x)
{
	real g = 0;
	switch(F_code)
	{
		case 0: g= x;break;
		case 1: g= fabs(sin(2*PI*x));break;
		case 2: g= x*x;break;
		case 3: g= (real)(x > 0.5);break;
		case 4: g = 5*fmod(x,.2);break;
		case 5: g = fabs(1-fabs(1-2*x));break;
		case 6: g = fabs(sin(3*PI*x));break;
		case 7: g = exp(-x*x);break;
		case 8: g = (real)(x < 0.5);break;
		case 9: g = sin(PI*x);break;
		case 10: g =  x*x*x;break;
		case 11: g = (exp(x)-1)/(exp(1)-1);break;
		case 12: g = fabs(fmod(sin(PI*x),cos(x)));break;
		case 13: g = random_real();break;
		default: g=  1;
	}
	g += (1 - rand()%3)*noise*random_real();
	if (g < 0 ) g = 0;
	if (g >1) g =.999999999;
	return g;
}
void print_stats()
{
	attron(A_BOLD);
	move(0,0);
	printw("mean error = %.6f",mean_error);
	move(1,0);
	printw("rate       = %.6f", rate);	
	move(2,0);
	printw("noise      = %.6f",noise);
	move(3,0);
	printw("width      = %6d", W);
	move(4,0);
	printw("height     = %6d", H);
	move(5,0);
	printw("auto_mode  = %6d", auto_mode);
	move(6,0);
	printw("F(x)       = %s", F_name[F_code]);
	move(7,0);
	printw("display    = %6d", mode); 
	//move(7,0);
	//printw("l/d/w/W/F/a");
	attroff(A_BOLD);
}
void print_menu()
{
	int down = 20;
	move(down,0);
	//printw("(l)(f)(d)(w)(W)(r)(F)");
	move(down+1,0);
	printw("(f)orward");
	move(down+2,0);
	printw("(d)isplay");
	move(down+3,0);
	printw("(W)idth++");
	move(down+4,0);
	printw("(w)idth--");
	move(down+5,0);
	printw("(r)andomize");
	move(down+6,0);
	printw("(a)uto");
	move(down+7,0);
	printw("(l)earn");
	move(down+8,0);
	printw("(N)oise++");
	move(down+9,0);
	printw("(n)oise--");
	move(down+10,0);
	printw("(F)unction++");
	refresh();
}

void results()
{
	print_stats();
	print_menu();
	switch(mode)
	{
		case 0: print_h();break;
		case 1: print_w();break;
		case 2: show_computation();break;
		case 3: print_comparison();break;
	}
	refresh();
}

void train()
{
	error_sum = 0;
	for (int i = 0; i < batch_size;i++)
	{
		forward(random_real());
		error = ff - F(f[0]);
		error_sum += fabs(error);
		backward(error);
	}
	mean_error = error_sum/batch_size;
}
void print_comparison()
{

	int down = H;
	real x = 0;
	set_color(white);
	move(0,left);
	attron(A_BOLD);
	printw(" x         f         F         |f - F|");
	attroff(A_BOLD);
	for (int i = 0; i < down; i++)
	{
		x = (real)i/down;
		forward(x);
		move(i+1,left);
		set_color(white);
		printw("%+5.5f",x);
		move(i+1, left + 10);
		set_color(red);
		printw("%+5.5f",ff);
		move(i+1,left+20);
		//attron(A_BLINK);
		set_color(white);
		printw("%+5.5f",F(x));
		//attroff(A_BLINK);
		set_color(red);
		move(i+1, left+30);
		//attron(A_BOLD);
		printw("%+5.5f", fabs(ff - F(x)));
		//attroff(A_BOLD);
	}
	refresh();
	set_color(white);
}


