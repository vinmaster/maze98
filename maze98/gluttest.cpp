#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<gl/glut.h>
#include<gl/glu.h>
#include<gl/gl.h>
#include<math.h>
#include<time.h>
#include<random>

#define PI 3.14159265

using namespace std;

struct Point {
public:
	int x;
	int y;
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}
	void set(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

struct Maze {
public:
	int size;
	int** list;
	Maze(int size) : size(10) {
		this->size = size;
		list = new int*[size];
		for(int i = 0; i < size; i++)
			list[i] = new int[size];
	}
	void Maze::generate() {
		// Fill border of the maze
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				if (i == 0 || j == 0 || i == size-1 || j == size-1)
					list[i][j] = 1;
				else
					list[i][j] = 15;
			}
		}
		// Make walls inside the maze
		vector<Point> visited;
		Point* p = new Point(1,1);
		//visited.push_back(p);

		//int random = rand() % visited.size();
		generateHelper(*p, &visited);
		/*
		Point* current = &visited.at(0);
		vector<Point> neighbors = getNeighbors(list, visited.at(0));
		cout << "current " << current->x << " " << current->y << endl;
			
		while (neighbors.size() > 0) {
			int random = rand() % neighbors.size();
			int index = find(visited, neighbors.at(random));
			if (index != -1) {
				neighbors.erase(neighbors.begin()+random);
				visited.erase(visited.begin()+index);
			} else { // The cell haven't been visited
				Point* p = &neighbors.at(random);
				int x = p->x;
				int y = p->y;
				cout << "p " << x << " " << y << endl;
				if (x > current->x) // south
					list[current->x][current->y] = list[current->x][current->y] & 13; // OR with 1101
				else if (x < current->x) // north
					list[current->x][current->y] = list[current->x][current->y] & 7; // OR with 0111
				else if (y > current->y) // east
					list[current->x][current->y] = list[current->x][current->y] & 11; // OR with 1011
				else if (y < current->y) // west
					list[current->x][current->y] = list[current->x][current->y] & 14; // OR with 1110
				neighbors.erase(neighbors.begin()+random);
				generateHelper(*p, visited);
			}
		}
		*/
	}
	void generateHelper(Point& po, vector<Point>* visited) {
		Point* current = &po;
		vector<Point> neighbors = getNeighbors(list, *current);
		cout << "current " << current->x << " " << current->y << endl;
			
		while (neighbors.size() > 0) {
			int random = 0;//rand() % neighbors.size();
			//cout << "rand " << random << endl;
			int index = find(*visited, neighbors.at(random));
			if (index != -1) {
				neighbors.erase(neighbors.begin()+random);
				//visited.erase(visited.begin()+index);
			} else { // The cell haven't been visited
				Point* p = &neighbors.at(random);
				int x = p->x;
				int y = p->y;
				cout << "p " << x << " " << y << endl;
				if (x > current->x) // south
					list[current->x][current->y] = list[current->x][current->y] & 13; // OR with 1101
				else if (x < current->x) // north
					list[current->x][current->y] = list[current->x][current->y] & 7; // OR with 0111
				else if (y > current->y) // east
					list[current->x][current->y] = list[current->x][current->y] & 11; // OR with 1011
				else if (y < current->y) // west
					list[current->x][current->y] = list[current->x][current->y] & 14; // OR with 1110
				cout << "list " << list[current->x][current->y] << " " << list[current->x][current->y] << endl;
				visited->push_back(*current);
				neighbors.erase(neighbors.begin()+random);
				generateHelper(*p, visited);
			}
		}
	}
	int find(vector<Point>& visited, Point& p) {
		for(int i = 0; i < visited.size(); i++)
			if(visited.at(i).x == p.x && visited.at(i).y == p.y)
				return i;
		return -1;
	}
	vector<Point> Maze::getNeighbors(int** list, Point& p) {
		int x = p.x;
		int y = p.y;
		vector<Point> neighbors;
		if (x-1 >= 0)
			neighbors.push_back(Point(x-1, y));
		if (x+1 < this->size)
			neighbors.push_back(Point(x+1, y));
		if (y-1 >= 0)
			neighbors.push_back(Point(x, y-1));
		if (y+1 < this->size)
			neighbors.push_back(Point(x, y+1));
		cout << "adasfs" << neighbors.size() << endl;
		return neighbors;
	}
	void Maze::print() {
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				cout << list[i][j] << " ";
			}
			cout << endl;
		}
	}
	~Maze() {
		for(int i = 0; i < size; i++)
			delete[] list[i];
		delete[] list;
	}
};

int WIDTH = 720;
int HEIGHT = 640;

float red=1.0, blue=1.0, green=1.0;
float wallWidth = 10.0f, wallHeight = 3.0f;
float wallThickness = 0.5f;
int mazeSize = 20;
Maze maze(mazeSize);

void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// X axis
	glBegin(GL_LINES);
		glColor3f(1.0,1.0,1.0);
		glVertex2i(-WIDTH,0);
		glVertex2i(WIDTH,0);
	glEnd();
	// Y axis
	glBegin(GL_LINES);
		glColor3f(1.0,1.0,1.0);
		glVertex2i(0,-HEIGHT);
		glVertex2i(0,HEIGHT);
	glEnd();

	glColor3f(1.0,1.0,1.0);
	for(int i = 0; i < mazeSize; i++) {
		for(int j = 0; j < mazeSize; j++) {
			Point point(i*wallWidth, j*wallWidth);
			//cout << point.x << " " << point.y << endl;
			if ((maze.list[i][j] & 13) == 13) // OR with 1101 south
			{
				glBegin(GL_LINES);
					glVertex2f(point.x, point.y);
					glVertex2f(point.x+wallWidth, point.y);
				glEnd();
			}
			if ((maze.list[i][j] & 7) == 7) // OR with 0111 north
			{
				glBegin(GL_LINES);
					glVertex2f(point.x, point.y+wallWidth);
					glVertex2f(point.x+wallWidth, point.y+wallWidth);
				glEnd();
			}
			if ((maze.list[i][j] & 11) == 11) // OR with 1011 east
			{
				glBegin(GL_LINES);
					glVertex2f(point.x+wallWidth, point.y+wallWidth);
					glVertex2f(point.x+wallWidth, point.y);
				glEnd();
			}
			if ((maze.list[i][j] & 14) == 14) // OR with 1110 west
			{
				glBegin(GL_LINES);
					glVertex2f(point.x, point.y+wallWidth);
					glVertex2f(point.x, point.y);
				glEnd();
			}
			/*
			switch(maze.list[i][j]){
			case 0:
				break;
			case 1:
				glBegin(GL_POLYGON);
					Point point(i*wallWidth, j*wallWidth);
					glVertex2f(point.x, point.y);
					glVertex2f(point.x, point.y+wallWidth-1);
					glVertex2f(point.x+wallWidth-1, point.y+wallWidth-1);
					glVertex2f(point.x+wallWidth-1, point.y);
				glEnd();
				break;
			}*/
		}
	}

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27 || key == 'q' || key == 'Q') { exit(0); }
}

void processSpecialKeys(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_F1 : red = 1.0; green = 0.0; blue = 0.0; break;
		case GLUT_KEY_F2 : red = 0.0; green = 1.0; blue = 0.0; break;
		case GLUT_KEY_F3 : red = 0.0; green = 0.0; blue = 1.0; break;
	}
}

void main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("Scanline and Transformation");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glOrtho(-WIDTH/2, WIDTH/2, -HEIGHT/2, HEIGHT/2, -1, 1);
	//adding here the setting of keyboard processing
	glutKeyboardFunc(processNormalKeys);  // normal keypress functions, NULL GLUT ignores
	glutSpecialFunc(processSpecialKeys);  // special keypress functions, NULL GLUT ignores

	srand(time(0));
	cout << maze.size << endl;
	Maze* maze2 = new Maze(10);
	cout << maze2->size << " " << maze2->list[2][2] << endl;
	delete maze2;
	maze.generate();
	maze.print();

	glutMainLoop();
}

