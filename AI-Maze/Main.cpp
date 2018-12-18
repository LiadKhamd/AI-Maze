#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Point2D.h"
#include "Point2D_hg.h"
#include "ComparePoints.h"

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
Point2D_hg* startAstar;

bool bfs_started = false, dfs_started = false, bfs_started_start_end = false, a_star = false;

clock_t startTime, endTime;

// gray queue
vector <Point2D*> grayFromStart;
vector <Point2D*> grayFromEnd;
priority_queue <Point2D_hg, vector <Point2D_hg>, ComparePoints> pq;

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

	startAstar = new Point2D_hg(*startPoint, *endPoint);
	pq.push(*startAstar);
	// save the start in gray
	grayFromStart.push_back(startPoint);
	grayFromEnd.push_back(endPoint);

	glClearColor(GLclampf(0.7), GLclampf(0.7), GLclampf(0.7), 0);

	glOrtho(-1, 1, -1, 1, -1, 1);
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
	Point2D *temp;
	Point2D_hg temp2;
	while (!grayFromStart.empty())
	{
		temp = grayFromStart[0];
		grayFromStart.erase(grayFromStart.begin());
		if (maze[temp->GetY()][temp->GetX()] != START)
			delete(temp);
	}
	while (!grayFromEnd.empty())
	{
		temp = grayFromEnd[0];
		grayFromEnd.erase(grayFromEnd.begin());
		if (maze[temp->GetY()][temp->GetX()] != TARGET)
			delete(temp);
	}
	while (!pq.empty())
	{
		temp2 = pq.top();
		pq.pop();
	}
}

void ShowPath(Point2D* pt)
{
	if (pt != NULL)
	{
		Point2D* temp;
		while (maze[pt->GetY()][pt->GetX()] != START)
		{
			maze[pt->GetY()][pt->GetX()] = PATH;
			temp = pt;
			pt = parent[pt->GetY()][pt->GetX()];
			delete(temp);
		}
	}
}

void BfsIteration()
{
	Point2D* pt = NULL;
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
		}
	}
	if (!bfs_started) { // target was found
		endTime = clock();
		cout << "BFS - Time: " << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " sec" << endl;
		ShowPath(pt);
	}
}

Point2D* ChangeDirection(Point2D* pt, int y, int x)
{
	if (pt != NULL)
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
	return NULL;
}

void BfsIterationStartEnd()
{
	Point2D* pt = NULL;
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
			if (bfs_started_start_end)
			{
				if (grayFromStart.empty() || grayFromEnd.empty())
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
					}
				}
			}
		}
	}
	if (!bfs_started_start_end) { // target was found
		endTime = clock();
		cout << "BFS 2W - Time: " << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " sec" << endl;
		ShowPath(pt);
	}
}

void DfsIteration()
{
	Point2D* pt = NULL;
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
		}
	}
	if (!dfs_started) { // target was found
		endTime = clock();
		cout << "DFS - Time: " << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " sec" << endl;
		ShowPath(pt);
	}
}

void AstarIteration()
{
	Point2D* pt = NULL;
	Point2D* pt1;
	Point2D_hg bestPoint;

	if (pq.empty())
	{
		a_star = false;// there is no path to the target
	}
	else // gray is not empty
	{
		bestPoint = pq.top();
		pq.pop();
		pt = new Point2D(bestPoint.getPoint());

		// paint pt VISITED
		if (maze[pt->GetY()][pt->GetX()] == TARGET) // we have found the target
		{
			a_star = false;
		}
		else
		{
			if (maze[pt->GetY()][pt->GetX()] != START)
				maze[pt->GetY()][pt->GetX()] = VISITED; // y is i, x is j!!! 
														// check non-visited neighbors
														// go up
			if (maze[pt->GetY() + 1][pt->GetX()] == TARGET)
			{
				a_star = false;
			}
			if (a_star && maze[pt->GetY() + 1][pt->GetX()] == SPACE)
			{ // add it to pq
				maze[pt->GetY() + 1][pt->GetX()] = GRAY;
				parent[pt->GetY() + 1][pt->GetX()] = pt;

				pt1 = new Point2D(pt->GetX(), pt->GetY() + 1);// y is i, x is j!!! 

				Point2D_hg *pbn = new Point2D_hg(bestPoint, *pt1, *endPoint);
				pq.emplace(*pbn);
			}
			// go down
			if (a_star && maze[pt->GetY() - 1][pt->GetX()] == TARGET)
			{
				a_star = false;
			}
			if (a_star &&  maze[pt->GetY() - 1][pt->GetX()] == SPACE)
			{ // add it to gray
				maze[pt->GetY() - 1][pt->GetX()] = GRAY;
				parent[pt->GetY() - 1][pt->GetX()] = pt;
				pt1 = new Point2D(pt->GetX(), pt->GetY() - 1);// y is i, x is j!!! 

				Point2D_hg *pbn = new Point2D_hg(bestPoint, *pt1, *endPoint);
				pq.emplace(*pbn);
			}
			// go right
			if (a_star && maze[pt->GetY()][pt->GetX() + 1] == TARGET)
			{
				a_star = false;

			}
			if (a_star &&  maze[pt->GetY()][pt->GetX() + 1] == SPACE)
			{ // add it to gray
				parent[pt->GetY()][pt->GetX() + 1] = pt;
				maze[pt->GetY()][pt->GetX() + 1] = GRAY;
				pt1 = new Point2D(pt->GetX() + 1, pt->GetY());// y is i, x is j!!! 
				Point2D_hg *pbn = new Point2D_hg(bestPoint, *pt1, *endPoint);
				pq.emplace(*pbn);
			}
			// go left
			if (a_star &&  maze[pt->GetY()][pt->GetX() - 1] == TARGET)
			{
				a_star = false;

			}
			if (a_star && maze[pt->GetY()][pt->GetX() - 1] == SPACE)
			{ // add it to gray
				maze[pt->GetY()][pt->GetX() - 1] = GRAY;
				parent[pt->GetY()][pt->GetX() - 1] = pt;
				pt1 = new Point2D(pt->GetX() - 1, pt->GetY());// y is i, x is j!!! 
				Point2D_hg *pbn = new Point2D_hg(bestPoint, *pt1, *endPoint);
				pq.emplace(*pbn);
			}
		}
	}
	if (!a_star) { // target was found
		endTime = clock();
		cout << "A* Time: " << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " sec" << endl;
		ShowPath(pt);
	}
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
				glColor3d(1.0, 1.0, 0.0); // YELLOW;
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
	Clean();
	switch (choice)
	{
	case 1:
		init();
		break;
	case 2:
		bfs_started = true;
		startTime = clock();
		goto DEFULAT;
	case 3:
		bfs_started_start_end = true;
		startTime = clock();
		goto DEFULAT;
	case 4:
		dfs_started = true;
		startTime = clock();
		goto DEFULAT;
	case 5:
		a_star = true;
		startTime = clock();
		goto DEFULAT;
	DEFULAT:	default:
		grayFromStart.push_back(startPoint);
		grayFromEnd.push_back(endPoint);
		pq.push(*startAstar);
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