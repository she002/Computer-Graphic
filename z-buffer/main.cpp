// Name:            Sihan He
// quater, year:    fall, 2014
// Lab:             021
// Project:         1
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////


/* ***** Instruction *******
 * 
 * press '0' to reset the program
 *
 * press 'q' to rotate counter clockwise though y-axis
 * 
 * press 'e' to rotate clockwise though y-axis
 *
 * press 'a' to move left
 *
 * press 'd' to move right
 * 
 * press 'w' to move up 
 * 
 * press 's' to move down
 * 
 * press 'r' to zoom in 
 * 
 * pree 'f' to zoom out
*/


#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <utility>
#include <vector>
#include "point2d.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

/****************************** global variables*****************************************/
const int WINDOW_WIDTH = 800;	       	//width of the screen 
const int WINDOW_HEIGHT = 800; 		//height of the screen
const int COLOR_CHOICES = 10;  		//number of available colors 
const int BUFFER_SIZE = 80;        
const double scale_large = 1.1;        //value that you want to scale the object to bigger size
const double scale_small = 0.9;        //value that you want to scale the object to smaller size
const double x_translate = 10;         //value that you want to translate through x axis
const double y_translate = 10;         //value that you want to translate through y axis
const double rotate_angle = 22.5;      //value of angle that you want to rotate the object
char mode = '1';

/***************************** define structure ******************************************/
struct point3d   //used to store a 3d point value include x, y and z
{
	double x;
	double y;
	double z;

	point3d() : x(0.0), y(0.0), z(0.0) {}
	point3d(const double & nx, const double & ny, const double & nz) : x(nx), y(ny), z(nz) {}
};
struct tri_struct  //used to store the position of 3 points of a triangle and thier color information
{
	int a;
	int b;
	int c;
	float red;
	float green;
	float blue;
};

struct line_data    //used to store necessary data for a line equation Ax+By+C = 0
{
	double mx;              //mx
	double my; 		//my
	double constant;	//c
	double max_x;           //biggest  x value of the line
	double min_x;		//smallest x value of the line
	double max_y;		//biggest  y value of the line
	double min_y;		//smallest y value of the line
};

struct colors      //store data for color including r(red), g(green) and b(blue) values
{
	float r;
	float g;
	float b;
	colors() : r(1.0), g(1.0), b(1.0) {};
	colors(const float & nr, const float & ng, const float & nb):r(nr), g(ng), b(nb){};
};

/********************************initialized data structure***************************************8*/
vector<Point2D>pts;

vector<point3d>all_vertices;          //store all the vertices

vector<tri_struct>all_triangles;      //store all the triangels;

double** z_buffer = new double*[WINDOW_HEIGHT+1];  // 801 * 801 matrix for z buffer

// store the color data
colors mix[COLOR_CHOICES] = {colors(1.0, 1.0, 1.0),
		colors(1.0, 0.0, 0.0),
		colors(0.0, 1.0, 0.0),
		colors(0.0, 0.0, 1.0),
		colors(0.5, 1.0, 1.0),
		colors(1.0, 0.5, 1.0),
		colors(1.0, 1.0, 0.5),
		colors(1.0, 1.0, 0.0),
		colors(1.0, 0.0, 1.0),
		colors(0.0, 1.0, 1.0)};




/****************************************define function******************************************/

/*****************************************************************************************************
 void renderPixel(int, int, float, float, float)
 Renders a quad at cell (x, y) with dimensions CELL_LENGTH
 *****************************************************************************************************/
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glColor3f(r, g, b);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
	
}

/*****************************************************************************************************

void get_tri_lines(tri_struct, vector<line_data>& ) :
This function can calculate the necessary information for the 3 lines that form a triangle
Each line can be defined by f(x,y) = Ax + By + C where A = my, B = -mx, C is constant
The necessary data we need for each line includes mx, my, constant, max_y, min_y, max_x, min_x

*****************************************************************************************************/
void get_tri_lines(tri_struct tri_1, vector<line_data>& lines)
{
	
	line_data line_a;
	line_data line_b;
	line_data line_c;
	//-------------------------------line a-------------------------------------------------------
	line_a.mx = all_vertices[tri_1.b].x - all_vertices[tri_1.a].x;       //get mx
	line_a.my = all_vertices[tri_1.b].y - all_vertices[tri_1.a].y;       //get my
	
	//get max_y and min_y
	if(line_a.my > 0)                        
	{
		line_a.max_y = all_vertices[tri_1.b].y;
		line_a.min_y = all_vertices[tri_1.a].y;
	}
	else
	{
		line_a.max_y = all_vertices[tri_1.a].y;
		line_a.min_y = all_vertices[tri_1.b].y;
	}

	//get max_x and min_x
	if(all_vertices[tri_1.a].x > all_vertices[tri_1.b].x)
	{
		line_a.max_x = all_vertices[tri_1.a].x;
		line_a.min_x = all_vertices[tri_1.b].x;
	}
	else
	{
		line_a.max_x = all_vertices[tri_1.b].x;
		line_a.min_x = all_vertices[tri_1.a].x;
	}
	
	//get constant
	line_a.constant = (line_a.mx)*(all_vertices[tri_1.a].y) - (line_a.my)*(all_vertices[tri_1.a].x); 


	//-------------------------------line b----------------------------------------------------------
	line_b.mx = all_vertices[tri_1.c].x - all_vertices[tri_1.b].x;    //get mx
	line_b.my = all_vertices[tri_1.c].y - all_vertices[tri_1.b].y;    //get my

	//get max_y and min_y
	if(line_b.my > 0)
	{
		line_b.max_y = all_vertices[tri_1.c].y;
		line_b.min_y = all_vertices[tri_1.b].y;
	}
	else
	{
		line_b.max_y = all_vertices[tri_1.b].y;
		line_b.min_y = all_vertices[tri_1.c].y;
	}

	//get max_x and min_x
	if(all_vertices[tri_1.b].x > all_vertices[tri_1.c].x)
	{
		line_b.max_x = all_vertices[tri_1.b].x;
		line_b.min_x = all_vertices[tri_1.c].x;
	}
	else
	{
		line_b.max_x = all_vertices[tri_1.c].x;
		line_b.min_x = all_vertices[tri_1.b].x;
	}
	//get constant c
	line_b.constant = (line_b.mx)*(all_vertices[tri_1.b].y) - (line_b.my)*(all_vertices[tri_1.b].x);

	//-------------------------------line c----------------------------------------------------------
	line_c.mx = all_vertices[tri_1.c].x - all_vertices[tri_1.a].x;     //get mx
	line_c.my = all_vertices[tri_1.c].y - all_vertices[tri_1.a].y;     //get my
	//get max_y and min_y
	if(line_c.my > 0)
	{
		line_c.max_y = all_vertices[tri_1.c].y;
		line_c.min_y = all_vertices[tri_1.a].y;
	}
	else
	{
		line_c.max_y = all_vertices[tri_1.a].y;
		line_c.min_y = all_vertices[tri_1.c].y;
	}
	//get max_x and min_x
	if(all_vertices[tri_1.a].x > all_vertices[tri_1.c].x)
	{
		line_c.max_x = all_vertices[tri_1.a].x;
		line_c.min_x = all_vertices[tri_1.c].x;
	}
	else
	{
		line_c.max_x = all_vertices[tri_1.c].x;
		line_c.min_x = all_vertices[tri_1.a].x;
	}
	line_c.constant = (line_c.mx)*(all_vertices[tri_1.a].y) - (line_c.my)*(all_vertices[tri_1.a].x);

	//push back to the vector(line_data> lines
	lines.push_back(line_a);
	lines.push_back(line_b);
	lines.push_back(line_c);
}


//this functino find normal vector
point3d get_normal_vector(tri_struct tri)
{
	// get 2 vector from the triangle
	point3d vector1, vector2, the_normal_vector;
	vector1 = point3d(all_vertices[tri.b].x-all_vertices[tri.a].x, 
			all_vertices[tri.b].y-all_vertices[tri.a].y, all_vertices[tri.b].z-all_vertices[tri.a].z);
	vector2 = point3d(all_vertices[tri.c].x-all_vertices[tri.a].x, 
			all_vertices[tri.c].y-all_vertices[tri.a].y, all_vertices[tri.c].z-all_vertices[tri.a].z);
	//do cross product to get normal vector
	double a = vector1.y*vector2.z-vector1.z*vector2.y;
	double b = vector1.z*vector2.x-vector1.x*vector2.z;
	double c = vector1.x*vector2.y-vector1.y*vector2.x;

	the_normal_vector = point3d(a, b, c);

	return the_normal_vector;
	
}


/****************************************************************************
double get_D(point3d, point3d):
this function calculated D for the plane equation Ax+By+Cz+D = 0
since we already have A,B,C value from normal vector
we can pick a random point and insert its x, y and z value to get D

*****************************************************************************/
double get_D(point3d pt, point3d normal_vector)
{
	int D;
	D = 0-((normal_vector.x)*(pt.x)+(normal_vector.y)*(pt.y)+(normal_vector.z)*(pt.z));
	return D;
}

/****************************************************************************
double find_max_y(tri_struct temp):
this function find maximun y value for a triangle
*****************************************************************************/
double find_max_y(tri_struct temp)
{
	double max_y = all_vertices[temp.a].y;
	if(all_vertices[temp.b].y > max_y)
	{
		max_y = all_vertices[temp.b].y;
	}
	if(all_vertices[temp.c].y > max_y)
	{
		max_y = all_vertices[temp.c].y;
	}
	return max_y;
}

/****************************************************************************
double find_min_y(tri_struct temp)
this function find minimun y value for a triangle
*****************************************************************************/
double find_min_y(tri_struct temp)
{
	double min_y = all_vertices[temp.a].y;
	if(all_vertices[temp.b].y < min_y)
	{
		min_y = all_vertices[temp.b].y;
	}
	if(all_vertices[temp.c].y < min_y)
	{
		min_y = all_vertices[temp.c].y;
	}
	return min_y;
}


/****************************************************************************
void draw_a_triangle(tri_struct ) 
this function can draw a triangle
tri_struct is the data structure that store data of 3 points
*****************************************************************************/
void draw_a_triangle(tri_struct tri_1)
{
	vector<line_data>lines;
	get_tri_lines(tri_1, lines); //get necessary data for the triangle

	//calculate A, B, C, D for plane equation Ax+By+Cz+D = 0
	//we can get A,B,C from calculating normal vector
	point3d normal_vector = get_normal_vector(tri_1);  //find normal vector for the triangle
	double constant_D = get_D(all_vertices[tri_1.a], normal_vector);   // get D value
	double y_max = find_max_y(tri_1);
	double y_min = find_min_y(tri_1);

	//scan through y from min y to max y 
	for(int y = (int)(y_min+0.5); y <= (int)(y_max+0.5); y++)
	{
		int x_min = 800, x_max = 800;
		//for each scan line of y, find minimum x value and maximum x value
		for(int i = 0; i < lines.size(); i++)
		{
			if(lines[i].max_y>= y && lines[i].min_y <= y)
			{
				if(lines[i].my == 0)
				{
					x_max = lines[i].max_x;
					x_min = lines[i].min_x;
				}
				else
				{
					int temp = (int)((((lines[i].mx)*y - (lines[i].constant))/(lines[i].my))+0.5);
					if(temp < x_min)
					{
						x_max = x_min;
						x_min = temp;
					
					}
					else if(temp >= x_min)
					{
						x_max = temp;
					}
					
				}
				
			}
			
		}
		if(y>=0 && y<=800)
		{
			//find the initial value of z in current scan line. 
			double z;	
			z = (0-(normal_vector.x)*x_min - (normal_vector.y)*y - constant_D)/normal_vector.z;

			//update z-buffer for the initial z value that render the pixel
			if(x_min>=0 && x_min <= 800)
			{
				if(z_buffer[y][x_min] > z)
				{
					z_buffer[y][x_min] = z;
					renderPixel(x_min, y, tri_1.red, tri_1.green, tri_1.blue);
				}
			}
			for(int x = x_min+1; x <= x_max; x++)
			{
				//scan from min x to max x to calculate depth of every pixel of the line inside the triangle
				z = z-(normal_vector.x)/(normal_vector.z);
				if(x>=0 && x<= 800)	
				{
					if(z_buffer[y][x] > z)
					{
						z_buffer[y][x] = z;
						renderPixel(x, y, tri_1.red, tri_1.green, tri_1.blue);
					}
				}
			
			}
		}
	}

}

/****************************************************************************************
void draw_many_triangles()
this function render all the triangles by calling "void draw_a_triangle(tri_struct)" many times
*****************************************************************************************/
void draw_many_triangles()
{
	for (int i = 0; i <= WINDOW_HEIGHT; i++) {
  		z_buffer[i] = new double[WINDOW_WIDTH+1];
	}
	for (int y = 0; y <= WINDOW_HEIGHT; y++) {
  		for (int x = 0; x <= WINDOW_HEIGHT; x++) {
  			z_buffer[y][x] = 1000;
		}
	}
	for(int i = 0; i < all_triangles.size(); i++)
	{
		draw_a_triangle(all_triangles[i]);
	}
	
}

/****************************************************************************************
void rotate(double )
this function rotate the object
*****************************************************************************************/
void rotate(double input)
{
	//get middle value of x and z of a object
	double angle = input*3.14159/180; // degree = radian*3.14159/180
	double total_x = 0;
	double total_z = 0;
	for(int i = 0; i < all_vertices.size(); i++)
	{
		total_x += all_vertices[i].x;
		total_z += all_vertices[i].z;
	}
	double mid_x = total_x/all_vertices.size();
	double mid_z = total_z/all_vertices.size();
	for(int i = 0; i < all_vertices.size(); i++)
	{
		//move to origin
		double temp_x, temp_z, new_x, new_z;
		temp_x = all_vertices[i].x;
		temp_z = all_vertices[i].z;
		temp_x = temp_x - mid_x;
		temp_z = temp_z - mid_z;
		//rotate through calculating x and z value
		new_x = temp_x*cos(angle) - temp_z*sin(angle);
		new_z = temp_x*sin(angle) + temp_z*cos(angle);
		//move back to origin
		new_x = new_x + mid_x;
		new_z = new_z + mid_z;	
		all_vertices[i].x = new_x;
		all_vertices[i].z = new_z;

	}
	
		
}


/****************************************************************************************
void translate_x(double )
this function move the object through x-axis
*****************************************************************************************/
void translate_x(double input)
{
	for(int i = 0; i < all_vertices.size(); i++)
	{		
		all_vertices[i].x = all_vertices[i].x+input;
	}
}

/****************************************************************************************
void translate_y(double )
this function move the object through y-axis
*****************************************************************************************/
void translate_y(double input)
{
	for(int i = 0; i < all_vertices.size(); i++)
	{
		all_vertices[i].y = all_vertices[i].y+input;
	}		
}


/****************************************************************************************
void scale(double)
this function can scale the size of object
*****************************************************************************************/
void scale(double input)
{

	//get mid point of a object
	double max_x = all_vertices[0].x;
	double min_x = all_vertices[0].x;
	double max_y = all_vertices[0].y;
	double min_y = all_vertices[0].y;
	double max_z = all_vertices[0].z;
	double min_z = all_vertices[0].z;
	for(int i = 1; i < all_vertices.size(); i++)
	{
		// get max value and min value for x, y and z
		if(all_vertices[i].x > max_x)
		{
			max_x = all_vertices[i].x;
		}
		else if(all_vertices[i].x < min_x)
		{
			min_x = all_vertices[i].x;
		}
		if(all_vertices[i].y > max_y)
		{
			max_y = all_vertices[i].y;
		}
		else if(all_vertices[i].y < min_y)
		{
			min_y = all_vertices[i].y;
		}
		if(all_vertices[i].z > max_z)
		{
			max_z = all_vertices[i].z;
		}
		else if(all_vertices[i].z < min_z)
		{
			min_z = all_vertices[i].z;
		}
	}
	
	//get mid point of x,y and z
	double mid_x = (max_x+min_x)/2;
	double mid_y = (max_y+min_y)/2;
	double mid_z = (max_z+min_z)/2;

	for(int i = 0; i < all_vertices.size(); i++)
	{
		double temp_x, temp_y, temp_z, new_x, new_y, new_z;
		//move to origin
		temp_x = all_vertices[i].x;
		temp_y = all_vertices[i].y;
		temp_z = all_vertices[i].z;
		temp_x = temp_x - mid_x;
		temp_y = temp_y - mid_y;
		temp_z = temp_z - mid_z;
		//scale
		new_x = temp_x*input;
		new_y = temp_y*input;
		new_z = temp_z*input;
		//return to original position
		new_x = new_x + mid_x;
		new_y = new_y + mid_y;	
		new_z = new_z + mid_z;
		all_vertices[i].x = new_x;
		all_vertices[i].y = new_y;
		all_vertices[i].z = new_z;

	}
}

/****************************************************************************************
void DDA_Line(int, int, int, int)
this function draw a line between 2 points
*****************************************************************************************/
void DDA_Line(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
   
    int steps;
    if (abs((float)dx) > abs((float)dy))
        steps = abs((float)dx);
    else
        steps = abs((float)dy);

    float stepx = static_cast<float>(dx) / steps;
    float stepy = static_cast<float>(dy) / steps;

    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; ++i) {
        renderPixel((int)(x + 0.5), (int)(y + 0.5));
        x += stepx;
        y += stepy;
    }
}
/****************************************************************************************
void draw_a_wire_frame(tri_struct)
this function draw wire frame triangle
*****************************************************************************************/
void draw_a_wire_frame(tri_struct tri_1)
{
	// pass in 3 points that form a triangle
	point3d point1 = all_vertices[tri_1.a];
	point3d point2 = all_vertices[tri_1.b];
	point3d point3 = all_vertices[tri_1.c];
	
	// draw lines between 3 points
	DDA_Line(point1.x,point1.y, point2.x, point2.y);
	DDA_Line(point1.x,point1.y, point3.x, point3.y);
	DDA_Line(point2.x,point2.y, point3.x, point3.y);
	
}

/****************************************************************************************
void draw_many_wire_frame()
it calls the function "void draw_a_wire_frame(tri_struct)" many times depends on 
how many triangles we need to implement
*****************************************************************************************/
void draw_many_wire_frame()
{
	for(int i = 0; i < all_triangles.size(); i++)
	{
		draw_a_wire_frame(all_triangles[i]);
	}
}


/****************************************************************************************
void GLKeyboardPress(unsigned char, int, int)
this function get input from keyboard and than excute the specific function
*****************************************************************************************/
void GLKeyboardPress(unsigned char key, int x, int y)
{
	if(key == '0')		//reset the program
	{
		pts.resize(0);
		glutPostRedisplay();
	}
	else if(key == 'q')	     //rotate the obejct left
	{
		rotate(-rotate_angle);
		glutPostRedisplay();
	}
	else if(key == 'e')         //rotate the obejct right
	{
		rotate(rotate_angle);
		glutPostRedisplay();
	}
	else if(key == 'a')         //move the obejct left
	{
		translate_x(-x_translate);
		glutPostRedisplay();
	}
	else if(key == 'd')         //move the obejct right
	{
		translate_x(x_translate);
		glutPostRedisplay();
	}
	else if(key == 'w')         //move the obejct up
	{
		translate_y(y_translate);
		glutPostRedisplay();
	}
	else if(key == 's')          //move the object down
	{
		translate_y(-y_translate);
		glutPostRedisplay();
	}
	else if(key == 'r')          //scale the object to bigger size
	{
		scale(scale_large);
		glutPostRedisplay();
	}
	else if(key == 'f')          //scale the object to smaller size
	{
		scale(scale_small);
		glutPostRedisplay();
	}
	else if(key == '1')
	{
		mode = '1';   //mode == 1 means to render the object
		glutPostRedisplay();
	}
	else if(key == '2')
	{
		mode = '2';   //mode == 2 means to wireframe the triangle
		glutPostRedisplay();
	}
	
}
			
Point2D GLscreenToWindowCoordinates(const Point2D& p)
{
	GLdouble model[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	Point2D converted;
	GLdouble temp;
	gluUnProject(p.x, viewport[3]-p.y, 0, model, projection, viewport, &converted.x, &converted.y, &temp);
	return converted;
}

/****************************************************************************************
void GL_render():
this function execute the action you want
*****************************************************************************************/
void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if(all_vertices.size() >= 3)
	{
		if(mode == '1')               //mode == 1 means to render the object
		{
			draw_many_triangles();
		}
		else if(mode == '2')          //mode == 2 means to draw the wire frame of the object
		{
			draw_many_wire_frame();
		}
	}
	glutSwapBuffers();
}


/****************************************************************************************
void get_input(int, char)
this function get the argument and pass in as a file name
then use the file name to open a txt file in the same directory
and input the data from the txt file and store the data in vectors
*****************************************************************************************/
void get_input(int* argc, char** argv)
{
	if(*argc > 1)
	{
		char buff[BUFFER_SIZE];
		int vertices_num; 
		int triangles_num;
		stringstream ss;
		ifstream infile;
		infile.open(argv[1]);
		infile.getline(buff, BUFFER_SIZE);
		ss<<buff;
		ss>>vertices_num>>triangles_num;
		ss.clear();
		int counter = 0;
		for(int i = 0; i < vertices_num; i++)
		{
			int x, y, z;
			infile.getline(buff, BUFFER_SIZE);
			ss<<buff;
			ss>>x>>y>>z;
			all_vertices.push_back(point3d(x, y, z));
			ss.clear();
		}

		for(int i = 0; i < triangles_num; i++)
		{
			int one, two, three;
			infile.getline(buff, BUFFER_SIZE);
			ss<<buff;
			ss>>one>>two>>three;
			tri_struct tri;
			tri.a = one;
			tri.b = two;
			tri.c = three;
			if(counter < COLOR_CHOICES)
			{
				tri.red = mix[counter].r;
				tri.green = mix[counter].g;
				tri.blue = mix[counter].b;
				counter++;
			}
			else
			{
				counter = 0;	
				tri.red = mix[counter].r;
				tri.green = mix[counter].g;
				tri.blue = mix[counter].b;
				counter++;
			}
			all_triangles.push_back(tri);
			ss.clear();
		}
		
			
			
	}

}
//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// ...
	// Complete this function
	// ...
	glutCreateWindow("CS 130 - she002");
	glutDisplayFunc(GL_render);
	glutKeyboardFunc(GLKeyboardPress);
	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	get_input(argc, argv);
}


int main(int argc, char** argv)
{	
			
	GLInit(&argc, argv);
	glutMainLoop();

	for (int i = 0; i <= WINDOW_HEIGHT; i++) {
  		delete [] z_buffer;
	}
	return 0;
}



