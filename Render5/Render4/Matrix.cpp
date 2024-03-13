#include <iostream>
#include "Eigen/Dense"
#include <vector>
#include <array>
#include <fstream>

using Eigen::MatrixXf;
using Eigen::VectorXf;

using namespace std;

ofstream out;
string name("C:/Users/chris/source/repos/Render5/Render4/PathFiles/verticies2.txt");
string PointPath("C:/Users/chris/source/repos/Render5/Render4/PathFiles/points.txt");

struct Point3D {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
};

vector<Eigen::Vector2f> points = {
	{ 1, 1 },
	{ 2, 4 },
	{ 4, 3 },
	{ 6, 3 },
};

float andregradsfunksjon(float a, float b, float c, float x) {
	return (a * pow(x, 2)) + (b * x) + c;
}

array<vector<float>, 1000> CalcGraph(VectorXf abc, float startX, float endX, int oppdeling) {
	float deltaX = (startX - endX) / oppdeling;

	array<vector<float>, 1000> points;

	for (int i = 0; i < 1000; i++) {
		vector<float> pos = {deltaX * i, andregradsfunksjon(abc[0], abc[1], abc[2], deltaX * i), 0, 0, 0 };
		points[i] = pos;
	}

	for (int k = 0; k < points.size() - 1; k++) {
		vector<float> vertex = points[k];
		cout << vertex[0] << " " << vertex[1] << " " << vertex[2] << " " << vertex[3] << " " << vertex[4] << " " << vertex[5] << endl;
	}

	return points;
}
//denne koden er gjenbrukt fra Oblig 1
void funct(array<Point3D, 1000> verticies, VectorXf abc, float startX, float endX) {
	float x = 0;

	vector<float> Df = { startX, endX };

	float h = (Df[1] - Df[0]) / verticies.size();

	for (int i = 0; i < verticies.size(); i++) {
		x = i * h;
		float fxi = andregradsfunksjon(abc[0], abc[1], abc[2], x);
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
		out << point[0] / 20 << " " << point[1] / 20 << endl;
	}
	out.close();
	cout << "filen " << PointPath << "er lagret" << endl;
}

int main()
{
	//8 datapunkter for en andregradfunskjon blir en 8x3 matrise
	MatrixXf A(8, 3);
	VectorXf y(8);

	A.setZero();

	//itererer gjennom alle vektorene i punktene skrevet opp og setter dem opp i A matrisen og y vectoren;
	for (int i = 0; i < points.size(); i++) {
		Eigen::Vector2f pos = points[i];
		float xPos = pos(0);
		float yPos = pos(1);
		A(i, 0) = xPos * xPos;
		A(i, 1) = xPos;
		A(i, 2) = 1;
		y(i) = yPos;
	}

	//finner B matrisen ved a multplisere A transponert med A
	MatrixXf B = A.transpose() * A;

	//finner vekotrene c og x
	VectorXf c = A.transpose() * y;
	VectorXf x = B.inverse() * c;

	string op1 = x(1) >= 0 ? " + " : " - ";
	string op2 = x(2) >= 0 ? " + " : " - ";

	cout << "funskjonen er: " << x(0) << "x^2" << op1 << x(1) << "x" << op2 << x(2) << endl;

	array<Point3D, 1000> verticies = {};

	funct(verticies, x, 0, 20);
	writePointPath();

	return 0;
}
