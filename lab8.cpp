//
//modified by: Hassen Hassen 
//
//Develope a project from this framework
//
//program: lab8.cpp
//author:  Gordon Griesel
//date:    Fall 2018
//
//An OpenGL 3D framework for students.
//
//
//
#include <unistd.h>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "fonts.h"
#include <GL/glut.h>

typedef float Flt;
typedef Flt Vec[3];
typedef Flt	Matrix[4][4];
//some macros
#define MakeVector(x, y, z, v) (v)[0]=(x);(v)[1]=(y);(v)[2]=(z)
#define VecCross(a,b,c) (c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1];\
                        (c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2];\
                        (c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0]
#define rnd() (Flt)rand() / (Flt)RAND_MAX
//some constants

//-----------------------------------------------------------------------------
//Setup timers
const double OOBILLION = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------

GLfloat triVert[] = {
    //0, 100, 0,
    //100, 200, 0,
    //500, 50, 0
    
    //-1.0, -1.0,  1.0,
	//-1.0,  1.0,  1.0,
	//1.0,  1.0,  1.0,



	0.0f, 0.0f, 0.0f, 
	0.5f, 0.0f, 0.0f,
	0.0f, 2.0f, 0.0f,
	
	0.5f, 0.0f, 0.0f, 
    0.5f, 2.0f, 0.0f,
	0.0f, 2.0f, 0.0f,


    0.0f, 2.0f, 0.0f,
    0.5f, 2.0f, 0.0f,
	0.0f, 3.0f, 0.0f,
 //going down the 
    0.0f, 3.0f, 0.0f,
    0.5f, 2.0f, 0.0f,
	0.5f, 3.0f, 0.0f,

    //top of triangle
    0.0f, 3.0f, 0.0f,
    0.5f, 3.0f, 0.0f,
	0.5f, 4.0f, 0.0f


    /*
    10.0f, 15.0f, 
	10.0f, 10.0f,
	10.0f, 100.0f

*/
};


class Global {
public:
	int xres, yres;
	Flt aspectRatio;
	Vec cameraPos;
	Flt cameraAngle;
	int circling;
	int sorting;
	int billboarding;
	GLuint img;
	float xRotate, yRotate, zRotate;
	float xRotate2, yRotate2, zRotate2;
	double innerRadius, outterRadius;
	int sides;
	float angle;
	GLint rings;
	float xRot;
	bool post;
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
	GLfloat lightPosition[4];
    GLfloat shine[];
	~Global() {
	}
	Global() {
		//constructor
        GLfloat la[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
        GLfloat ld[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
        GLfloat ls[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
        GLfloat lp[] = { 100.0f, 60.0f, -140.0f, 1.0f };
        memcpy(lightAmbient, la, sizeof(GLfloat)*4);
        memcpy(lightDiffuse, ld, sizeof(GLfloat)*4);
        memcpy(lightSpecular, ls, sizeof(GLfloat)*4);
        memcpy(lightPosition, lp, sizeof(GLfloat)*4);
		xres = 640;
		yres = 480;
		aspectRatio = (GLfloat)xres / (GLfloat)yres;
		MakeVector(0.0, 15.0, 75.0, cameraPos);
		cameraAngle = 0.0;
		circling = 0;
		sorting = 0;
		innerRadius = 0.5;
		outterRadius = 1.9;
		sides = 25;
		rings = 25;
		//light is high, right, toward
		MakeVector(100.0f, 1240.0f, 40.0f, lightPosition);
		lightPosition[3] = 1.0f;
		//clock_gettime(CLOCK_REALTIME, &smokeStart);
		post = true;
		xRot = 0.0;
        shine[0] = 51.2f;
	}
} g;
/*
class Image {
    public:
        int width, height;
        unsigned char *data;
        ~Image() {delete [] data;}
        Image(const char *fname) {
            int slen = strlen(fname);
            if(slen <= 0)
                return;
            int ppmFlag = 0;
            char name[40]; 
            strcpy(name, fname);
            char ppmname[80];
            if(strncmp(name+(slen-4), ".ppm", 4) == 0) {
                ppmFlag = 1;
                strcpy(ppmname, name);
            } else {
                name[slen-4] = '\0';
                sprintf(ppmname, "%s.ppm", name);
                char ts[100];
                sprintf(ppmname, "%s %s", fname, ppmname);
                system(ts);
            }
            FILE *fpi = fopen(ppmname, "r");
            if(fpi) {
                char line[200];
                fgets(line, 200, fpi);
                fgets(line, 200, fpi);
                
                while(line[0] == '#' || strlen(line) < 2)
                    fgets(line, 200, fpi);
                sscanf(line, "%i %i", &width, &height);
                fgets(line, 200, fpi);

                int n = width * height * 3;
                data = new unsigned char[n];
                    for(int i = 0; i < n; i++)
                        data[i] = fgetc(fpi);
                fclose(fpi);
            } else {
                printf("error opening image: %s\n", ppmname);
                exit(0);
            }
            if(!ppmFlag)
                unlink(ppmname);
        }
} img = "img1.jpg";
*/







class Image {
     public:
         int width, height;
         unsigned char *data;
         ~Image() { delete [] data; }
         Image(const char *fname) {
             int slen = strlen(fname);
             if(slen <= 0)
                 return;
             int ppmFlag = 0;
             char name[40];
             strcpy(name, fname);
             char ppmname[80];
             if(strncmp(name+(slen-4), ".ppm", 4) == 0 ) {
                 ppmFlag = 1;
                 strcpy(ppmname, name);
             } else {
                 name[slen-4] = '\0';
                 sprintf(ppmname, "%s.ppm", name);
                 char ts[100];
                 sprintf(ts, "convert %s %s", fname, ppmname);
                 system(ts);
             }
             FILE *fpi = fopen(ppmname, "r");
             if(fpi) {
                 char line[200];
                 fgets(line, 200, fpi);
                 fgets(line, 200, fpi);
                 //skip comments and blank lines
                 while(line[0] == '#' || strlen(line) < 2)
                         fgets(line, 200, fpi);
                 sscanf(line, "%i %i", &width, &height);
                 fgets(line, 200, fpi);
                 //get pixel data
                 int n = width * height * 3;
                 data = new unsigned char[n];
                 for(int i = 0; i < n; i++)
                     data[i] = fgetc(fpi);
                 fclose(fpi);
             } else {
                 printf("ERRIR opening Image: %s\n", ppmname);
                 exit(0);
             }
             if(!ppmFlag)
                unlink(ppmname);
         }
} img = "img1.jpg";











//X Windows wrapper class
class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	X11_wrapper() {
	    //Look here for information on XVisualInfo parameters.
	    //http://www.talisman.org/opengl-1.1/Reference/glXChooseVisual.html
	    //
	    GLint att[] = { GLX_RGBA,
			GLX_STENCIL_SIZE, 2,
			GLX_DEPTH_SIZE, 24,
			GLX_DOUBLEBUFFER, None };
	    XSetWindowAttributes swa;
	    setup_screen_res(g.xres, g.yres);
	    dpy = XOpenDisplay(NULL);
	    if (dpy == NULL) {
			printf("\ncannot connect to X server\n\n");
			exit(EXIT_FAILURE);
	    }
	    Window root = DefaultRootWindow(dpy);
	    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	    if (vi == NULL) {
			printf("\nno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
	    }
	    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	    swa.colormap = cmap;
	    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			StructureNotifyMask | SubstructureNotifyMask;
	    win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	    set_title("3480 OpenGL");
	    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	    glXMakeCurrent(dpy, win, glc);
	}
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void set_title(const char *str) {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, str);
	}
	void setup_screen_res(const int w, const int h) {
		g.xres = w;
		g.yres = h;
		g.aspectRatio = (GLfloat)g.xres / (GLfloat)g.yres;
	}
	void reshape_window(int width, int height) {
		//window has been resized.
		setup_screen_res(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
	}
	void check_resize(XEvent *e) {
		//The ConfigureNotify is sent by the server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != g.xres || xce.height != g.yres) {
			//Window size did change.
			reshape_window(xce.width, xce.height);
		}
	}
	bool getXPending() {
		return XPending(dpy);
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
} x11;


void init_opengl();
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();
void displayTorus();

int main(int argc, char **argv)
{
	srand((unsigned)time(NULL));
	init_opengl();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
	}
	cleanup_fonts();
	return 0;
}

void init()
{

}

void init_opengl()
{
	//OpenGL initialization
	glClearColor(0.5f, 0.6f, 1.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, g.aspectRatio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	//Enable this so material colors are the same as vert colors.
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, g.lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, g.lightDiffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, g.shine);
    glLightfv(GL_LIGHT0, GL_SPECULAR, g.lightSpecular);
	//Turn on a light
	//glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);
	glEnable(GL_LIGHT0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	//
	//Test the stencil buffer on this computer.
	//
	// https://www.opengl.org/discussion_boards/showthread.php/
	// 138452-stencil-buffer-works-on-one-machine-but-not-on-another
	//
	// Before you try using stencil buffer try this:
	// Code :
	// GLint stencilBits = 0;
	// glGetIntegerv(GL_STENCIL_BITS, &amp;stencilBits);
	// if (stencilBits < 1)
	//    MessageBox(NULL,"no stencil buffer.\n","Stencil test", MB_OK);
	GLint stencilBits = 0;
	glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
	if (stencilBits < 1) {
		printf("No stencil buffer on this computer.\n");
		printf("Exiting program.\n");
		exit(0);
	}
	//
	//Setup a texture map for the floor surface
	//
    /*
	unsigned char data[] = {
		255,255,0, 240,220,0, 255,255,0, 240,220,0,
	   	240,220,0, 255,255,0, 240,220,0, 255,255,0,
		255,255,0, 240,220,0, 255,255,0, 240,220,0,
	   	240,220,0, 255,255,0, 240,220,0, 255,255,0};
        */
	glGenTextures(1, &g.img);
	int w = img.width;
	int h = img.height;
	glBindTexture(GL_TEXTURE_2D, g.img);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
		GL_RGB, GL_UNSIGNED_BYTE, img.data);
        

}

void displayTri(void)
{
   /* 
    //float i = 1.0; 
    glPushMatrix();
    glTranslatef(1.0, 0.0, 1.0);
    //rotation kinda works
    glRotatef(g.xRot, 0.0f, 1.0f, 1.0f);
    
    if( g.post)
      g.xRot++;
    else
      g.xRot--;

    if(g.xRot == 70.0)
      g.post = false;
    if(g.xRot == -70.0)
      g.post = true;  
    
    
    glColor3f(1.0, 0.8, 0.1);
	glColor3ub(0,255,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
*/
    
    if( g.post)
      g.xRot+= 1.0;
    else
      g.xRot-= 1.0;

    if(g.xRot == 70.0)
      g.post = false;
    if(g.xRot == -70.0)
      g.post = true;  
    

    float a = -1.0;
    
    
    for(float i = 0; i < 20; i+=0.5) {
        for(float j = 0; j < 20; j+=0.5) {
    glPushMatrix();
    glTranslatef(j, 0.0, i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    
    for(float i = 0; i < 20; i+=0.5) {
        for(float j = 0; j < 20; j+=0.5) {
    glPushMatrix();
    glTranslatef(-j, 0.0, i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }

    
    for(float i = 0; i < 20; i+=0.5) {
        for(float j = 0; j < 20; j+=0.5) {
    glPushMatrix();
    glTranslatef(-j, 0.0, i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
     
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }
    //glColor3ub(255,255,255);
    glColor3ub(0,255*0.6,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }

    for(float i = 0; i < 20; i+=0.5) {
        for(float j = 0; j < 20; j+=0.5) {
    glPushMatrix();
    glTranslatef(j, 0.0, -i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
     
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }
    //glColor3ub(255,255,255);
    glColor3ub(0,255*0.6,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }

   /* 
    for(float i = 0; i < 20; i+=0.7) {
        for(float j = 0; j < 20; j+=0.7) {
    glPushMatrix();
    glTranslatef(j, 0.0, -i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(255, 0, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 0, 255);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    } 
    //glColor3ub(255,255,255);
    glColor3ub(0,255*0.6,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    */
    
    for(float i = 0; i < 20; i+=0.7) {
        for(float j = 0; j < 20; j+=0.7) {
    glPushMatrix();
    glTranslatef(-j, 0.0, -i);
    //glColor3f(1.0, 0.8, 0.1);
	
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }
    //glColor3ub(255,255,255);
    glColor3ub(0,255*0.6,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    
   /* 
    for(int i = 0; i < 20; i++) {
        for(int j = 0; j > -20; j--) {
    glPushMatrix();
    glTranslatef(j , 0.0, i);
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    
    if( (int)j % 2 == 0 && (int)i % 2 == 0) { 
    glColor3ub(0, 100, 0);
	}

    else if( (int)j % 2 != 0 && (int)i % 2 == 0) { 
	
    glColor3ub(0, 128, 0);
    }

    else if( (int)j % 2 != 0 && (int)i % 2 != 0) { 
	
    glColor3ub(0, 255, 0);
	}

    
    else if( (int)j % 2 == 0 && (int)i % 2 != 0) { 
	
    glColor3ub(34, 139, 34);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 15);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    */
    /*
    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < 20; j++) {
    glPushMatrix();
    glTranslatef(j , 0.0, -i);
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < 20; j++) {
    glPushMatrix();
    glTranslatef(-j , 0.0, -i);
    glRotatef(-g.xRot * a, 0.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triVert);
    //change the value to mess with more Verti
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    a *= -1.0;
      }
    }
    */
}


void displayTorus()
{
	glTranslatef(0.0, 8.0, 0.0);
	glColor3f(1.0, 0.8, 0.1);
	
	Flt rad = (g.xRotate/180.0) * 50.0;
	glRotatef(g.xRotate, 1.f, 0.f, 0.f);
	glRotatef(g.yRotate, 0.f, cos(rad),-sin(rad));
	g.yRotate += 2.0;

	glScalef(2.0, 2.0, 2.0);
	//g.angle += 1.0;
	//glutPostRedisplay();
	glutSolidTorus(g.innerRadius, g.outterRadius, g.sides, g.rings);
    //GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
    //glMaterialfv(GL_FRONT, GL_AMBIENT, black);    

	glFlush();


}


void check_mouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
}

int check_keys(XEvent *e)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		switch(key) {
			case XK_r:
				break;
			case XK_1:
				//camera front
				MakeVector(0.0, 12.5, 42.1, g.cameraPos);
				g.circling = 0;
				break;
			case XK_2:
				//camera high
				MakeVector(0.0, 65.0, 15.0, g.cameraPos);
				g.circling = 0;
				break;
			case XK_3:
				//camera right
				MakeVector(50.0, 15.0, 3.0, g.cameraPos);
				g.circling = 0;
				break;
			case XK_4:
				//camera is circling
				g.cameraAngle = 3.145 * 1.5;
				g.circling = 1;
				break;
			case XK_s:
				g.sorting ^= 1;
				break;
			case XK_b:
				g.billboarding ^= 1;
				break;
            case XK_l:
                g.lightPosition[0] = rnd() * 200.0 - 100.0;    
			case XK_Escape:
				return 1;
		}
	}
	return 0;
}

void drawBox(float w1, float h1, float d1)
{
	//
	//   1================2
	//   |\              /|
	//   | \            / |
	//   |  \          /  |
	//   |   \        /   |
	//   |    5------6    |
	//   |    |      |    |
	//   |    |      |    |
	//   |    |      |    |
	//   |    4------7    |
	//   |   /        \   |
	//   |  /          \  |
	//   | /            \ |
	//   |/              \|
	//   0================3
	//
	const Flt vert[][3] = {
	-1.0, -1.0,  1.0,
	-1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,
	 1.0, -1.0,  1.0,
	-1.0, -1.0, -1.0,
	-1.0,  1.0, -1.0,
	 1.0,  1.0, -1.0,
	 1.0, -1.0, -1.0 };
	//left,top,right,bottom,front,back.
	const int face[][4] = {
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	0,4,7,3,
	2,1,0,3,
	4,5,6,7 };
	const Flt norm[][3] = {
	-1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	1.0, 0.0, 0.0,
	0.0,-1.0, 0.0,
	0.0, 0.0, 1.0,
	0.0, 0.0,-1.0 };
	//half the width from center.
	Flt w = w1 * 0.5;
	Flt d = d1 * 0.5;
	Flt h = h1 * 0.5;
	//Normals are required for any lighting.
	glBegin(GL_QUADS);
		for (int i=0; i<6; i++) {
			glNormal3fv(norm[i]);
			for (int j=0; j<4; j++) {
				int k = face[i][j];
				glVertex3f(vert[k][0]*w, vert[k][1]*h, vert[k][2]*d);
			}
		}
	glEnd();
}

void drawFloor()
{
	//Draw a floor surface.
	//#define FLOOR_TEX
	//#ifdef FLOOR_TEX
	//FLOOR-------------------------------------------------
	glColor3ub(34,139,34);
	Flt w = 40.0 * 0.5;
	Flt d = 40.0 * 0.5;
	Flt h = 0.0;
	
	//glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		//top
		glNormal3f(0.0f, 1.0f, 0.0f);
		//glTexCoord2i( 0,  0); 
        glVertex3f(-w, h, -d);
		//glTexCoord2i( 1,  0); 
        glVertex3f( w, h, -d);
		//glTexCoord2i( 1,  1); 
        glVertex3f( w, h,  d);
		//glTexCoord2i( 0,  1); 
        glVertex3f(-w, h,  d);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, 0);
	
}

void wall()
{
	glColor3ub(255,255,255);
    Flt w = 40.0 * 0.5;
	Flt d = -5.0;
	

	glBindTexture(GL_TEXTURE_2D, g.img);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);	
	
    glTexCoord2f(1, 0);
	glVertex3f(-w, 25, d);

	glTexCoord2f(1, 1);
	glVertex3f(-w, 0, d);
	
	glTexCoord2f(0, 1);
	glVertex3f(w, 0, d);
	
	glTexCoord2f(0, 0);
	glVertex3f(w, 25, d);
	
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//------------------------------------------------------
	
	//#else //FLOOR_TEX
	/*
	glColor3f(0.9f, 0.9f, 0.1f);
	Flt w = 40.0 * 0.5;
	Flt d = 40.0 * 0.5;
	Flt h = 0.0;
	glBegin(GL_QUADS);
		//top
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-w, h, -d);
		glVertex3f( w, h, -d);
		glVertex3f( w, h,  d);
		glVertex3f(-w, h,  d);
	glEnd();
	#endif //FLOOR_TEX
	*/
	//
	//Draw colored corner posts.
	/*
	unsigned char c[] = {255,0,0, 0,255,0, 0,0,255, 255,255,255};
	for (int i=0; i<4; i++) {
		glPushMatrix();
		glColor3ubv(c+(i*3));
		glRotatef(90.0*(Flt)i, 0.0, 1.0, 0.0);
		glTranslatef(20.0, -0.9, 20.0);
		drawBox(4, 8, 4);
		glPopMatrix();
	}*/
}
/*
void make_view_matrix(const Vec p1, const Vec p2, Matrix m)
{
    //Line between p1 and p2 form a LOS Line-of-sight.
    //A rotation matrix is built to transform objects to this LOS.
    //Diana Gruber  http://www.makegames.com/3Drotation/
    m[0][0]=m[1][1]=m[2][2]=1.0f;
    m[0][1]=m[0][2]=m[1][0]=m[1][2]=m[2][0]=m[2][1]=0.0f;
    Vec out = { p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2] };
    //
    Flt l1, len = out[0]*out[0] + out[1]*out[1] + out[2]*out[2];
    if (len == 0.0f) {
        MakeVector(0.0f,0.0f,1.0f,out);
    } else {
        l1 = 1.0f / sqrtf(len);
        out[0] *= l1;
        out[1] *= l1;
        out[2] *= l1;
    }
    m[2][0] = out[0];
    m[2][1] = out[1];
    m[2][2] = out[2];
    Vec up = { -out[1] * out[0], upv[1] - out[1] * out[1], -out[1] * out[2] };
    //
    len = up[0]*up[0] + up[1]*up[1] + up[2]*up[2];
    if (len == 0.0f) {
        MakeVector(0.0f,0.0f,1.0f,up);
    }
    else {
        l1 = 1.0f / sqrtf(len);
        up[0] *= l1;
        up[1] *= l1;
        up[2] *= l1;
    }
    m[1][0] = up[0];
    m[1][1] = up[1];
    m[1][2] = up[2];
    //make left vector.
    VecCross(up, out, m[0]);
}
*/

void vecNormalize(Vec v)
{
	Flt len = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	if (len == 0.0)
		return;
	len = 1.0 / sqrt(len);
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

void vecScale(Vec v, Flt s)
{
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
}

/*
void drawSmoke()
{
	if (g.sorting) {


	}
	//
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i=0; i<g.nsmokes; i++) {
		glPushMatrix();
		if (g.billboarding) {


		}
		glTranslatef(g.smoke[i].pos[0],g.smoke[i].pos[1],g.smoke[i].pos[2]);
		glColor4ub(255, 0, 0, (unsigned char)g.smoke[i].alpha);
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0, 0.0, 1.0);
		for (int j=0; j<g.smoke[i].n; j++) {
			//each vertex of the smoke
			//glVertex3fv(g.smoke[i].vert[j]);
			vecNormalize(g.smoke[i].vert[j]);
			vecScale(g.smoke[i].vert[j], g.smoke[i].radius);
			glVertex3fv(g.smoke[i].vert[j]);
		}
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_BLEND);
}

void make_a_smoke()
{
	if (g.nsmokes < MAX_SMOKES) {
		Smoke *s = &g.smoke[g.nsmokes];
		s->pos[0] = rnd() * 5.0 - 2.5;
		s->pos[2] = rnd() * 5.0 - 2.5;
		s->pos[1] = rnd() * 0.1 + 0.1;
		s->radius = rnd() * 1.0 + 0.5;
		s->n = rand() % 5 + 5;
		Flt angle = 0.0;
		Flt inc = (PI*2.0) / (Flt)s->n;
		for (int i=0; i<s->n; i++) {
			s->vert[i][0] = cos(angle) * s->radius;
			s->vert[i][1] = sin(angle) * s->radius;
			s->vert[i][2] = 0.0;
			angle += inc;
		}
		s->maxtime = 8.0;
		s->alpha = 100.0;
		clock_gettime(CLOCK_REALTIME, &s->tstart);
		++g.nsmokes;
	}
}
*/
void physics()
{
    /*
	clock_gettime(CLOCK_REALTIME, &g.smokeTime);
	double d = timeDiff(&g.smokeStart, &g.smokeTime);
	if (d > 0.05) {
		//time to make another smoke particle
		make_a_smoke();
		timeCopy(&g.smokeStart, &g.smokeTime);
	}
	//move smoke particles
	for (int i=0; i<g.nsmokes; i++) {
		//smoke rising
		g.smoke[i].pos[1] += 0.015;
		g.smoke[i].pos[1] += ((g.smoke[i].pos[1]*0.24) * (rnd() * 0.075));
		//expand particle as it rises
		g.smoke[i].radius += g.smoke[i].pos[1]*0.002;
		//wind might blow particle
		if (g.smoke[i].pos[1] > 10.0) {
			g.smoke[i].pos[0] -= rnd() * 0.1;
		}
	}
	//check for smoke out of time
	int i=0;
	while (i < g.nsmokes) {
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double d = timeDiff(&g.smoke[i].tstart, &bt);
		if (d > g.smoke[i].maxtime - 3.0) {
			g.smoke[i].alpha *= 0.95;
			if (g.smoke[i].alpha < 1.0)
				g.smoke[i].alpha = 1.0;
		}
		if (d > g.smoke[i].maxtime) {
			//delete this smoke
			--g.nsmokes;
			g.smoke[i] = g.smoke[g.nsmokes];
			continue;
		}
		++i;
	}
    */
	//
	if (g.circling) {
		Flt rad = 80 + sin(g.cameraAngle) * 50.0;
		Flt x = cos(g.cameraAngle) * rad;
		Flt z = sin(g.cameraAngle) * rad;
		Flt y = 25.0;
		MakeVector(x, y, z, g.cameraPos);
		g.cameraAngle -= 0.01;
	}
}

void render()
{
	//Clear the depth buffer and screen.
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//
	//Render a 3D scene
	//
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	gluPerspective(45.0f, g.aspectRatio, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		g.cameraPos[0], g.cameraPos[1], g.cameraPos[2],
		0,12,0,
		0,1,0);
	glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);
	//
	drawFloor();
	wall();
	displayTri();
	displayTorus();
	//
	//switch to 2D mode
	//
	Rect r;
	glViewport(0, 0, g.xres, g.yres);
	glMatrixMode(GL_MODELVIEW);   glLoadIdentity();
	glMatrixMode (GL_PROJECTION); glLoadIdentity();
	gluOrtho2D(0, g.xres, 0, g.yres);
	glDisable(GL_LIGHTING);
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00000000, "1 - Camera front");
	ggprint8b(&r, 16, 0x00000000, "2 - Camera high");
	ggprint8b(&r, 16, 0x00000000, "3 - Camera right");
	ggprint8b(&r, 16, 0x00000000, "4 - Camera circling");
	//ggprint8b(&r, 16, 0x00000000, "nsmokes: %i", g.nsmokes);
}



