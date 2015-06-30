#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "point2d.h"

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int COLOR_CHOICES = 8;
const int BUFF_SIZE = 80;

struct point3d
{
	double x;
	double y;
	double z;

	point3d() : x(0.0), y(0.0), z(0.0) {}
	point3d(const double & nx, const double & ny, const double & nz) : x(nx), y(ny), z(nz) {}
};
struct tri_struct  //used to store the 3 points of a triangle and the color information
{
	int a;
	int b;
	int c;
	float red;
	float green;
	float blue;
};

struct line_data    //a structure used to store 3 lines that forms a triangles
{
	double mx;
	double my;
	double constant;
	double max_y;
	double min_y;
};

struct colors
{
	float r;
	float g;
	float b;
	colors() : r(1.0), g(1.0), b(1.0) {};
	colors(const float & nr, const float & ng, const float & nb):r(nr), g(ng), b(nb){};
};

vector<point3d>all_vertices;          //store all the vertices

vector<tri_struct>all_triangles;      //store all the triangels;

int main(int argc, char** argv)
{	
	if(argc > 1)
	{
		char buff[BUFF_SIZE];
		int vertices_num; 
		int triangles_num;
		stringstream ss;
		ifstream infile;
		infile.open(argv[1]);
		infile.getline(buff, BUFF_SIZE);
		ss<<buff;
		ss>>vertices_num>>triangles_num;
		ss.clear();
		int counter = 0;
		for(int i = 0; i < vertices_num; i++)
		{
			int x, y, z;
			infile.getline(buff, BUFF_SIZE);
			ss<<buff;
			ss>>x>>y>>z;
			all_vertices.push_back(point3d(x, y, z));
			ss.clear();
		}

		for(int i = 0; i < triangles_num; i++)
		{
			int one, two, three;
			infile.getline(buff, BUFF_SIZE);
			ss<<buff;
			ss>>one>>two>>three;
			tri_struct tri;
			tri.a = one;
			tri.b = two;
			tri.c = three;
			if(counter < COLOR_CHOICES)
			{
				tri.red = 0.0;
				tri.green = 0.0;
				tri.blue = 0.0;
				counter++;
			}
			else
			{
				counter = 0;	
				tri.red = 0.0;
				tri.green = 0.0;
				tri.blue = 0.0;
				counter++;
			}
			all_triangles.push_back(tri);
			ss.clear();
		}
		cout<<vertices_num<<" "<<triangles_num<<endl;
		for(int i = 0; i < vertices_num; i++)
		{
			cout<<all_vertices[i].x<<" "<<all_vertices[i].y<<" "<<all_vertices[i].z<<endl;
		}
		for(int i = 0; i < triangles_num; i++)
		{	
			cout<<all_triangles[i].a<<" "<<all_triangles[i].b<<" "<<all_triangles[i].c<<endl;
		}
			
			
	}	

	return 0;
}
