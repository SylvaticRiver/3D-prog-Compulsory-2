#include "oppgave2.h"
#include <iostream>
#include "Eigen/Dense"
#include <vector>
#include <array>
#include <fstream>

using Eigen::MatrixXf;
using Eigen::VectorXf;

using namespace std;

ofstream out;
string name("C:/Users/chris/source/repos/Render5/Render4/PathFiles/verticies1.txt");
string PointPath("C:/Users/chris/source/Render5/Render4/PathFiles/points.txt");

struct Point3D {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
};

vector<Eigen::Vector2f> points = {
	{0, -1},
	{3, 1},
	{5, 3},
	{6, -3}
};

float funksjonsuttryk(float a, float b, float c, float d, float x) {
	return ((a * pow(x, 3)) + (b * pow(x, 2)) + (c * x) + d);
}

void funct(array<Point3D, 1000> verticies, VectorXf abc, float startX, float endX) {
	float x = 0;

	vector<float> Df = { startX, endX };

	float h = (Df[1] - Df[0]) / verticies.size();

	for (int i = 0; i < verticies.size(); i++) {
		x = i * h;
		float fxi = funksjonsuttryk(abc[0], abc[1], abc[2], abc[3], x);
		verticies[i].x = x;
		verticies[i].z = fxi;
		verticies[i].y = 0;
		if (i != 0) {
			float diff = verticies[i].y - verticies[i - 1].y;
			if (diff >= 0) {
				verticies[i].r = 0;
				verticies[i].g = 1;
				verticies[i].b = 0;
			}
			else {
				verticies[i].r = 1;
				verticies[i].g = 0;
				verticies[i].b = 0;
			}
		}
	}
	out.open(name);

	out << verticies.size() << endl;

	for (Point3D vertex : verticies) {
		out << vertex.x << " " << vertex.y << " " << vertex.z << endl;
	}
	out.close();
	cout << "filen " << name << "er lagret" << endl;
}

void writePointPath() {
	out.open(PointPath);

	out << points.size() << endl;

	for (VectorXf point : points) {
		out << point[0] / 8 << " " << point[1] / 8 << endl;
	}
	out.close();
	cout << "filen " << PointPath << "er lagret" << endl;
}

int main() {

	Eigen::Matrix4f A;
	Eigen::Vector4f y;

	A.setZero();

	for (int i = 0; i < points.size(); i++) {
		Eigen::Vector2f pos = points[i];
		float xPos = pos(0);
		float yPos = pos(1);
		A(i, 0) = xPos * xPos * xPos;
		A(i, 1) = xPos * xPos;
		A(i, 2) = xPos;
		A(i, 3) = 1;
		y(i) = yPos;
	}

	Eigen::Vector4f px = A.inverse() * y;

	string op1 = px(1) >= 0 ? " + " : " - ";
	string op2 = px(2) >= 0 ? " + " : " - ";
	string op3 = px(3) >= 0 ? " + " : " - ";

	cout << "funskjonen er: " << px(0) << "x^3" << op1 << px(1) << "x^2" << op2 << px(2) << "x" << op3 << px(3) << endl;

	array<Point3D, 1000> verticies = {};

	funct(verticies, px, 0, 8);
	writePointPath();

	return 0;
}