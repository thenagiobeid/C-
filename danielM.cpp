#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"

//Daniel McArthur

extern void showNagi(int x, int y);
extern void MoathRend(int x, int y, Rect r);

void danielCredits(int x, int y)
{
	Rect r;
	r.bot = y;
	r.left = x;
	r.center=0;

	ggprint16(&r, 16, 0xfff00fff, "DANIEL MCARTHUR");
	ggprint16(&r, 16, 0xffffffff, "something i did");
	ggprint16(&r, 16, 0xffffffff, "something else i did");
}

void doPrintCredits(int xres, int yres)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	int xCenter = xres/2;
	int yCenter = yres/2 ;
	danielCredits(xCenter, yCenter);
	showNagi(xCenter, yCenter+60);
	Rect r;
	MoathRend(xCenter, yCenter+84, r);
}
//------------------------------------------------------------------------------
//----------------------TURRET RELATED FUNCTIONS--------------------------------
//------------------------------------------------------------------------------

/*
void findClosestTarget(int turretX, int turretY)//can I return a struct instead?
{
	int numEnemiesOnMap,closestTarget;
	float distance;
	float smallestDistance = 2*hypot(xres,yres);
	for (int i = 0; i<numEnemiesOnMap; i++) {
		distance = calcLinearDistance(int x, int y);
		if (smallestDistance > distance) {
			smallestDistance = distance;
			closestTarget = i;
		}
	}
}

void aimAtTarget(int turretX, int turretY, int targetX, int targetY)
{
	//for now just do linear targeting
	//function should be passed the objects and then pick out the
	//coordinates from the objects
	int rise = targetY - turretY;
	int run = targetX - turretY;
	float aimAngle = atan(rise/run); //i believe this is in degrees


}

float calcLinearDistance(int a, int b) //needs passed same info as aimAtTarget
{	float rise = abs(targetY - turretY); //gotta cast to float probably
	float run = abs(targetX - turretY);
	float distance = hypot(rise, run);
	return distance;
}
class Tower {//derived from Ship class, try to get this to run from my source
public:
        Vec dir;
        Vec pos;
        float angle;
        float angleVel;
        float color[3];
public:
        Tower() {
                VecZero(dir);
                pos[0] = (Flt)(gl.xres/2);
                pos[1] = (Flt)(gl.yres/2);
                pos[2] = 0.0f;
                angle = 0.0;
                color[0] = color[1] = color[2] = 1.0;
        }
};

*/

