// Name: Sihan He
// Quarter, Year:  fall, 2014
// Lab: 021
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
#include <cmath>
#include <vector>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <utility>
using namespace std;

const int BUFFER_SIZE = 80;
const int COLOR_CHOICES = 10; 
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = WINDOW_WIDTH;
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = WINDOW_HEIGHT;
const float VIEW_NEAR = -400;
const float VIEW_FAR = 400;

int ver_size,tri_size;
const double scale_large = 1.1;        //value that you want to scale the object to bigger size
const double scale_small = 0.9;        //value that you want to scale the object to smaller size
const double x_translate = 10;         //value that you want to translate through x axis
const double y_translate = 10;         //value that you want to translate through y axis
const double rotate_angle = 15;      //value of angle that you want to rotate the object

// Retrieve random float
float randFloat()
{
	return rand() / static_cast<float>(RAND_MAX);
}
float randFloat(float min, float max)
{
	return randFloat() * (max - min) + min;
}

// A simple wrapper for store 3D vectors
struct Vector3
{
	float x;
	float y;
	float z;
    
	Vector3() : x(0.0), y(0.0), z(0.0)
	{}
    
	Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
	{}
    
	Vector3(const Vector3 & v)
    : x(v.x), y(v.y), z(v.z)
	{}
    
	Vector3 operator+(const Vector3 & rhs) const
	{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator-(const Vector3 & rhs) const
	{ return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3 operator*(float rhs) const
	{ return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator/(float rhs) const
	{ return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3 operator+=(const Vector3 & rhs)
	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 operator-=(const Vector3 & rhs)
	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3 operator*=(float rhs)
	{ x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vector3 operator/=(float rhs)
	{ x /= rhs; y /= rhs; z /= rhs; return *this; }
    
	float magnitude() const
	{ return sqrt(x * x + y * y + z * z); }
	void normalize()
	{ *this /= magnitude(); }
	Vector3 normalized() const
	{ return *this / magnitude(); }
	float dot(const Vector3 & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Vector3 cross(const Vector3 & rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y,
                       z * rhs.x - x * rhs.z,
                       x * rhs.y - y * rhs.x);
	}
};

// A simple wrapper to store colors
struct Color3d
{
	float r;
	float g;
	float b;
	float a;
    
	Color3d()
    : r(0.0), g(0.0), b(0.0), a(1.0)
	{}
	Color3d(float r, float g, float b, float a = 1.0)
    : r(r), g(g), b(b), a(a)
	{}
};
struct tri_struct  //used to store the position of 3 points of a triangle and thier color information
{
	int p_a;
	int p_b;
	int p_c;
	Vector3 normal;
	Color3d color;
	bool outer;

};
Vector3 light_source = Vector3(0, 600, -200);
Color3d Ld = Color3d(1, 1, 1);

float det(Vector3 v1, Vector3 v2, Vector3 v3)
{
	float a =v1.x*(v2.y*v3.z - v3.y*v2.z)-v2.x*(v1.y*v3.z - v3.y*v1.z)+v3.x*(v1.y*v2.z - v2.y*v1.z);
	return a;
}
void renderPixel(int x, int y, Color3d& color, float sz = 1.0)
{
	glPointSize(sz);
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

vector<Vector3>all_vertices;
vector<Vector3>vertex_normals;
vector<tri_struct>all_triangles;

Color3d mix[COLOR_CHOICES] = {Color3d(1.0, 1.0, 1.0),
		Color3d(1.0, 0.0, 0.0),
		Color3d(0.0, 1.0, 0.0),
		Color3d(0.0, 0.0, 1.0),
		Color3d(0.5, 1.0, 1.0),
		Color3d(1.0, 0.5, 1.0),
		Color3d(1.0, 1.0, 0.5),
		Color3d(1.0, 1.0, 0.0),
		Color3d(1.0, 0.0, 1.0),
		Color3d(0.0, 1.0, 1.0)};

struct Ray
{
    Vector3 origin;
    Vector3 direction;
    Ray() {Vector3 d(0.0, 0.0, 1.0);  direction = d;}
    Ray(const Vector3& o, const Vector3& dir)
    {
        origin = o;
        Vector3 d(0.0, 0.0, 1.0);
        float mag = dir.magnitude();
        if (mag > 0.0) {d = dir;}
        direction = d;
    }
};
/****************************************************************************************
Vector3 get_normal(tri_struct tri)
this function find a normal based on the 3 vertices of a triangles
however this function don't detect if the normal is point outer or inner
*****************************************************************************************/
Vector3 get_normal(tri_struct tri)
{
	// get 2 vector from the triangle
	Vector3 v1, v2, the_normal_vector;
	v1 = Vector3(all_vertices[tri.p_b].x-all_vertices[tri.p_a].x, 
		     all_vertices[tri.p_b].y-all_vertices[tri.p_a].y,
                     all_vertices[tri.p_b].z-all_vertices[tri.p_a].z);
	v2 = Vector3(all_vertices[tri.p_c].x-all_vertices[tri.p_a].x, 
		     all_vertices[tri.p_c].y-all_vertices[tri.p_a].y, 
                     all_vertices[tri.p_c].z-all_vertices[tri.p_a].z);
	//do cross product to get normal vector
	float a = v1.y*v2.z-v1.z*v2.y;
	float b = v1.z*v2.x-v1.x*v2.z;
	float c = v1.x*v2.y-v1.y*v2.x;

	the_normal_vector = Vector3(a, b, c);

	return the_normal_vector;
	
}
/****************************************************************************************
void find_all_normals()
this function find all the normal that point outer and sotred into the tri_struct
*****************************************************************************************/
void find_all_normals()
{
	for(int i = 0; i < tri_size; i++)
	{
		vector<int>tri_pt;
		vector<Vector3>tri_vec;
		int pt_a = all_triangles[i].p_a;
		int pt_b = all_triangles[i].p_b;
		int pt_c = all_triangles[i].p_c;
		//cout<<"~~~~~~~~~~~~~~~\n"<<tri_size<<endl<<"1: "<<pt_a<<" "<<pt_b<<" "<<pt_c<<endl;
		Vector3 v1 = all_vertices[pt_a] - all_vertices[pt_c];
		v1.normalize();
		Vector3 v2 = all_vertices[pt_b] - all_vertices[pt_a];
		v2.normalize();
		Vector3 v3 = all_vertices[pt_c] - all_vertices[pt_b];
		v3.normalize();
		for(int k = 0; k < tri_size; k++)
		{
			if(all_triangles[k].p_a == pt_a ||
			all_triangles[k].p_b == pt_a ||
			all_triangles[k].p_c == pt_a)
			{
				if(all_triangles[k].p_a != pt_c &&
				all_triangles[k].p_b != pt_c &&
				all_triangles[k].p_c != pt_c)
				{
					//cout<<"find"<<endl;
				//this triangle contained the vertex we pick
					if(all_triangles[k].outer == false )
					{
						//cout<<"2: "<<all_triangles[k].p_a<<" "<<all_triangles[k].p_b<<" "<<all_triangles[k].p_c<<endl;
						Vector3 n = get_normal(all_triangles[k]);
						n.normalize();
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<endl;
						//cout<<v1.x<<" "<<v1.y<<" "<<v1.z<<endl;
						float dot_result = v1.dot(n);
						if(dot_result < 0)
						{
							n = Vector3(0.0,0.0,0.0)-n;
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						else if(dot_result > 0)
						{
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<"**"<<endl;
					}
						
				}
			}
				
		}
		for(int k = 0; k < tri_size; k++)
		{
			if(all_triangles[k].p_a == pt_b ||
			all_triangles[k].p_b == pt_b ||
			all_triangles[k].p_c == pt_b)
			{
				if(all_triangles[k].p_a != pt_a &&
				all_triangles[k].p_b != pt_a &&
				all_triangles[k].p_c != pt_a)
				{
				//this triangle contained the vertex we pick
					if(all_triangles[k].outer == false )
					{
						//cout<<"3: "<<all_triangles[k].p_a<<" "<<all_triangles[k].p_b<<" "<<all_triangles[k].p_c<<endl;
						Vector3 n = get_normal(all_triangles[k]);
						n.normalize();
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<endl;
						//cout<<v2.x<<" "<<v2.y<<" "<<v2.z<<endl;
						float dot_result = v2.dot(n);
						if(dot_result < 0)
						{
							n = Vector3(0.0,0.0,0.0)-n;
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						else if(dot_result > 0)
						{
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<"**"<<endl;
					}
						
				}
			}
				
		}
		for(int k = 0; k < tri_size; k++)
		{
			if(all_triangles[k].p_a == pt_c ||
			all_triangles[k].p_b == pt_c ||
			all_triangles[k].p_c == pt_c)
			{
				if(all_triangles[k].p_a != pt_b &&
				all_triangles[k].p_b != pt_b &&
				all_triangles[k].p_c != pt_b)
				{
				//this triangle contained the vertex we pick
					if(all_triangles[k].outer == false )
					{
						//cout<<"4: "<<all_triangles[k].p_a<<" "<<all_triangles[k].p_b<<" "<<all_triangles[k].p_c<<endl;
						Vector3 n = get_normal(all_triangles[k]);
						n.normalize();
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<endl;
						//cout<<v3.x<<" "<<v3.y<<" "<<v3.z<<endl;
						float dot_result = v3.dot(n);
						if(dot_result < 0)
						{
							n = Vector3(0.0,0.0,0.0)-n;
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						else if(dot_result > 0)
						{
							all_triangles[k].normal = n;
							all_triangles[k].outer = true;
						}
						
						
						//cout<<n.x<<" "<<n.y<<" "<<n.z<<"**"<<endl;
					}
						
				}
			}
				
		}
	
	}
	for(int i = tri_size; i < all_triangles.size(); i++)
	{
		Vector3 n = get_normal(all_triangles[i]);
		n.normalize();
		if(n.dot(Vector3(0,0,1)) > 0)
		{
			n = Vector3(0.0, 0.0, 0.0) - n;
		}
		all_triangles[i].normal = n;
		all_triangles[i].outer = true;
	}
}
/****************************************************************************************
void find_all_normals()
this function find all the vertex normal that point outer and sotred into a vector vertex_normals
*****************************************************************************************/
void get_vertex_normals(int which_tri)
{
	Vector3 normal = all_triangles[which_tri].normal;
	if(vertex_normals[all_triangles[which_tri].p_a].x == 0 &&
		vertex_normals[all_triangles[which_tri].p_a].y == 0 &&
		vertex_normals[all_triangles[which_tri].p_a].z == 0)
	{
		Vector3 vertex_normal = Vector3(0,0,0);
		float vertex_divider = 0;
		for(int i = 0; i < tri_size; i++)
		{
			if(all_triangles[i].p_a == all_triangles[which_tri].p_a ||
			all_triangles[i].p_b == all_triangles[which_tri].p_a ||
			all_triangles[i].p_c == all_triangles[which_tri].p_a)
			{
				if(normal.dot(all_triangles[i].normal) >= 0)
				{
					vertex_normal += all_triangles[i].normal;
					vertex_divider += all_triangles[i].normal.magnitude();
				}
			}
			
		}
		vertex_normal /= vertex_divider;
		vertex_normal.normalize();
		vertex_normals[all_triangles[which_tri].p_a] = vertex_normal;
		
	}
	if(vertex_normals[all_triangles[which_tri].p_b].x == 0 &&
		vertex_normals[all_triangles[which_tri].p_b].y == 0 &&
		vertex_normals[all_triangles[which_tri].p_b].z == 0)
	{
		Vector3 vertex_normal = Vector3(0,0,0);
		float vertex_divider = 0;
		for(int i = 0; i < tri_size; i++)
		{
			if(all_triangles[i].p_a == all_triangles[which_tri].p_b ||
			all_triangles[i].p_b == all_triangles[which_tri].p_b ||
			all_triangles[i].p_c == all_triangles[which_tri].p_b)
			{
				
				if(normal.dot(all_triangles[i].normal) >= 0)
				{
					vertex_normal += all_triangles[i].normal;
					vertex_divider += all_triangles[i].normal.magnitude();
				}
			}
			
		}
		vertex_normal /= vertex_divider;
		vertex_normal.normalize();
		vertex_normals[all_triangles[which_tri].p_b] = vertex_normal;

		
	}
	if(vertex_normals[all_triangles[which_tri].p_c].x == 0 &&
		vertex_normals[all_triangles[which_tri].p_c].y == 0 &&
		vertex_normals[all_triangles[which_tri].p_c].z == 0)
	{	
		Vector3 vertex_normal = Vector3(0,0,0);
		float vertex_divider = 0;
		for(int i = 0; i < tri_size; i++)
		{
			
			if(all_triangles[i].p_a == all_triangles[which_tri].p_c ||
			all_triangles[i].p_b == all_triangles[which_tri].p_c ||
			all_triangles[i].p_c == all_triangles[which_tri].p_c)
			{
				if(normal.dot(all_triangles[i].normal) >= 0)
				{
					vertex_normal += all_triangles[i].normal;
					vertex_divider += all_triangles[i].normal.magnitude();
				}
				
			}
		}
		vertex_normal /= vertex_divider;
		vertex_normal.normalize();
		vertex_normals[all_triangles[which_tri].p_c] = vertex_normal;
	}
			
}
/****************************************************************************************
bool rayIntersectsTri(const Ray& current, const tri_struct& tri, float* t = NULL)
this function check if e-ray intersect the triangle
*****************************************************************************************/
bool rayIntersectsTri(const Ray& current, const tri_struct& tri, float* t = NULL)
{
	if(t != NULL){*t = -1.0;}
	Vector3 Eb = all_vertices[tri.p_b] - all_vertices[tri.p_a];
	Vector3 Ec = all_vertices[tri.p_c] - all_vertices[tri.p_a];
	Vector3 Rd = current.direction;
	Vector3 Ro = current.origin;
	Vector3 zero = Vector3(0.0,0.0,0.0);
	float det_t = det(zero-Eb, zero-Ec, all_vertices[tri.p_a]-Ro);
	float det_B = det(all_vertices[tri.p_a]-Ro, zero-Ec, Rd);
	float det_C = det(zero-Eb, all_vertices[tri.p_a]-Ro, Rd);
	float det_A = det(zero-Eb, zero-Ec, Rd);
	*t = det_t/det_A;
	float B = det_B/det_A;
	float r = det_C/det_A;

	if(*t > 0)
	{
		if(B>=0 && r>=0)
		{
			if(B+r <= 1)
			{
				return true;
			}
		}
	}
	return false;

}
/****************************************************************************************
int tri_pixelOn(float pixel_x, float pixel_y, float* t )
when find a triangle, return its id
*****************************************************************************************/
int tri_pixelOn(float pixel_x, float pixel_y, float* t )
{
	Ray current(Vector3(pixel_x, pixel_y, 0.0), Vector3(0.0, 0.0, 1.0));
	int num_triangles = all_triangles.size();
	float min_t = -1.0;
	int min_i = -1;
	bool found = false;
	for(int i = 0; i < num_triangles; i++)
	{
		float t1;
		if(rayIntersectsTri(current, all_triangles[i], &t1))
		{
			if(found == false)
			{
				min_t = t1;
				min_i = i;
				found = true;
			}
			else
			{
				if(t1 < min_t)
				{
					min_t = t1;
					min_i = i;
				}
				
			}	
		}		
	}
	if(min_t > 0)
	{
		*t = min_t;
	}
	return min_i;
}
/****************************************************************************************
bool shadowRayInteractsTri(const Ray& current)
tracing shadow ray to see if it interacts any triangles. return true if yes
*****************************************************************************************/
bool shadowRayInteractsTri(const Ray& current)
{
	int num_triangles = all_triangles.size();
	float min_t = -1.0;
	int min_i = -1;
	bool found = false;
	for(int i = 0; i < num_triangles; i++)
	{
		float t1;
		if(rayIntersectsTri(current, all_triangles[i], &t1))
		{
			return true;
		}	
	}
	return false;
}


void GLrender();
void get_input(int* argc, char** argv);
void rotate(double input);
void translate_x(double input);
void translate_y(double input);
void scale(double input);
void GLKeyboardPress(unsigned char key, int x, int y);


//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Lab 7 - Enter Your Name");
	get_input(argc, argv);
	glutDisplayFunc(GLrender);
	glutKeyboardFunc(GLKeyboardPress);
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, VIEW_NEAR, VIEW_FAR);
}

int main(int argc, char** argv)
{
	GLInit(&argc, argv);
    	//loadSpheres();
	glutMainLoop();
	return 0;
}
map<pair<int, int>, Vector3>y_increN_map;

/****************************************************************************************
void get_edge_normal_interpolation_y(int which_tri)
find a incremental ratio between 2 vertex normals of a tri so we can interporate normal between vertices 
*****************************************************************************************/
void get_edge_normal_interpolation_y(int which_tri)
{
	int pa = all_triangles[which_tri].p_a;
	int pb = all_triangles[which_tri].p_b;
	int pc = all_triangles[which_tri].p_c;
	//assume a triangle has vertices a, b and c.
	//find normal incremental ratio from vertex a to b 
	//increment based on y. so get nothing if a and b has same y
	if(all_vertices[pa].y > all_vertices[pb].y)
	{
		Vector3 increNormal = vertex_normals[pb] - vertex_normals[pa];
		increNormal /= 	(float)(all_vertices[pa].y - all_vertices[pb].y);
		pair<int, int>key(pa, pb);
		y_increN_map[key] = increNormal;
	}
	else if(all_vertices[pb].y > all_vertices[pa].y)
	{
		Vector3 increNormal = vertex_normals[pa] - vertex_normals[pb];
		increNormal /= 	(float)(all_vertices[pb].y - all_vertices[pa].y);
		//cout<<which_tri<<" : "<<(vertex_normals[pa] - vertex_normals[pb]).x<<" "<<(vertex_normals[pa] - vertex_normals[pb]).y<<" "<<(vertex_normals[pa] - vertex_normals[pb]).z<<" "<<all_vertices[pb].y - all_vertices[pa].y<<endl;
		pair<int, int>key(pb, pa);
		y_increN_map[key] = increNormal;
	}
	//find normal incremental ratio from vertex a to c for a triangle abc
	if(all_vertices[pa].y > all_vertices[pc].y)
	{
		Vector3 increNormal = vertex_normals[pc] - vertex_normals[pa];
		increNormal /= 	(float)(all_vertices[pa].y - all_vertices[pc].y);
		pair<int, int>key(pa, pc);
		y_increN_map[key] = increNormal;
	}
	else if(all_vertices[pc].y > all_vertices[pa].y)
	{
		Vector3 increNormal = vertex_normals[pa] - vertex_normals[pc];
		increNormal /= 	(float)(all_vertices[pc].y - all_vertices[pa].y);
		pair<int, int>key(pc, pa);
		y_increN_map[key] = increNormal;
	}
	//find normal incremental ratio from vertex b to c for a triangle abc
	if(all_vertices[pb].y > all_vertices[pc].y)
	{
		Vector3 increNormal = vertex_normals[pc] - vertex_normals[pb];
		increNormal /= 	(float)(all_vertices[pb].y - all_vertices[pc].y);
		pair<int, int>key(pb, pc);
		y_increN_map[key] = increNormal;
	}
	else if(all_vertices[pc].y > all_vertices[pb].y)
	{
		Vector3 increNormal = vertex_normals[pb] - vertex_normals[pc];
		increNormal /= 	(float)(all_vertices[pc].y - all_vertices[pb].y);
		pair<int, int>key(pc, pb);
		y_increN_map[key] = increNormal;
	}
	
	
}
/*
struct line
{
	int big;
	int small;
	float max_y;
	float min_y;
	float x_pos;
	float slope;
	bool differ;
};
*/
/****************************************************************************************
void GLrender()
scan though x and y to render each pixel using phong shading model
*****************************************************************************************/
void GLrender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for(int i = 0; i < tri_size; i++)
    {
	all_triangles[i].outer = false;
    }
    find_all_normals();
    map<int, bool>if_checked;
    for(int i = 0; i < tri_size; i++) 
    {
	if_checked[i] = false;
    }
    for (int j = 0; j < WINDOW_HEIGHT; j++)
    {
        for (int i = 0; i < WINDOW_WIDTH; i++)
        {  
	    float time;
            int which_tri = tri_pixelOn(i,800- j, &time);
            if (which_tri >= 0)
            {
		Color3d La = all_triangles[which_tri].color;
		Vector3 shadow_ray_origin = Vector3(i,800- j, time);
		Vector3 shadow_ray_direction = light_source-shadow_ray_origin;
		shadow_ray_direction.normalize();
                Color3d Illumination = Color3d();
		get_vertex_normals(which_tri);
		/*
		//-----from here, implement interpolation of normals-----
		if(if_checked[which_tri] == false && which_tri < tri_size)
		{
			cout<<which_tri<<endl;
			get_edge_normal_interpolation_y(which_tri);
			if_checked[which_tri] = true;
			pair<int, int>key(all_triangles[which_tri].p_b, all_triangles[which_tri].p_c);
			if(which_tri == 1)
			{
				cout<<y_increN_map[key].x<<" "<<y_increN_map[key].y<<" "<<y_increN_map[key].z<<endl;
			}
		}
		int pa = all_triangles[which_tri].p_a;
		int pb = all_triangles[which_tri].p_b;
		int pc = all_triangles[which_tri].p_c;
		line ab, ac, bc; 
		vector<line>picks;
		//line a-b
		if(all_vertices[pa].y > all_vertices[pb].y )
		{
			ab.big = pa;
			ab.small = pb;
			ab.max_y = all_vertices[pa].y;
			ab.min_y = all_vertices[pb].y;
			ab.slope = (all_vertices[pa].x - all_vertices[pb].x)/ab.max_y - ab.min_y;
			ab.x_pos = (all_vertices[pa].x + all_vertices[pb].x)/2;
			ab.differ = true;
			if(800-j >= ab.min_y && 800-j <= ab.max_y)
			{
				picks.push_back(ab);
			}
		}
		else if(all_vertices[pa].y < all_vertices[pb].y )
		{
			ab.big = pb;
			ab.small = pa;
			ab.max_y = all_vertices[pb].y;
			ab.min_y = all_vertices[pa].y;
			ab.slope = (all_vertices[pb].x - all_vertices[pa].x)/ab.max_y - ab.min_y;
			ab.x_pos = (all_vertices[pa].x + all_vertices[pb].x)/2;
			ab.differ = true;
			if(800-j >= ab.min_y && 800-j <= ab.max_y)
			{
				picks.push_back(ab);
			}
		}
		else
		{
			ab.differ = false;
		}
		//line a-c
		if(all_vertices[pa].y > all_vertices[pc].y )
		{
			ac.big = pa;
			ac.small = pc;
			ac.max_y = all_vertices[pa].y;
			ac.min_y = all_vertices[pc].y;
			ac.slope = (all_vertices[pa].x - all_vertices[pc].x)/ac.max_y - ac.min_y;
			ac.x_pos = (all_vertices[pa].x + all_vertices[pc].x)/2;
			ac.differ = true;
			if(800-j >= ac.min_y && 800-j <= ac.max_y)
			{
				picks.push_back(ac);
			}
		}
		else if(all_vertices[pa].y < all_vertices[pc].y )
		{
			ac.big = pc;
			ac.small = pa;
			ac.max_y = all_vertices[pc].y;
			ac.min_y = all_vertices[pa].y;
			ac.slope = (all_vertices[pc].x - all_vertices[pa].x)/ac.max_y - ac.min_y;
			ac.x_pos = (all_vertices[pc].x + all_vertices[pa].x)/2;
			ac.differ = true;
			if(800-j >= ac.min_y && 800-j <= ac.max_y)
			{
				picks.push_back(ac);
			}
		}
		else
		{
			ac.differ = false;
		}
		//line b-c
		if(all_vertices[pb].y > all_vertices[pc].y )
		{
			bc.big = pb;
			bc.small = pc;
			bc.max_y = all_vertices[pb].y;
			bc.min_y = all_vertices[pc].y;
			bc.slope = (all_vertices[pb].x - all_vertices[pc].x)/bc.max_y - bc.min_y;
			bc.x_pos = (all_vertices[pb].x + all_vertices[pc].x)/2;
			bc.differ = true;
			if(800-j >= bc.min_y && 800-j <= bc.max_y)
			{
				picks.push_back(bc);
			}
		}
		else if(all_vertices[pb].y < all_vertices[pc].y )
		{
			bc.big = pc;
			bc.small = pb;
			bc.max_y = all_vertices[pc].y;
			bc.min_y = all_vertices[pb].y;
			bc.slope = (all_vertices[pc].x - all_vertices[pb].x)/bc.max_y - bc.min_y;
			bc.x_pos = (all_vertices[pc].x + all_vertices[pb].x)/2;
			bc.differ = true;
			if(800-j >= bc.min_y && 800-j <= bc.max_y)
			{
				picks.push_back(bc);
			}
		}
		else
		{
			bc.differ = false;
		}
		if(picks.size() == 2)
		{
			if(picks[0].x_pos > picks[1].x_pos )
			{
				line tmp = picks[0];
				picks[0] = picks[1];
				picks[1] = tmp;
			}		
		}
		else
		{
			cerr<<"wrong calculation on lines picks"<<endl;
		}	
		pair<int, int>key1(picks[0].big, picks[0].small);
		pair<int, int>key2(picks[1].big, picks[1].small);
		Vector3 increNa = y_increN_map[key1];
		Vector3 increNb = y_increN_map[key2];
		Vector3 Na = vertex_normals[picks[0].big] + increNa*(picks[0].max_y - (800-j));
		Vector3 Nb = vertex_normals[picks[1].big] + increNb*(picks[1].max_y - (800-j));
		float Na_x = all_vertices[picks[0].big].x + picks[0].slope*(picks[0].max_y - (800-j));
		float Nb_x = all_vertices[picks[1].big].x + picks[1].slope*(picks[1].max_y - (800-j));
		Vector3 normal = (Na*(Nb_x - i)+Nb*(i - Nb_x))*(1/(Nb_x - Na_x));
		*/
		//done
		Vector3 normal = all_triangles[which_tri].normal;
		if(normal.dot(Vector3(0.0, 0.0, 1.0)) > 0)
		{
			normal = Vector3(0.0,0.0,0.0) - normal;
			all_triangles[which_tri].normal = normal;
		}
		
		Ray shadow_ray = Ray(shadow_ray_origin+normal*0.2, shadow_ray_direction);
		//Vector3 reflection = normal*((normal*2).dot(shadow_ray_direction)) - shadow_ray_direction;
		Vector3 h = (shadow_ray_direction+Vector3(0.0,0.0,-1.0))
				/(shadow_ray_direction+Vector3(0.0,0.0,-1.0)).magnitude();
		if(shadowRayInteractsTri(shadow_ray))
		{
			Illumination.r = Illumination.r + 0.4*La.r;
			Illumination.g = Illumination.g + 0.4*La.g;
			Illumination.b = Illumination.b + 0.4*La.b;
		}
		else
		{
			/*
			Illumination.r = 0.8*Ld.r*(shadow_ray_direction.dot(normal)) +
					0.6*Ld.r*pow((Vector3(0.0,0.0,-1.0).dot(reflection)),10.0) +
					0.4*La.r;
			Illumination.g = 0.8*Ld.g*(shadow_ray_direction.dot(normal)) +
					0.6*Ld.g*pow((Vector3(0.0,0.0,-1.0).dot(reflection)),10.0)+
					0.4*La.g;
			Illumination.b = 0.8*Ld.b*(shadow_ray_direction.dot(normal)) + 
					0.6*Ld.b*pow((Vector3(0.0,0.0,-1.0).dot(reflection)),10.0) +
					0.4*La.b;*/
			Illumination.r = 0.8*Ld.r*(shadow_ray_direction.dot(normal)) +
					0.5*Ld.r*pow(h.dot(normal),10.0) +
					0.4*La.r;
			Illumination.g = 0.8*Ld.g*(shadow_ray_direction.dot(normal)) +
					0.5*Ld.g*pow(h.dot(normal),10.0)+
					0.4*La.g;
			Illumination.b = 0.8*Ld.b*(shadow_ray_direction.dot(normal)) + 
					0.5*Ld.b*pow(h.dot(normal),10.0) +
					0.4*La.b;
			
		}
		renderPixel(i, 800-j , Illumination);
            }
        }
    } 
    for(int i = 0; i < vertex_normals.size(); i++)
    {
	cout<<i<<" : "<<vertex_normals[i].x<<" "<<vertex_normals[i].y<<" "<<vertex_normals[i].z<<" "<<endl;
    }
    glFlush();	
    glutSwapBuffers();
}

/****************************************************************************************
void get_input(int* argc, char** argv)
this get sample data from txt file
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
			all_vertices.push_back(Vector3(x, y, z));
			vertex_normals.push_back(Vector3(0,0,0));
			ss.clear();
		}

		for(int i = 0; i < triangles_num; i++)
		{
			int one, two, three;
			infile.getline(buff, BUFFER_SIZE);
			ss<<buff;
			ss>>one>>two>>three;
			tri_struct tri;
			tri.p_a = one;
			tri.p_b = two;
			tri.p_c = three;
			tri.color = Color3d(1.0, 0.0, 0.0);
			tri.outer = false;
			all_triangles.push_back(tri);
			ss.clear();
		}
		ver_size = all_vertices.size();
		tri_size = all_triangles.size();
		all_vertices.push_back(Vector3(0, 0, 0));
		all_vertices.push_back(Vector3(0, 300, 1600));
		all_vertices.push_back(Vector3(1600, 300, 1600));
		all_vertices.push_back(Vector3(1600, 0, 0));
		tri_struct tri1;
		tri1.p_a = ver_size; 
		tri1.p_b = ver_size+2; 
		tri1.p_c = ver_size+1;
		tri1.color = Color3d(0.5, 0.5, 0.5);
		tri_struct tri2;
		tri2.p_a = ver_size;
		tri2.p_b = ver_size+3; 
		tri2.p_c = ver_size+2;
		tri2.color = Color3d(0.5, 0.5, 0.5);
		all_triangles.push_back(tri1);
		all_triangles.push_back(tri2);
		
		
	}

}
/****************************************************************************************
void GLKeyboardPress(unsigned char key, int x, int y)
this function get input from keyboard
*****************************************************************************************/
void GLKeyboardPress(unsigned char key, int x, int y)
{
	if(key == 'q')	     //rotate the obejct left
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
	for(int i = 0; i < ver_size; i++)
	{
		total_x += all_vertices[i].x;
		total_z += all_vertices[i].z;
	}
	double mid_x = total_x/ver_size;
	double mid_z = total_z/ver_size;
	for(int i = 0; i < ver_size; i++)
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
	for(int i = 0; i < ver_size; i++)
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
	for(int i = 0; i < ver_size; i++)
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
	for(int i = 1; i < ver_size; i++)
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

	for(int i = 0; i < ver_size; i++)
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

