// ConsoleApplication1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <GL/freeglut.h>
#include <thread>
#include <vector>
#include <string>

using namespace std;

struct datapoint
{
	float x, y;
};

bool work = true;
bool dataready = false;
bool showdatagen = true;
vector <datapoint> vdata;
float c1 = 10;
float c2 = 0.1;
float c3 = -15;
float x1 = -100;
float x2 = 100;
int fid = 0;
int prec = 1000;

float ymin, ymax;

float screenw, screenh;

bool needrefresh = false;

// Math

string getfunctextbyid(int id)
{
	if (id == 0) return "c1 * sin(c2*X)";
	if (id == 1) return "c1 * cos(c2*X)";
	if (id == 2) return "c1 * X^2 + c2*X + c3";
	if ((id <0) || (id>2)) return "c1*X+c2";
}


float calcfunc(int id, float c1, float c2, float c3, float x)
{
	switch (id)
	{
	case 0: return c1 * sin(c2*x);	break;
	case 1: return c1 * cos(c2*x); break;
	case 2: return c1 * pow(x, 2) + c2 * x + c3; break;
	default: return c1 * x + c2;  break;
	}
	
}

void makedataset(vector <datapoint> &dta, float x1, float x2, int prec, int cfid)
{
	float d = (x2 - x1) / prec;
	dta.clear();
	for (int i = 0; i <= prec; i++)
	{
		dta.push_back(datapoint());
		dta[i].x = x1 + (i*d);
		dta[i].y = calcfunc(cfid, c1,c2,c3, dta[i].x);
		if (showdatagen)
		printf("%d) [%f; %f] \n", i, dta[i].x, dta[i].y);
	}
}

void getextremes(float &min, float &max, vector <datapoint> dta)
{
	int l = dta.size();
	if (l>0)
	{
		float mn = dta[0].y;
		float mx = dta[0].y;
		for (int i = 0; i < l; i++)
		{
			if (dta[i].y<mn)
			{
				mn = dta[i].y;
			}
			if (dta[i].y>mx)
			{
				mx = dta[i].y;
			}
		}
		min = mn;
		max = mx;
	}
}


//CALLBACKS

void displayCB(void)		/* function called whenever redisplay needed */
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);		/* clear the display */
	glColor3f(1.0, 1.0, 1.0);		/* set current color to white */
	if (!dataready)
	{
		gluOrtho2D(-200, 200, -200, 200);
		float radius = 175;
		double twicePi = 2.0 * 3.142;
		float x = 0; float y = 0;
		//BEGIN CIRCLE
		for (int i = 0; i < 20; i++) {
			glBegin(GL_LINES);
				glVertex2f(
					(x + (radius * cos(i * twicePi / 20))), (y + (radius * sin(i * twicePi / 20)))
				);
				glVertex2f(
					(x + (radius * cos((i+1) * twicePi / 20))), (y + (radius * sin((i+1) * twicePi / 20)))
				);
			glEnd();
		}
		 //END
		
		glBegin(GL_POLYGON);			/* draw filled triangle */
			glVertex2i(-100, -100);			/* specify each vertex of triangle */
			glVertex2i(100, -100);
			glVertex2i(0, 100);
		glEnd();				/* OpenGL draws the filled triangle */

	}
	if (dataready)
	{
		int l = vdata.size();
		
		gluOrtho2D(0, x2-x1, 0, ymax-ymin);

		//rendering chart BG
		glColor3f(0.1, 0.1, 0.1);
		glBegin(GL_POLYGON);
			glColor3f(0.1, 0.1, 0.1);
			glVertex2f(0, 0);
			glColor3f(0.2, 0.0, 0.0);
			glVertex2f(0, ymax - ymin);
			glColor3f(0.2, 0.0, 0.0);
			glVertex2f(x2-x1, ymax-ymin);
			glColor3f(0.1, 0.1, 0.1);
			glVertex2f(x2 - x1, 0);
		glEnd();

		//rendering grid
		glColor3f(0, .5, 0);

		//VERTICAL
		int gdv = (int)(ymax - ymin);
		while (gdv > 20) gdv = (int)(gdv / 10);
		if (gdv != 0)
			while (gdv < 5) gdv = (int)(gdv * 10);
		else
		{
			gdv = 5;
		}
		float gd = (int)((ymax - ymin) / gdv);
		float cg = 0;

		while (cg < (ymax - ymin))
		{
			glBegin(GL_LINES);
			glVertex2f(0, cg);
			glVertex2f(x2 - x1, cg);
			glEnd();
			cg += gd;
			if (cg >= (ymax - ymin))
			{
				glBegin(GL_LINES);
				glVertex2f(0, ymax - ymin);
				glVertex2f(x2 - x1, ymax - ymin);
				glEnd();
			}
		}

		//HORIZONTAL
		gdv = (int)(x2 - x1);
		while (gdv > 20) gdv = (int)(gdv / 2);
		if (gdv != 0)
			while (gdv < 5) gdv = (int)(gdv * 10);
		else
		{
			gdv = 5;
		}
		gd = (int)((x2 - x1) / gdv);
		cg = 0;

		while (cg < (x2 - x1))
		{
			glBegin(GL_LINES);
			glVertex2f(cg, 0);
			glVertex2f(cg, ymax - ymin);
			glEnd();
			cg += gd;
			if (cg >= (x2 - x1))
			{
				glBegin(GL_LINES);
				glVertex2f(x2 - x1, 0);
				glVertex2f(x2 - x1, ymax - ymin);
				glEnd();
			}
		}

		//rendering chart
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		for (int i = 1; i < l; i++)
		{
			if (dataready)
			glVertex2f(vdata[i-1].x-x1,vdata[i-1].y-ymin);
			if (dataready)
			glVertex2f(vdata[i].x-x1, vdata[i].y-ymin);
		}
		glEnd();

		//rendering scale
		glColor3f(1, 0, 0);
		//horizontal
		glBegin(GL_LINES);
			glVertex2f(0, 0-ymin);
			glVertex2f(x2-x1, 0-ymin);
		glEnd();
	
		//vertical
		glBegin(GL_LINES);
			glVertex2f(-x1, 0);
			glVertex2f(-x1, ymax-ymin);
		glEnd();
			   
	}
	glFlush();				/* Complete any pending operations */
	glutSwapBuffers();
}

void idleCB()
{
	//provides redraw call when explicitly required by needrefresh bool flag
	if (needrefresh) {
		displayCB();
		needrefresh = false;
	}

}

void reshapeCB(int wx, int wy)
{
	screenw = wx; screenh = wy;
	glViewport(0, 0, wx, wy);
}

void keyCB(unsigned char key, int x, int y)	/* called on key press */
{
	if (key == 'q') exit(0);
}

// THREADS

void mainthreadfunc()
{
	printf("Main Thread Started. \n");
	while (work)
	{
		
		string sdg = "no";
		if (showdatagen) sdg = "yes";

		printf("Current Status:\n Function %s. C1=%f, C2=%f, C3=%f.\n   Range X1..X2 = [%f .. %f]. Precision = %d. Log: %s\n", (getfunctextbyid(fid)).c_str(), c1, c2, c3, x1, x2,prec,sdg.c_str());
		printf("Action Request. \n1 - Make Dataset\n2 - Quit\n 3 - Input C1 \n 4 - Input C2 \n 5 - Input C3 \n 6 - Input X1\n 7 - Input X2 \n 8 - Input PREC \n 9 - Select Function Type \n 10 - Toggle data logging\n 11 - Force refresh\n\n");
		
		string k = "0";
		getline(cin, k);
		if (k == "1")
		{
			dataready = false;
			needrefresh = true;
			makedataset(vdata,x1,x2,prec,fid);
			getextremes(ymin,ymax,vdata);
			printf("Dataset min = %f, dataset max = %f \n",ymin,ymax);
			dataready = true;
			printf("Dataset is ready and should be visualized\n");
			needrefresh = true;
		}
		if (k == "2")
		{
			dataready = false;
			work = false;
			exit(0);
		}
		if (k == "9")
		{
			printf("Input function type id\n");
			for (int i = 0; i < 4; i++)
			{
				printf(" %d - %s\n", i, getfunctextbyid(i).c_str());
			}
			getline(cin, k);
			fid = atoi(k.c_str());
			needrefresh = true;
		}
		if (k == "3")
		{
			printf("Input c1\n");
			getline(cin, k);
			c1 = atof(k.c_str());
			needrefresh = true;
		}
		if (k == "4")
		{
			printf("Input c2\n");
			getline(cin, k);
			c2 = atof(k.c_str());
			needrefresh = true;
		}
		if (k == "5")
		{
			printf("Input c3\n");
			getline(cin, k);
			c3 = atof(k.c_str());
			needrefresh = true;
		}
		if (k == "6")
		{
			printf("Input x1\n");
			getline(cin, k);
			x1 = atof(k.c_str());
			needrefresh = true;
		}
		if (k == "7")
		{
			printf("Input x2\n");
			getline(cin, k);
			x2 = atof(k.c_str());
			needrefresh = true;
		}
		if (k == "8")
		{
			printf("Input amount of values to generate\n");
			getline(cin, k);
			prec = atoi(k.c_str());
			needrefresh = true;
		}
		if (k == "10")
		{
			if (showdatagen)
			{
				showdatagen = false;
				printf("Data generation log disabled\n");
			}
			else
			{
				showdatagen = true;
				printf("Data generation log enabled\n");
			}
		}
		if (k == "11")
		{
			printf("Force requesting refresh\n");
			needrefresh = true;
		}

	}
}

void glutthreadfunc()
{

	printf("GLUT Thread Started. \n");

	int win;
	
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 480);		/* width=640pixels height=480pixels */
	win = glutCreateWindow("Chart Display");	/* create window */

	/* from this point on the current window is win */

	glClearColor(0.0, 0.0, 0.0, 0.0);	/* set background to black */
	gluOrtho2D(0, 640, 0, 480);		/* how object is mapped to window */
	glutIdleFunc(idleCB);
	glutDisplayFunc(displayCB);		/* set window's display callback */
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyCB);		/* set window's key callback */

	glutMainLoop();			/* start processing events... */
}

// MAIN

int main(int argc, char *argv[])
{
	
	glutInit(&argc, argv);		/* initialize GLUT system */

	thread mainthr(mainthreadfunc);
	thread glutthr(glutthreadfunc);

	mainthr.join();
	glutthr.join();

	return 0;
}