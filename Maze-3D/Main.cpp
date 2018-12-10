#define _CRT_SECURE_NO_WARNINGS

#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <vector>
#include "Point2D.h"

using namespace std;

const int W = 600; // window width
const int H = 600; // window height
const int MAX_HEIGHT = 40;
const int SPACE = 1;
const int WALL = 2;
const int VISITED = 3;
const int START = 4;
const int TARGET = 5;
const int GRAY = 6;
const int PATH = 7;

const int MSIZE = 512;
const double SQSIZE = 2.0 / MSIZE;

int maze[MSIZE][MSIZE];
double ground[MSIZE][MSIZE];

Point2D* parent[MSIZE][MSIZE];

bool bfs_started = false, dfs_started = false;

// gray queue
vector <Point2D*> gray;


double eyex = 0, eyey = 250, eyez = 30;
double angle = 0;

void SetupMaze();

void LoadBitmap(char * filename)
{
	int sz;
	unsigned char *bmp;
	int i, j;

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	FILE* pf = fopen(filename, "rb"); // read binary file

	fread(&bf, sizeof(bf), 1, pf);
	fread(&bi, sizeof(bi), 1, pf);
	sz = bi.biHeight * bi.biWidth * 3;

	bmp = (unsigned char*)malloc(sz);

	fread(bmp, 1, sz, pf);
	// fill ground with heights

	for (i = 0; i<MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
		{
			ground[i][j] = (bmp[(i*MSIZE + j) * 3 + 0] / 256.0) * MAX_HEIGHT;
		}


	free(bmp);
	fclose(pf);
}


void init()
{
	int i, j;
	Point2D* pt;

	srand(time(0));

	// clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
			maze[i][j] = SPACE;

	SetupMaze();

	LoadBitmap("r5.bmp");

	maze[MSIZE / 2][MSIZE / 2] = START;
	maze[rand() % MSIZE][rand() % MSIZE] = TARGET;
	pt = new Point2D(MSIZE / 2, MSIZE / 2);
	// save the start in gray
	gray.push_back(pt);


	glClearColor(GLclampf(0.0), GLclampf(0.0), GLclampf(0.3), 0);

	glEnable(GL_DEPTH_TEST);
	//	glOrtho(-1, 1, -1, 1, -1, 1);
}

void SetupMaze()
{
	int i, j;

	for (i = 0; i < MSIZE; i++)
	{
		maze[i][0] = maze[i][MSIZE - 1] = WALL;
		maze[0][i] = maze[MSIZE - 1][i] = WALL;
	}
	// set walls
	for (i = 1; i < MSIZE - 1; i++)
		for (j = 1; j < MSIZE - 1; j++)
		{
			if (i % 2 == 0) // mostly walls
			{
				if (rand() % 100 < 50) maze[i][j] = WALL;
			}
			else // mostly spaces
			{
				if (rand() % 100 < 25) maze[i][j] = WALL;
			}
		}
}

void ShowPath(Point2D* pt)
{
	while (maze[pt->GetY()][pt->GetX()] != START)
	{
		maze[pt->GetY()][pt->GetX()] = PATH;
		pt = parent[pt->GetY()][pt->GetX()];
	}
}

void BfsIteration()
{
	Point2D* pt;
	Point2D* pt1;

	if (gray.empty())
	{
		bfs_started = false;// there is no path to the target
	}
	else // gray is not empty
	{
		pt = gray[0]; // this will be the parent
		gray.erase(gray.begin()); // dequeue

								  // paint pt VISITED
		if (maze[pt->GetY()][pt->GetX()] == TARGET) // we have found the target
		{
			bfs_started = false;
		}
		else
		{
			if (maze[pt->GetY()][pt->GetX()] != START)
				maze[pt->GetY()][pt->GetX()] = VISITED; // y is i, x is j!!! 
														// check non-visited neighbors
														// go up
			if (maze[pt->GetY() + 1][pt->GetX()] == TARGET)
			{
				bfs_started = false;

			}
			if (bfs_started && maze[pt->GetY() + 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() + 1][pt->GetX()] = GRAY;
				parent[pt->GetY() + 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go down
			if (maze[pt->GetY() - 1][pt->GetX()] == TARGET)
			{
				bfs_started = false;

			}
			if (bfs_started && maze[pt->GetY() - 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() - 1][pt->GetX()] = GRAY;
				parent[pt->GetY() - 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go right
			if (maze[pt->GetY()][pt->GetX() + 1] == TARGET)
			{
				bfs_started = false;

			}
			if (bfs_started && maze[pt->GetY()][pt->GetX() + 1] == SPACE)
			{ // add it to gray
				parent[pt->GetY()][pt->GetX() + 1] = pt;
				maze[pt->GetY()][pt->GetX() + 1] = GRAY;
				pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go left
			if (bfs_started && maze[pt->GetY()][pt->GetX() - 1] == TARGET)
			{
				bfs_started = false;

			}
			if (bfs_started && maze[pt->GetY()][pt->GetX() - 1] == SPACE)
			{ // add it to gray
				maze[pt->GetY()][pt->GetX() - 1] = GRAY;
				parent[pt->GetY()][pt->GetX() - 1] = pt;
				pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			if (!bfs_started) // target was found
				ShowPath(pt);
		}
	}
}

void DfsIteration()
{
	Point2D* pt;
	Point2D* pt1;

	if (gray.empty())
	{
		dfs_started = false;// there is no path to the target
	}
	else // gray is not empty
	{
		pt = gray[gray.size() - 1]; // this will be the parent
		gray.pop_back(); // pop

						 // paint pt VISITED
		if (maze[pt->GetY()][pt->GetX()] == TARGET) // we have found the target
		{
			dfs_started = false;
		}
		else
		{
			if (maze[pt->GetY()][pt->GetX()] != START)
				maze[pt->GetY()][pt->GetX()] = VISITED; // y is i, x is j!!! 
														// check non-visited neighbors
														// go up
			if (maze[pt->GetY() + 1][pt->GetX()] == TARGET)
			{
				dfs_started = false;

			}
			if (dfs_started && maze[pt->GetY() + 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() + 1][pt->GetX()] = GRAY;
				parent[pt->GetY() + 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go down
			if (maze[pt->GetY() - 1][pt->GetX()] == TARGET)
			{
				dfs_started = false;

			}
			if (dfs_started && maze[pt->GetY() - 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() - 1][pt->GetX()] = GRAY;
				parent[pt->GetY() - 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go right
			if (maze[pt->GetY()][pt->GetX() + 1] == TARGET)
			{
				dfs_started = false;

			}
			if (dfs_started && maze[pt->GetY()][pt->GetX() + 1] == SPACE)
			{ // add it to gray
				parent[pt->GetY()][pt->GetX() + 1] = pt;
				maze[pt->GetY()][pt->GetX() + 1] = GRAY;
				pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			// go left
			if (dfs_started && maze[pt->GetY()][pt->GetX() - 1] == TARGET)
			{
				dfs_started = false;

			}
			if (dfs_started && maze[pt->GetY()][pt->GetX() - 1] == SPACE)
			{ // add it to gray
				maze[pt->GetY()][pt->GetX() - 1] = GRAY;
				parent[pt->GetY()][pt->GetX() - 1] = pt;
				pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
				gray.push_back(pt1);
			}
			if (!dfs_started) // target was found
				ShowPath(pt);
		}
	}
}

void DrawMaze()
{
	int i, j;

	for (i = 0; i<MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
		{
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.4, 0, 0); // dark red;
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white;
				break;
			case VISITED:
				glColor3d(0, 0.9, 0); // green;
				break;
			case START:
				glColor3d(0, 0, 1); // blue;
				break;
			case TARGET:
				glColor3d(1, 0, 0); // RED;
				break;
			case GRAY:
				glColor3d(1, .8, 0); // ORANGE;
				break;
			case PATH:
				glColor3d(0.8, .5, 1); // ORANGE;
				break;

			}
			// draw square
			glBegin(GL_POLYGON);
			glVertex2d(j*SQSIZE - 1 - SQSIZE / 2, i*SQSIZE - 1 + SQSIZE / 2);
			glVertex2d(j*SQSIZE - 1 + SQSIZE / 2, i*SQSIZE - 1 + SQSIZE / 2);
			glVertex2d(j*SQSIZE - 1 + SQSIZE / 2, i*SQSIZE - 1 - SQSIZE / 2);
			glVertex2d(j*SQSIZE - 1 - SQSIZE / 2, i*SQSIZE - 1 - SQSIZE / 2);
			glEnd();
		}

}

void SetColor(int i, int j)
{
	double h = 0.4 + 0.6*ground[i][j] / MAX_HEIGHT;
	switch (maze[i][j])
	{
	case WALL:
		glColor3d(h*0.4, h*0.2, h * 0); // dark red;
		break;
	case SPACE:
		glColor3d(h*0.6, h* 0.6, h*0.6); // white;
		break;
	case VISITED:
		glColor3d(h * 0, h*0.7, h * 0); // green;
		break;
	case START:
		glColor3d(0, 0, 1); // blue;
		break;
	case TARGET:
		glColor3d(1, 0, 0); // RED;
		break;
	case GRAY:
		glColor3d(h*0.7, h*.7, h * 0); // ORANGE;
		break;
	case PATH:
		glColor3d(h*0.5, h * 0, h* 0.6); // MAGENTA;
		break;

	}

}

void Draw3DMaze()
{
	int i, j;

	for (i = 0; i<MSIZE - 1; i++)
		for (j = 0; j < MSIZE - 1; j++)
		{
			// draw square
			glBegin(GL_POLYGON);
			SetColor(i, j);
			glVertex3d(j - MSIZE / 2, ground[i][j], i - MSIZE / 2);
			SetColor(i, j + 1);
			glVertex3d(j + 1 - MSIZE / 2, ground[i][j + 1], i - MSIZE / 2);
			SetColor(i + 1, j + 1);
			glVertex3d(j + 1 - MSIZE / 2, ground[i + 1][j + 1], i + 1 - MSIZE / 2);
			SetColor(i + 1, j);
			glVertex3d(j - MSIZE / 2, ground[i + 1][j], i + 1 - MSIZE / 2);
			glEnd();
		}

}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 0.7, 800);
	gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotated(angle, 0, 1, 0);
	//	DrawMaze();
	Draw3DMaze();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	angle += 0.1;

	if (bfs_started)
		BfsIteration();
	if (dfs_started)
		DfsIteration();
	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	switch (choice)
	{
	case 1:
		bfs_started = true;
		break;
	case 2:
		dfs_started = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Maze 3D");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	init();

	glutCreateMenu(Menu);
	glutAddMenuEntry("BFS", 1);
	glutAddMenuEntry("DFS", 2);
	glutAddMenuEntry("A*", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutMainLoop();
}