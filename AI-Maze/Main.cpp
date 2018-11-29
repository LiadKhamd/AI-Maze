#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <vector>
#include "Point2D.h"
using namespace std;

const int W = 600; // window width
const int H = 600; // window height

const int SPACE = 1;
const int WALL = 2;
const int VISITED = 3;
const int START = 4;
const int TARGET = 5;
const int GRAY = 6;
const int PATH = 7;

const int MSIZE = 100;
const double SQSIZE = 2.0 / MSIZE;

int maze[MSIZE][MSIZE];
Point2D* parent[MSIZE][MSIZE];

bool bfs_started = false;

// gray queue
vector <Point2D*> gray;


void SetupMaze();

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

	maze[MSIZE / 2][MSIZE / 2] = START;
	maze[rand() % MSIZE][rand() % MSIZE] = TARGET;
	pt = new Point2D(MSIZE / 2, MSIZE / 2);
	// save the start in gray
	gray.push_back(pt);


	glClearColor(0.7, 0.7, 0.7, 0);

	glOrtho(-1, 1, -1, 1, -1, 1);
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



void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	DrawMaze();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	if (bfs_started)
		BfsIteration();
	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	switch (choice)
	{
	case 1:
		bfs_started = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Digits Example ");

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