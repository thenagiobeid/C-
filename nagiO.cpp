//Nagi Obeid Source File
//Four Steps of Debugging 10-09-2019
//1-Identify
//2-Isolate
//3-Fix
//4-Review
////////////////////////
#include <iostream>
#include <cstdlib>
#include <GL/glx.h>
#include "fonts.h"
#include <fstream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <math.h>
#include<stdio.h>
#include<stdlib.h>

///////DEFINED TYPES////////////
typedef float Vec[3];
typedef float Flt;
///////////MACROS//////////////
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)     ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                                                (c)[1]=(a)[1]-(b)[1]; \
                                                (c)[2]=(a)[2]-(b)[2]
////////CONSTANTS//////////////
const float gravity = 0.2f;
using namespace std;
///////////FUNCTIONS/////////////
void renderBalloon();
void getPathcent(int arr[2]);
void getPatharr(unsigned char arr[18][18]);
void updateBlnPos();
void getStop(float x);
void getBltpos(float x, float y);
void showMenu();
void showgameplayMenu();
//void play();
///////////EXTERN////////////////
extern int getXres();
extern int getYres();
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern double timeSpan;
extern void timeCopy(struct timespec *dest, struct timespec *source);
/////////////////////////////////
class Bullet
{
public:
	float x;
	float y;

	Bullet() {
	}
} blt;

class Path
{
public:
	float cent[2];
	unsigned char mapArr[18][18];
	double startX;
	double startY;
	bool gotMap;
	Path() {
		gotMap = false;
	}
} pth;

class nGlobal
{
public:
	int xres;
	int yres;
	GLuint tttTexture;
        GLuint balloonTexture;
        GLuint balloonTextureblue;
        GLuint balloonTextureyellow;
        GLuint balloonTexturegreen;
        GLuint balloonTexturepurple;
        GLuint menubarTexture;
        GLuint menubar2Texture;
        int menubar;
        int menubar2;
        int ttt;
        int balloon;
        int balloonblue;
        int balloonyellow;
        int balloongreen;
        int balloonpurple;
        nGlobal() {
                ttt=1;
                balloon=2;
        	balloonblue=3;
        	balloonyellow=4;
        	balloongreen=5;
        	balloonpurple=6;
       		menubar=7;
       		menubar2=8;

        	xres = getXres();
        	yres = getYres();
		printf ("Yres: %d\n",yres);
	}
        ~nGlobal() {
        }

} nG;

class Balloon
{
public:
	
	Vec vel;
	double hDiff;
	double wDiff;
	float cent[2];
	float xCoor[4];
	float yCoor[4];
	int currX;
	int currY;
	int stop;
	int go;
	bool startNext;
	bool startMoving;
	struct Balloon* next;
	struct Balloon* prev;
	bool goleft;
	bool goright;
	bool goDown;
	bool goUp;
	bool reachedEnd;
	int blnnumber;
	int blncolor;

	float cirCentr[2];
	float radius;

	Balloon() {
		next = NULL;
		prev = NULL;
		wDiff = 40;
		hDiff = 30.0;//37.5;
		go = 1;
		reachedEnd = false;
		//startMoving = false;
	}
} bln;

//IMAGE CLASS WAS NOT IN OUR CHOSEN FRAMEWORK, 
//HAD TO MIGRATE TO INDIVIVIDUAL CPP FILE
class Image {
public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(const char *fname) {
		if (fname[0] == '\0')
			return;
		int ppmFlag = 0;
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		char ppmname[80];
		if (strncmp(name+(slen-4), ".ppm", 4) == 0)
			ppmFlag = 1;
		if (ppmFlag) {
			strcpy(ppmname, name);
		} else {
			name[slen-4] = '\0';
			sprintf(ppmname,"%s.ppm", name);
			char ts[100];
			sprintf(ts, "convert %s %s", fname, ppmname);
			system(ts);
		}
		FILE *fpi = fopen(ppmname, "r");
		if (fpi) {
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			while (line[0] == '#' || strlen(line) < 2)
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			int n = width * height * 3;
			data = new unsigned char[n];
			for (int i=0; i<n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		} else {
			printf("ERROR opening image: %s\n",ppmname);
			exit(0);
		}
		if (!ppmFlag)
			return;
	}
};
Image img[8] = {"./ttt.png", "./balloon.png", "./b2.png", "./b3.png", "./b4.png", "./b5.png", "./mbar.png", "./mbar2.png"};


class nGameBln
{
public:
	Balloon *ahead;
	Balloon *tail;
	Balloon *n;
	int random;
	nGameBln() {
		srand(time(NULL));
		ahead = NULL;
		srand (time(NULL));
       		for (int i = 0; i < 100; i++) {
			Balloon *b = new Balloon;
			
			random = rand() % 5 + 1;
			b->blncolor = random;
        		
                	b->radius = 25;
			b->blnnumber = i+1;
			
			///X COORDINATE
			b->cent[0] = pth.startY * 80;
       			//printf ("B->CENT[0]: %f\n",b->cent[0]);
			b->currX = pth.startY -0.5;
			
			///Y COORDINATE
			b->cent[1] = pth.startX * 60 + nG.yres;
       			//printf ("B->CENT[1]: %f\n",b->cent[1]);
			b->currY = pth.startX + 0.5;
			
			b->vel[0] = 0.05;
			b->vel[1] = 0.05;
			
			if (ahead == NULL){
				ahead = b;
				ahead->prev = NULL;
				tail = b;
			}
			else {
				b->prev = tail;
				tail->next = b;
				tail = b;
			}
		}
	}
} nGame;

unsigned char *buildAlphaData(Image *img)
{
        int i;
        int a,b,c;
        unsigned char *newdata, *ptr;
        unsigned char *data = (unsigned char *)img->data;
        newdata = (unsigned char *)malloc(img->width * img->height * 4);
        ptr = newdata;
        for (i=0; i<img->width * img->height * 3; i+=3) {
                a = *(data+0);
                b = *(data+1);
                c = *(data+2);
                *(ptr+0) = a;
                *(ptr+1) = b;
                *(ptr+2) = c;
                *(ptr+3) = (a|b|c);
                ptr += 4;
                data += 3;
        }
        return newdata;
}

void initTTT()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	
	//////////////////////////////////
	//main img texture here
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &nG.tttTexture);
	glBindTexture(GL_TEXTURE_2D, nG.tttTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		img[0].width, img[0].height,
		0, GL_RGB, GL_UNSIGNED_BYTE, img[0].data);
	
	///////////////////////////////////////
	//ballon texture here red
	glGenTextures(1, &nG.balloonTexture);
	glBindTexture(GL_TEXTURE_2D, nG.balloonTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteDatared = buildAlphaData(&img[1]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[1].width, img[1].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteDatared);
	free(silhouetteDatared);
	///////////////////////////////////////
	//blue
	glGenTextures(1, &nG.balloonTextureblue);
	glBindTexture(GL_TEXTURE_2D, nG.balloonTextureblue);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteDatablue = buildAlphaData(&img[2]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[2].width, img[2].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteDatablue);
	free(silhouetteDatablue);
	///////////////////////////////////////
	//yellow
	glGenTextures(1, &nG.balloonTextureyellow);
	glBindTexture(GL_TEXTURE_2D, nG.balloonTextureyellow);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteDatayellow = buildAlphaData(&img[3]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[3].width, img[3].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteDatayellow);
	free(silhouetteDatayellow);
	///////////////////////////////////////
	//green
	glGenTextures(1, &nG.balloonTexturegreen);
	glBindTexture(GL_TEXTURE_2D, nG.balloonTexturegreen);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteDatagreen = buildAlphaData(&img[4]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[4].width, img[4].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteDatagreen);
	free(silhouetteDatagreen);
	///////////////////////////////////////
	//purlple
	glGenTextures(1, &nG.balloonTexturepurple);
	glBindTexture(GL_TEXTURE_2D, nG.balloonTexturepurple);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteDatapurple = buildAlphaData(&img[5]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[5].width, img[5].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteDatapurple);
	free(silhouetteDatapurple);
	///////////////////////////////////
	//mbar
	glGenTextures(1, &nG.menubarTexture);
	glBindTexture(GL_TEXTURE_2D, nG.menubarTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *menubar = buildAlphaData(&img[6]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[6].width, img[6].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, menubar);
	free(menubar);
	////////////////////////////////////
	//mbar2
	glGenTextures(1, &nG.menubar2Texture);
	glBindTexture(GL_TEXTURE_2D, nG.menubar2Texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *menubar2 = buildAlphaData(&img[7]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[7].width, img[7].height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, menubar2);
	free(menubar2);
}

void renderTTT(int x, int y)
{
	glColor3f(1.0, 1.0, 1.0);
	if (nG.ttt) {
		glBindTexture(GL_TEXTURE_2D, nG.tttTexture);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); 
			glVertex2i(0, 0);
			glTexCoord2f(0.0f, 0.0f); 
			glVertex2i(0, y);
			glTexCoord2f(1.0f, 0.0f); 
			glVertex2i(x, y);
			glTexCoord2f(1.0f, 1.0f); 
			glVertex2i(x, 0);
		glEnd();
	}
	////////////////////////////////////////////
	showMenu();
}

void getPatharr(unsigned char arr[18][18])
{
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 18; j++) {
			if (arr[i][j] == 's' || arr[i][j] == 'm') {
				if (arr[i][j] == 's') {
					////startX is actually vertical position
					pth.startX = -1*i - 0.5 + nG.yres;
					////startY is actually horizantal position
					pth.startY = j + 0.5;
				}
				pth.mapArr[i][j] = arr[i][j];
			}
			if (arr[i][j] == 'b') {
				pth.mapArr[i][j] = arr[i][j];
			}
		}
	}
	pth.gotMap = true;
}
void updateBlnPos()
{
	Balloon *b = nGame.ahead;
	float dirx = 0.00;
	float diry = 0.00;
	while(b) {
		
		b->startMoving = true;
		///IF MULTIPLE BALLONS ALREADY GENERATED : CHECK//////////
		printf("cent[1]:%f", b->cent[1]);
		if (b->prev != NULL ) {
			////////////////////////816 or 1414
			if (b->prev->cent[1] > 816) {
				b->cent[0] += 0.00;
				b->cent[1] += 0.00;
				b->startMoving = false;
			}
			else {
				b->startMoving = true;
			}
		}
	
		///IF ONLY ONE BALLON OR START MOVING = TRRE: CHECK/////////
		if (b->prev == NULL || b->startMoving) {
			///////////////////////Down/////////////////////////
			if (pth.mapArr[b->currY+1][b->currX] != 'b') {
				b->goDown = true;
				b->goUp = false;
				if (b->goDown) {
					//delete-
					/*if(b->blnnumber <= 37 && b->cent[1] > 1414) {
						dirx = 0.00;
						diry = -1.5;
					}//delete*/
					//else {//delete
						dirx = 0.00;
						diry = -1.50;
					//} //delete
					//
					if (b->go == 1) {
						b->go = 0;
						b->stop = b->cent[1] - 60;
					}
				}
			}
			
			//////////////////////left////////////Right/////////
			else if (pth.mapArr[b->currY][b->currX-1] != 'b' || pth.mapArr[b->currY][b->currX+1] != 'b' ){
				if (pth.mapArr[b->currY][b->currX+1] == 'b'){
					b->goleft = true;
					b->goright = false;
				}
				else if (pth.mapArr[b->currY][b->currX-1] == 'b'){
					b->goleft = false;
					b->goright = true;
				}
				if (b->goleft) {
					dirx = -4.00;
					diry = 0.00;
					if (b->go == 1) {
						b->go = 0;
						b->stop = b->cent[0] - 80;
					}
				}
				else if (b->goright) {
					dirx = 4.00;
					diry = 0.00;
					if (b->go == 1) {
						b->go = 0;
						b->stop = b->cent[0] + 80;
					}
				}
			}

			//-------///////////////CHECKING/////////////////-------//	
			//////////////////////////DOWN///////////////////////////
			if ((b->cent[1] != b->stop) && b->goDown) {
				b->cent[0] += dirx;
				b->cent[1] += diry;
			}	 
			else if (b->cent[1] == b->stop){
				if (b->goDown) {
					b->currX = b->currX;
					b->currY = b->currY+1;
				}
				b->goDown = false;
				b->goUp = false;
				b->go = 1;
				b->stop = 0;
				dirx = 0.00;
				diry = 0.00;
			}	
		
			///////////////////LEFT or RIGHT////////////////////////
			if ((b->cent[0] != b->stop) && (b->goleft || b->goright)) {
				b->cent[0] += dirx;
				b->cent[1] += diry;
			} 
			else if (b->cent[0] == b->stop) {
				if (b->goleft) {
					b->currX = b->currX - 1;
					b->currY = b->currY;
				}
				else if (b->goright) {
					b->currX = b->currX + 1;
					b->currY = b->currY;
				}
				b->go = 1;
				b->stop = 0;
				dirx = 0.00;
				diry = 0.00;
			}

		}/////////////////////end of start moving

		////////////////////checking for collision
		if ((blt.x-b->cent[0])*(blt.x-b->cent[0]) + (blt.y-b->cent[1])*(blt.y-b->cent[1]) < b->radius*b->radius) {
			printf ("Bullet has hit the balloon!!: %f\n",blt.x);
			
			Balloon *b2 = b->next;
			if (b->prev == NULL) {
				if(b->next == NULL) {
					///only 1 balloon
					nGame.ahead = NULL;
				} else {
					//first balloon
					b->next->prev = NULL;
					nGame.ahead = b->next;
				}
			}
			else {	
				if (b->next == NULL) {
					b->prev->next = NULL;
				}
				else {
					b->prev->next = b->next;
					b->next->prev = b->prev;
				}
				//Balloon *b2 = b->next;
				//b->prev->next = b->next;
                        	//b->next->prev = b->prev;
				//delete b;
                        	//b = NULL;
                        	//b = b2;
			}
			delete b;
			b = NULL;
			b = b2;
			if (b == NULL) {
				break;
			}
		}
	printf ("BcurrX%d\n",b->currX);
	printf ("BcurrY%d\n",b->currY);
	b=b->next;
	}
}

void showNagi(int x, int y)
{
    	Rect r;
    	r.center = 0;
	r.left = x;
	r.bot = y;
	ggprint8b(&r, 16, 0x00ff0000, "Nagi Obeid");
	ggprint8b(&r, 16, 0x00ff0000, "Credit Screen");
	ggprint8b(&r, 16, 0x00ff0000, "This is My stuff");
}
void printCredits(int xres, int yres)
{
	glClearColor(0.0,0.0,0.0,1.0);
	int xVal = xres / 2;
	int yVal = yres / 2;

	showNagi(xVal, yVal);
}
void clearScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void checkScores(Rect r, int x, int y)
{
	ifstream fin("Names.txt");

	char name[3];
	int score[5];
	int hiscore = 0;
	int indx = 0;

	if (fin.is_open()) {
		while (fin >> name >> score[indx]) {
			indx++;
		}
		fin.close();
	}

	if (score[0] > score[1] && score[0] > score[2]) {
		hiscore = score[0];
	}
	else if (score[1] > score[2] && score[1] > score[0]) {
		hiscore = score[1];
	}
	else if (score[2] > score[0] && score[2] > score[1]) {
		hiscore = score[2];
	}
	
	r.bot = y/2;
	r.left = x/2;	
	ggprint16(&r, 16, 0xfff0, "HISCORE:%i", hiscore);
}
void renderBalloon()
{
	double oldYres = nG.yres;
	Balloon *b = nGame.ahead;
	float theta;
	while (b) {
		nG.yres = getYres();
		b->cent[1] = b->cent[1] + (nG.yres-oldYres);
		glColor3f(1.0, 1.0, 1.0);
                        glPushMatrix();
			if (b->blncolor == 1) {
                        	glBindTexture(GL_TEXTURE_2D, nG.balloonTexture);
			}
			else if (b->blncolor == 2) {
                        	glBindTexture(GL_TEXTURE_2D, nG.balloonTextureblue);
			}
			else if (b->blncolor == 3) {
                        	glBindTexture(GL_TEXTURE_2D, nG.balloonTextureyellow);
			}
			else if (b->blncolor == 4) {
                        	glBindTexture(GL_TEXTURE_2D, nG.balloonTexturegreen);
			}
			else if (b->blncolor == 5) {
                        	glBindTexture(GL_TEXTURE_2D, nG.balloonTexturepurple);
			}
                        
			glEnable(GL_ALPHA_TEST);
                        glAlphaFunc(GL_GREATER, 0.0f);
                        glColor4ub(255,255,255,255);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
                                glVertex2i(b->cent[0] - b->wDiff ,b->cent[1] - b->hDiff );
                                glTexCoord2f(0.0f, 0.0f);
                                glVertex2i(b->cent[0] - b->wDiff ,b->cent[1] + b->hDiff );
                                glTexCoord2f(1.0f, 0.0f);
                                glVertex2i(b->cent[0] + b->wDiff ,b->cent[1] + b->hDiff );
                                glTexCoord2f(1.0f, 1.0f);
                                glVertex2i(b->cent[0] + b->wDiff ,b->cent[1] - b->hDiff );
                        glEnd();
                        glPopMatrix();
                        glDisable(GL_ALPHA_TEST);
			
			///CIRLCE WITH RADIUS
			//
			/*
			glColor3f(0,0,0);
			glBegin(GL_POLYGON);
			for (int i = 0; i< 360; i++) {
				theta = i*3.142/180;
				glVertex2f(b->cent[0]+30*cos(theta), b->cent[1]+30*sin(theta));
			}
			glEnd();
			*/	

			b->cirCentr[0] = b->cent[0]+30*cos(theta);
			b->cirCentr[1] = b->cent[1]+30*sin(theta);
			b->radius = 30;
			/////end circle
			
			b = b->next;
	}
}

void getBltpos(float x, float y) {
	blt.x = x;
	blt.y = y;
}

void showMenu() {
	glColor4ub(255,255,255,255);
	glBindTexture(GL_TEXTURE_2D, nG.menubarTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
                glVertex2i(500,100);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(500,200);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(1100,200);
                glTexCoord2f(1.0f, 1.0f);
               	glVertex2i(1100,100);
	glEnd();
	
	glColor4ub(255,255,255,255);
	glBindTexture(GL_TEXTURE_2D, nG.menubarTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
                glVertex2i(1200,100);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(1200,200);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(1800,200);
                glTexCoord2f(1.0f, 1.0f);
               	glVertex2i(1800,100);
	glEnd();
		
    	Rect m1;
    	m1.center = 0;
	m1.left = 630;
	m1.bot = 140;
	ggprint16(&m1, 16, 0x00ff0000, "PRESS DOWN ARROW TO START");
    	
	Rect m2;
    	m2.center = 0;
	m2.left = 1400;
	m2.bot = 140;
	ggprint16(&m2, 16, 0x00ff0000, "PRESS (ESC) TO EXIT");
}

void showgameplayMenu() {
	glColor4ub(255,255,255,255);
	glBindTexture(GL_TEXTURE_2D, nG.menubar2Texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
                glVertex2i(1500,400);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(1500,1460);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(2200,1460);
                glTexCoord2f(1.0f, 1.0f);
               	glVertex2i(2200,400);
	glEnd();
}
