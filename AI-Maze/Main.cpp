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
const int VISITED_FROM_END_TO_START = 8;
const int GRAY_FROM_END = 9;

const int MSIZE = 100;
const double SQSIZE = 2.0 / MSIZE;

int maze[MSIZE][MSIZE];
Point2D* parent[MSIZE][MSIZE];
Point2D* startPoint, *endPoint;

bool bfs_started = false, dfs_started = false, bfs_started_start_end = false, a_star = false;

// gray queue
vector <Point2D*> grayFromStart;
vector <Point2D*> grayFromEnd;

void SetupMaze();

void init()
{
	int i, j;

	srand(time(0));

	// clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
			maze[i][j] = SPACE;

	SetupMaze();

	startPoint = new Point2D(MSIZE / 2, MSIZE / 2);
	maze[startPoint->GetY()][startPoint->GetX()] = START;

	//endPoint
	endPoint = new Point2D(rand() % MSIZE, rand() % MSIZE);
	maze[endPoint->GetY()][endPoint->GetX()] = TARGET;

	// save the start in gray
	grayFromStart.push_back(startPoint);
	grayFromEnd.push_back(endPoint);

	glClearColor(GLclampf(0.7), GLclampf(0.7), GLclampf(0.7), 0);

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

void Clean()
{
	bfs_started = bfs_started_start_end = dfs_started = a_star = false;

	for (int i = 0; i < MSIZE; i++)
	{
		for (int j = 0; j < MSIZE; j++)
		{
			if (maze[i][j] != START && maze[i][j] != TARGET&& maze[i][j] != WALL)
				maze[i][j] = SPACE;
		}
	}
	while (!grayFromStart.empty())
		grayFromStart.erase(grayFromStart.begin());
	while (!grayFromEnd.empty())
		grayFromEnd.erase(grayFromEnd.begin());
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

	if (grayFromStart.empty())
	{
		bfs_started = false;// there is no path to the target
	}
	else // gray is not empty
	{
		pt = grayFromStart[0]; // this will be the parent
		grayFromStart.erase(grayFromStart.begin()); // dequeue

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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
			}
			if (!bfs_started) // target was found
				ShowPath(pt);
		}
	}
}

Point2D* ChangeDirection(Point2D* pt, int y, int x)
{
	Point2D* temp = new Point2D(x, y), *temp2;
	do
	{
		temp2 = temp;
		temp = parent[temp->GetY()][temp->GetX()];
		parent[y][x] = pt;
		pt = temp2;
		x = temp->GetX();
		y = temp->GetY();
	} while (maze[temp->GetY()][temp->GetX()] != TARGET);
	return temp2;
}

void BfsIterationStartEnd()
{
	Point2D* pt;
	Point2D* pt1;

	if (grayFromStart.empty() || grayFromEnd.empty())
	{
		bfs_started_start_end = false;// there is no path to the target
	}
	else // gray is not empty
	{
		pt = grayFromStart[0]; // this will be the parent
		grayFromStart.erase(grayFromStart.begin()); // dequeue

								  // paint pt VISITED
		if (maze[pt->GetY()][pt->GetX()] == TARGET || maze[pt->GetY()][pt->GetX()] == VISITED_FROM_END_TO_START) // we have found the target
		{
			bfs_started_start_end = false;
		}
		else
		{
			if (maze[pt->GetY()][pt->GetX()] != START)
				maze[pt->GetY()][pt->GetX()] = VISITED; // y is i, x is j!!! 
														// check non-visited neighbors
														// go up
			if (maze[pt->GetY() + 1][pt->GetX()] == TARGET)
			{
				bfs_started_start_end = false;

			}
			if (bfs_started_start_end && maze[pt->GetY() + 1][pt->GetX()] == GRAY_FROM_END)
			{
				bfs_started_start_end = false;
				pt = ChangeDirection(pt, pt->GetY() + 1, pt->GetX());
			}
			if (bfs_started_start_end && maze[pt->GetY() + 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() + 1][pt->GetX()] = GRAY;
				parent[pt->GetY() + 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 
				grayFromStart.push_back(pt1);
			}
			// go down
			if (bfs_started_start_end &&maze[pt->GetY() - 1][pt->GetX()] == TARGET)
			{
				bfs_started_start_end = false;

			}
			if (bfs_started_start_end &&maze[pt->GetY() - 1][pt->GetX()] == GRAY_FROM_END)
			{
				bfs_started_start_end = false;
				pt = ChangeDirection(pt, pt->GetY() - 1, pt->GetX());
			}
			if (bfs_started_start_end && maze[pt->GetY() - 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() - 1][pt->GetX()] = GRAY;
				parent[pt->GetY() - 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 
				grayFromStart.push_back(pt1);
			}
			// go right
			if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() + 1] == TARGET)
			{
				bfs_started_start_end = false;

			}
			if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() + 1] == GRAY_FROM_END)
			{
				bfs_started_start_end = false;
				pt = ChangeDirection(pt, pt->GetY(), pt->GetX() + 1);
			}
			if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() + 1] == SPACE)
			{ // add it to gray
				parent[pt->GetY()][pt->GetX() + 1] = pt;
				maze[pt->GetY()][pt->GetX() + 1] = GRAY;
				pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
				grayFromStart.push_back(pt1);
			}
			// go left
			if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() - 1] == TARGET)
			{
				bfs_started_start_end = false;

			}
			if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() - 1] == GRAY_FROM_END)
			{
				bfs_started_start_end = false;
				pt = ChangeDirection(pt, pt->GetY(), pt->GetX() - 1);
			}
			if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() - 1] == SPACE)
			{ // add it to gray
				maze[pt->GetY()][pt->GetX() - 1] = GRAY;
				parent[pt->GetY()][pt->GetX() - 1] = pt;
				pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
				grayFromStart.push_back(pt1);
			}
			if (!bfs_started_start_end) // target was found
				ShowPath(pt);
			else
			{
				if (bfs_started_start_end && (grayFromStart.empty() || grayFromEnd.empty()))
				{
					bfs_started_start_end = false;// there is no path to the target
				}
				else
				{
					pt = grayFromEnd[0]; // this will be the parent
					grayFromEnd.erase(grayFromEnd.begin()); // dequeue

					if (bfs_started_start_end && (maze[pt->GetY()][pt->GetX()] == START || maze[pt->GetY()][pt->GetX()] == VISITED)) // we have found the target
					{
						bfs_started_start_end = false;
					}
					else
					{
						if (bfs_started_start_end && maze[pt->GetY()][pt->GetX()] != TARGET)
							maze[pt->GetY()][pt->GetX()] = VISITED_FROM_END_TO_START; // y is i, x is j!!! 
																	// check non-visited neighbors
																	// go up
						if (bfs_started_start_end && maze[pt->GetY() + 1][pt->GetX()] == START)
						{
							bfs_started_start_end = false;

						}
						if (bfs_started_start_end && maze[pt->GetY() + 1][pt->GetX()] == GRAY)
						{
							bfs_started_start_end = false;
							pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 
							pt = ChangeDirection(pt1, pt->GetY(), pt->GetX());
						}
						if (bfs_started_start_end && maze[pt->GetY() + 1][pt->GetX()] == SPACE)
						{ // add it to gray
							maze[pt->GetY() + 1][pt->GetX()] = GRAY_FROM_END;
							parent[pt->GetY() + 1][pt->GetX()] = pt;
							pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 
							grayFromEnd.push_back(pt1);
						}
						// go down
						if (bfs_started_start_end && maze[pt->GetY() - 1][pt->GetX()] == START)
						{
							bfs_started_start_end = false;

						}
						if (bfs_started_start_end && maze[pt->GetY() - 1][pt->GetX()] == GRAY)
						{
							bfs_started_start_end = false;
							pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 
							pt = ChangeDirection(pt1, pt->GetY(), pt->GetX());
						}
						if (bfs_started_start_end && maze[pt->GetY() - 1][pt->GetX()] == SPACE)
						{ // add it to gray
							maze[pt->GetY() - 1][pt->GetX()] = GRAY_FROM_END;
							parent[pt->GetY() - 1][pt->GetX()] = pt;
							pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 
							grayFromEnd.push_back(pt1);
						}
						// go right
						if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() + 1] == START)
						{
							bfs_started_start_end = false;

						}
						if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() + 1] == GRAY)
						{
							bfs_started_start_end = false;
							pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
							pt = ChangeDirection(pt1, pt->GetY(), pt->GetX());
						}
						if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() + 1] == SPACE)
						{ // add it to gray
							parent[pt->GetY()][pt->GetX() + 1] = pt;
							maze[pt->GetY()][pt->GetX() + 1] = GRAY_FROM_END;
							pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
							grayFromEnd.push_back(pt1);
						}
						// go left
						if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() - 1] == START)
						{
							bfs_started_start_end = false;

						}
						if (bfs_started_start_end &&maze[pt->GetY()][pt->GetX() - 1] == GRAY)
						{
							bfs_started_start_end = false;
							pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
							pt = ChangeDirection(pt1, pt->GetY(), pt->GetX());
						}
						if (bfs_started_start_end && maze[pt->GetY()][pt->GetX() - 1] == SPACE)
						{ // add it to gray
							maze[pt->GetY()][pt->GetX() - 1] = GRAY_FROM_END;
							parent[pt->GetY()][pt->GetX() - 1] = pt;
							pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
							grayFromEnd.push_back(pt1);
						}
						if (!bfs_started_start_end) // target was found
							ShowPath(pt);
					}
				}
			}
		}
	}
}

void DfsIteration()
{
	Point2D* pt;
	Point2D* pt1;

	if (grayFromStart.empty())
	{
		dfs_started = false;// there is no path to the target
	}
	else // gray is not empty
	{
		pt = grayFromStart[grayFromStart.size() - 1]; // this will be the parent
		grayFromStart.pop_back(); // pop

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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
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
				grayFromStart.push_back(pt1);
			}
			if (!dfs_started) // target was found
				ShowPath(pt);
		}
	}
}

void AstarIteration()
{

}

void DrawMaze()
{
	int i, j;

	for (i = 0; i < MSIZE; i++)
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
				glColor3d(1, .8, 0); // GRAY;
				break;
			case GRAY_FROM_END:
				glColor3d(1, .8, 0); // GRAY;
				break;
			case PATH:
				glColor3d(1.0, 0.25, 0.0); // ORANGE;
				break;
			case VISITED_FROM_END_TO_START:
				glColor3d(1, 0.43, 0.78);  // NeonPink ;
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
	if (bfs_started_start_end)
		BfsIterationStartEnd();
	if (dfs_started)
		DfsIteration();
	if (a_star)
		AstarIteration();
	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	switch (choice)
	{
	case 1:
		Clean();
		init();
		break;
	case 2:
		bfs_started = true;
		break;
	case 3:
		bfs_started_start_end = true;
		break;
	case 4:
		a_star = true;
		break;
	case 5:
		dfs_started = true;
		break;
	case 6:
		Clean();
		grayFromStart.push_back(startPoint);
		grayFromEnd.push_back(endPoint);
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Maze");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	init();

	glutCreateMenu(Menu);
	glutAddMenuEntry("New maze", 1);
	glutAddMenuEntry("BFS", 2);
	glutAddMenuEntry("BFS Start-End", 3);
	glutAddMenuEntry("DFS", 4);
	glutAddMenuEntry("A*", 5);
	glutAddMenuEntry("Clean", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
}