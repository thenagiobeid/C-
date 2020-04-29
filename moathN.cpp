//3350
//program: walk2.cpp
//author:  Gordon Griesel
//date:    summer 2017
//         spring 2018
//
//Walk cycle using a sprite sheet.
//images courtesy: http://games.ucla.edu/resource/walk-cycles/
//
//This program includes:
//  multiple sprite-sheet animations
//  a level tiling system
//  parallax scrolling of backgrounds
//
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
//#include "log.h"
//#include "ppm.h"
#include "fonts.h"
#include <fstream>
#include <iostream>

using namespace std;

//function prototypes
void MinitOpengl(void);
void Minit();
void Mphysics();
void Mrender();

class MImage;
typedef double Vec[3];

typedef struct Map1{

    int status;
    int hover;
    int type;

}map1;
map1 grid[18][18];
class Turret {
    public:
        int posx, posy;
        float angle;
        int upgrade;
        int type;


    public:
        Turret(){
            angle = 90.0f;
            upgrade = 0;

        }
}turret;
int qsize;
class MGlobal {
    public:
        Turret Tur[3][4];
        int grid_dim;
        int board_dim;
        int maxTur, maxTur2;
        MImage * ground;
        GLuint groundTex;
        MImage * sew;
        GLuint sewTex;
        MImage * sns;
        GLuint snsTex;
        MImage * cne;
        GLuint cneTex;
        MImage * cnw;
        GLuint cnwTex;
        MImage * cse;
        GLuint cseTex;
        MImage * csw;
        GLuint cswTex;
        int currType;
        ~MGlobal() {
        }
        MGlobal() {
            grid_dim = 15;
            board_dim = 800;
            maxTur = 0;
            maxTur2 = 0;


        }
} mgl;
class MLevel {
    public:
        unsigned char arr[18][18];
        int nrows, ncols;
        MLevel() {
            //Log("Level constructor\n");
            //read level
            FILE *fpi = fopen("level1.txt","r");
            if (fpi) {
                nrows=0;
                char line[100];
                while (fgets(line, 100, fpi) != NULL) {
                    removeCrLf(line);
                    int slen = strlen(line);
                    ncols = slen;
                    //Log("line: %s\n", line);
                    for (int j=0; j<slen; j++) {
                        arr[nrows][j] = line[j];
                    }
                    ++nrows;
                }
                fclose(fpi);
                //printf("nrows of background data: %i\n", nrows);
            }
            for (int i=0; i<nrows; i++) {
                for (int j=0; j<ncols; j++) {
                    printf("%c", arr[i][j]);
                }
                printf("\n");
            }
	////////////////////////////////nagi
	extern void getPatharr(unsigned char arr[18][18]);
        getPatharr(arr);
	///////////////////////////////nagi
        }
        void removeCrLf(char *str) {
            //remove carriage return and linefeed from a Cstring
            char *p = str;
            while (*p) {
                if (*p == 10 || *p == 13) {
                    *p = '\0';
                    break;
                }
                ++p;
            }
        }
} mlev;
class MImage {
    public:
        int width, height;
        unsigned char *data;
        ~MImage() { delete [] data; }
        MImage(const char *fname) {
            if (fname[0] == '\0')
                return;
            //printf("fname **%s**\n", fname);
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
                //printf("name **%s**\n", name);
                sprintf(ppmname,"%s.ppm", name);
                //printf("ppmname **%s**\n", ppmname);
                char ts[100];
                //system("convert eball.jpg eball.ppm");
                sprintf(ts, "convert %s %s", fname, ppmname);
                system(ts);
            }
            //sprintf(ts, "%s", name);
            //printf("read ppm **%s**\n", ppmname); fflush(stdout);
            FILE *fpi = fopen(ppmname, "r");
            if (fpi) {
                char line[200];
                fgets(line, 200, fpi);
                fgets(line, 200, fpi);
                //skip comments and blank lines
                while (line[0] == '#' || strlen(line) < 2)
                    fgets(line, 200, fpi);
                sscanf(line, "%i %i", &width, &height);
                fgets(line, 200, fpi);
                //get pixel data
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
                unlink(ppmname);
        }
};
MImage Mimg[7] = {
    "./images/dirt_full_new.png",
    "./images/ssew.png",
    "./images/ssns.png",
    "./images/scne.png",
    "./images/scnw.png",
    "./images/scse.png",
    "./images/scsw.png"};



void MinitOpengl(void)
{

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_COLOR_MATERIAL);

    glBindTexture(GL_TEXTURE_2D, 0);
    //load the images file into a ppm structure.
    int w = Mimg[0].width;
    int h = Mimg[0].height;
    mgl.ground = &Mimg[0];
    glGenTextures(1, &mgl.groundTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.groundTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.ground->data);
    w = Mimg[1].width;
    h = Mimg[1].height;
    mgl.sew = &Mimg[1];
    glGenTextures(1, &mgl.sewTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.sewTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.sew->data);
    w = Mimg[3].width;
    h = Mimg[3].height;
    mgl.cne = &Mimg[3];
    glGenTextures(1, &mgl.cneTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.cneTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.cne->data);
    w = Mimg[4].width;
    h = Mimg[4].height;
    mgl.cnw = &Mimg[4];
    glGenTextures(1, &mgl.cnwTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.cnwTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.cnw->data);
    w = Mimg[5].width;
    h = Mimg[5].height;
    mgl.cse = &Mimg[5];
    glGenTextures(1, &mgl.cseTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.cseTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.cse->data);
    w = Mimg[6].width;
    h = Mimg[6].height;
    mgl.csw = &Mimg[6];
    glGenTextures(1, &mgl.cswTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.cswTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.csw->data);
    w = Mimg[2].width;
    h = Mimg[2].height;
    mgl.sns = &Mimg[2];
    glGenTextures(1, &mgl.snsTex);
    //create opengl texture elements
    glBindTexture(GL_TEXTURE_2D, mgl.snsTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, mgl.sns->data);
}
int qsize2;
void Minit() {
    qsize = 80;
    qsize2 = 60;
}

void get_grid_center(const int g, const int i, const int j, int cent[2])
{
    int s0 ;//= gl.xres;
    int s1 = qsize2;
    //This determines the center of each grid.
    switch(g) {
        case 1:
            s0 = qsize;
            break;
    }
    cent[0] = s0;
    cent[1] = s1;
    cent[0] *= j;
    cent[1] *= i;
}
void checkhover(int x, int y, int yyres)
{
    int cent[2];
    int i,j;
    for(i =0; i < 18; i++) {
        for( j =0; j < 18; j++) {
            grid[i][j].hover=0;
        }
    }

    for( i =0; i < 18; i++) {
        for(j =0; j < 18; j++) {
            get_grid_center(1,i,j,cent);
            if (x >= cent[0]-qsize &&
                    x <= cent[0]+qsize &&
                    y >= yyres-cent[1]-qsize2 &&
                    y <= yyres-cent[1]+qsize2) {
                grid[i][j].hover=1; 
                break;
            }

        }
        if(grid[i][j].hover)
            break;    
    }
}
void clearhover()
{
    int i,j; 
    for(i =0; i < 18; i++) {
        for( j =0; j < 18; j++) {
            grid[i][j].hover=0;
        }
    }

}
void checkT(int typet, int yres)
{
    int cent[2];
    int i,j;
    for(i =0; i < 18; i++) {
        for( j =0; j < 18; j++) {
            get_grid_center(1,i,j,cent);
            if(grid[i][j].hover) {
                if(typet == 0 && mgl.maxTur <=2){
                    printf("placed");
                    mgl.Tur[typet][mgl.maxTur].posx = cent[0]+30;
                    mgl.Tur[typet][mgl.maxTur].posy = yres-cent[1]-40;
                    printf("posx: %d, posy: %d ", mgl.Tur[typet][mgl.maxTur].posx,
                            mgl.Tur[typet][mgl.maxTur].posy);
                    mgl.Tur[typet][mgl.maxTur].type = typet;
                    mgl.maxTur++;
                    grid[i][j].status = 1;
                    grid[i][j].type = typet;
                }
                if(typet == 1 && mgl.maxTur2<=2){
                    printf("placed");
                    mgl.Tur[typet][mgl.maxTur2].posx = cent[0]+30;
                    mgl.Tur[typet][mgl.maxTur2].posy = yres-cent[1]-40;
                    printf("posx: %d, posy: %d ", mgl.Tur[typet][mgl.maxTur2].posx,
                            mgl.Tur[typet][mgl.maxTur2].posy);

                    mgl.Tur[typet][mgl.maxTur2].type = typet;
                    mgl.maxTur2++;
                    grid[i][j].status = 1;
                    grid[i][j].type = typet;
                }


            }
        }
    }
}
void renderTur(int yres)
{   
    int cent[2];
    int temp = 0;
    for (int i=0; i<18; i++) {
        for (int j=0; j<18; j++) {
            if (grid[i][j].status == 1 && grid[i][j].type==0) {
                glColor3f(1.0, 1.0, 1.0);
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.snsTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0]+5,yres-cent[1]-qsize2+10);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0]+5,yres-cent[1]-10);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize-5,yres-cent[1]-10);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize-5,yres-cent[1]-qsize2+10);
                glEnd();
            }    
            if (grid[i][j].status == 1 && grid[i][j].type==1) {
                get_grid_center(1,i,j,cent);
                //glBindTexture(GL_TEXTURE_2D, mgl.sewTex);
                glColor3f(0.2, 0.2, 0.2);
                //glBegin(GL_QUADS);
                glPushMatrix();
                glTranslatef(mgl.Tur[1][temp].posx,mgl.Tur[1][temp].posy,0.0f);
                glRotatef(mgl.Tur[1][temp].angle,0.0f,0.0f,1.0f);
                glBegin(GL_TRIANGLES);
                glVertex2f(-12.0f, -10.0f);
                glVertex2f(  0.0f, 20.0f);
                glVertex2f(  0.0f, -6.0f);
                glVertex2f(  0.0f, -6.0f);
                glVertex2f(  0.0f, 20.0f);
                glVertex2f( 12.0f, -10.0f);
                glEnd();
                glPopMatrix();
                temp++;
                
            }
        }
    }

}

double GetAngle(int xt, int xa, int yt, int ya)
{

    double difx =abs(abs(xt)-abs(xa)), dify=abs(abs(yt)-abs(ya));
    double beta, alpha;
    if(xt >= xa && yt <= ya) {

        beta = dify/2;
        alpha = 90 - beta;
    }
    else if(xt >= xa && yt >= ya) {

        beta = dify/2;
        alpha = 90 + beta;
    }
    else if(xt <= xa && yt >= ya) {

        beta = difx/2;
        alpha = 270 + beta;
        if(alpha == 360)
            alpha =0;
    }
    else if(xt <= xa && yt <= ya) {

        beta = dify/2;
        alpha = 270 - beta;
    }

    return alpha;

}

void UpdateTurColi(int ii, int jj, float angle)
{
    mgl.Tur[ii][jj].angle = angle;
}

bool checkColiTur(int xt, int xa, int yt, int ya, int r)
{
    int temp =mgl.maxTur;
        if(mgl.maxTur <= mgl.maxTur2) {
            temp = mgl.maxTur2;
        }
    for(int i =0; i < 2;i++) {
        for(int j = 0; j <temp;j++){ 
    if(mgl.Tur[i][j].type == 1){
    xt = mgl.Tur[i][j].posx;
    yt = mgl.Tur[i][j].posy;

    if( ((xt-xa)*(xt-xa)+(yt-ya)*(yt-ya)) < r*r) {
      //  printf("xt: %d, xa: %d, yt: %d, ya: %d",xt,xa,yt,ya);
        UpdateTurColi(i, j, GetAngle(xt,xa,yt,ya));
    }

        
}
}
} return true;
}
void MoathRend(int x, int y, Rect r)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    x = x/2;
    y = y/2;

    r.bot =y;
    r.left=x;
    r.center=0;

    ggprint16(&r, 16, 0xfff0, "Moath's File");

}
void ShowScores(Rect r)
{
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ifstream fin("Names.txt");

    char name[3];
    int score;
    if(fin.is_open()){ 
        while (fin >> name >> score)
        {
            ggprint8b(&r, 16, 0xfff0, name);
            ggprint8b(&r, 16, 0xfff0, "score:%i", score);

        }
        fin.close();}
}
void Mrender(int yres)
{
    //Rect r;
    int cent[2];

    for (int i=0; i<18; i++) {
        for (int j=0; j<18; j++) {
            if (mlev.arr[i][j] == 's' || mlev.arr[i][j] == '2') {
                glColor3f(1.0, 1.0, 1.0);
                //put tile in its place

                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.snsTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == '1') {
                glColor3f(1.0, 1.0, 1.0);
                //put tile in its place
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.sewTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == '5') {
                glColor3f(1.0, 1.0, 1.0);
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.cneTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == '6') {
                glColor3f(1.0, 1.0, 1.0);
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.cnwTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == '3') {
                glColor3f(1.0, 1.0, 1.0);
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.cseTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == '4') {
                glColor3f(1.0, 1.0, 1.0);
                get_grid_center(1,i,j,cent);
                glBindTexture(GL_TEXTURE_2D, mgl.cswTex);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
            if (mlev.arr[i][j] == 'b') {
                glColor3f(1.0, 1.0, 1.0);
                if(grid[i][j].hover){
                    glColor3f(0.0f,0.0f,1.0f);
                }
                glBindTexture(GL_TEXTURE_2D, mgl.groundTex);
                get_grid_center(1,i,j,cent);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(cent[0],yres-cent[1]-qsize2);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(cent[0],yres-cent[1]);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(cent[0]+qsize,yres-cent[1]-qsize2);
                glEnd();
            }
        }
    }
}
