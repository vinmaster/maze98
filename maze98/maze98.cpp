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

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
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
					list[i][j] = 15;
				else
					list[i][j] = 15;
			}
		}
		// Make walls inside the maze
		vector<Point> visited;
		Point* p = new Point(0,0);
		//visited.push_back(p);

		//int random = rand() % visited.size();
		generateHelper(*p, &visited);
	}
	void generateHelper(Point& po, vector<Point>* visited) {
		Point* current = &po;
		vector<Point> neighbors = getNeighbors(list, *current);
		cout << "current " << current->x << " " << current->y << endl;
		visited->push_back(Point(current->x, current->y));

		while (neighbors.size() > 0) {
			int random = rand() % neighbors.size();
			//cout << "rand " << random << endl;
			int index = find(*visited, neighbors.at(random));
			cout << "p " << neighbors.at(random).x << " " << neighbors.at(random).y << endl;
			if (index != -1) {
				neighbors.erase(neighbors.begin()+random);
				//visited.erase(visited.begin()+index);
			} else { // The cell haven't been visited
				Point* p = &neighbors.at(random);
				int x = p->x;
				int y = p->y;
				if (x > current->x) // east
					list[current->y][current->x] = list[current->y][current->x] & 11; // OR with 1011
				else if (x < current->x) // west
					list[current->y][current->x] = list[current->y][current->x] & 14; // OR with 1110
				else if (y > current->y) // north
					list[current->y][current->x] = list[current->y][current->x] & 7; // OR with 0111
				else if (y < current->y) // south
					list[current->y][current->x] = list[current->y][current->x] & 13; // OR with 1101
				cout << "list " << list[current->x][current->y] << " " << list[current->x][current->y] << endl;
				
				neighbors.erase(neighbors.begin()+random);
				cout << "p2 " << x << " " << y << endl;
				generateHelper(Point(x, y), visited);
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

// angle of rotation for the camera direction
float angle = 0.0f;

// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f;

// XZ position of the camera
float x=0.0f, y=1.0f, z=5.0f;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;
GLuint texture;
float wallWidth = 10.0f, wallHeight = 3.0f;
float wallThickness = 0.5f;
int mazeSize = 10;
Maze maze(mazeSize);

bool wasButtonReleased = true;

GLuint LoadTexture( const char * filename )
{
  int width, height;

  unsigned char * data;

  FILE * file;

  file = fopen( filename, "rb" );

  if ( file == NULL ) return 0;
  width = 1024;
  height = 1024;
  data = (unsigned char *)malloc( width * height * 3 );
  //int size = fseek(file,);
  fread( data, width * height * 3, 1, file );
  fclose( file );

 for(int i = 0; i < width * height ; ++i)
{
   int index = i*3;
   unsigned char B,R;
   B = data[index];
   R = data[index+2];

   data[index] = R;
   data[index+2] = B;

}


glGenTextures( 1, &texture );
glBindTexture( GL_TEXTURE_2D, texture );
glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
free( data );

return texture;
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawSnowMan() {

	glColor3f(1.0f, 1.0f, 1.0f);

// Draw Body
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(0.75f,20,20);

// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f,20,20);

// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();

// Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f,0.5f,10,2);
}

void drawWall() {
	float wallWidth = 3.0f, wallHeight = 3.0f;
	float wallThickness = 0.5f;

	// Front
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glScalef(2.0f, 1.0f, 1.0f);
	glRectf(0.0f, 0.0f, wallWidth, wallHeight);

	glTranslatef(0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(0.0f, 0.0f, wallWidth, 0.1f);
	glPopMatrix();

	// Left side
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRectf(0.0f, 0.0f, wallThickness, wallHeight);
	glPopMatrix();

	// Right side
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(wallWidth, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRectf(0.0f, 0.0f, wallThickness, wallHeight);
	glPopMatrix();

	// Back
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -wallThickness);
	glRectf(0.0f, 0.0f, wallWidth, wallHeight);
	glPopMatrix();
}

void computePos(float deltaMove) {

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void computeDir(float deltaAngle) {
    if(wasButtonReleased == true) {
	    angle += deltaAngle;
	    lx = floor(sin(angle * PI/180) + .5);
	    lz = floor(-cos(angle * PI/180) + .5);
        wasButtonReleased = false;
    }
}

void renderScene(void) {

	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y,  z+lz,
			0.0f, y,  0.0f);

	// Draw texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 2.0f);glVertex2f(0.0f,0.0f);
	glTexCoord2f(2.0f, 2.0f);glVertex2f(2,0.0f);
	glTexCoord2f(2.0f, 0.0f);glVertex2f(2,2);
	glTexCoord2f(0.0f, 0.0f);glVertex2f(0.0f,2);
	glEnd();

// Draw ground

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();

	// Draw walls
	//drawWall();

// Draw 36 SnowMen

	for(int i = -3; i < 3; i++)
		for(int j=-3; j < 3; j++) {
			if (i != 0 && j != 0) {
                     glPushMatrix();
                     glTranslatef(i*10.0,0,j * 10.0);
                     drawSnowMan();
                     glPopMatrix();
			}
        }
        glutSwapBuffers();
} 

void processNormalKeys(unsigned char key, int xx, int yy) { 	

        if (key == 27)
              exit(0);
} 

void pressKey(int key, int xx, int yy) {

       switch (key) {
             case GLUT_KEY_UP : deltaMove = 0.5f; break;
             case GLUT_KEY_DOWN : deltaMove = -0.5f; break;
			 case GLUT_KEY_LEFT : deltaAngle = -90.0f; break;
			 case GLUT_KEY_RIGHT : deltaAngle = 90.0f; break;
			 case GLUT_KEY_PAGE_UP : y+=0.1f; break;
			 case GLUT_KEY_PAGE_DOWN : y-=0.1f; break;
			 case GLUT_KEY_HOME : z++; break;
			 case GLUT_KEY_END : z--; break;
       }
} 

void releaseKey(int key, int x, int y) {
    wasButtonReleased = true;
        switch (key) {
             case GLUT_KEY_UP :
             case GLUT_KEY_DOWN : deltaMove = 0;break;
			 case GLUT_KEY_LEFT :
			 case GLUT_KEY_RIGHT : deltaAngle = 0;break;
        }
} 

void mouseMove(int x, int y) { 	

    // this will only be true when the left button is down
	if (xOrigin >= 0) {

		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
		}
	}
}

int main(int argc, char **argv) {
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,640);
	glutCreateWindow("Maze 98");

	// register callbacks
    texture= LoadTexture( "wall.png" );
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	// here are the two new functions
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	//maze.generate();
	int a[] = {13, 9, 10, 12, 3, 10, 10, 10, 12, 13};
	int b[] = {1,6,13,5,9,10,10,12,3,4};
	int c[] = {5,9,6,3,2,12,13,1,14,5};
	int d[] = {5, 5, 11, 12, 9, 6, 5, 5, 9, 6};
	int e[] = {3, 6, 9, 6, 5, 9, 6, 5, 3, 12};
	int f[] = {9, 12, 1, 8, 6, 5, 9, 6, 9, 6};
	int g[] = {5, 7, 5, 1, 8, 6, 5, 9, 6, 13};
	int h[] = {3, 10, 6, 5, 5, 13, 3, 6, 9, 4};
	int i[] = {9, 10, 10, 4, 3, 6, 9, 10, 6, 5};
	int j[] = {3, 10, 14, 3, 10, 8, 6, 11, 10, 6};
	maze.list[0] = a;
	maze.list[1] = b;
	maze.list[2] = c;
	maze.list[3] = d;
	maze.list[4] = e;
	maze.list[5] = f;
	maze.list[6] = g;
	maze.list[7] = h;
	maze.list[8] = i;
	maze.list[9] = j;

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}