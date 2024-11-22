// RandomTiles.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <filesystem>

#define TILESIZE (25.4f)

struct Point {
	float x, y, z;

	inline Point() { x = y = z = 0.0f; }
	inline Point(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}

	inline Point& operator=(const Point& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	friend std::ostream& operator<<(std::ostream& os, const Point& point);
};
static inline std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << p.x << "," << p.y << "," << p.z;
	return os;
}

struct Tri {
	Point p[3];

	inline Tri() {}
	inline Tri(Point points[3]) {
		memcpy(p, points, 3 * sizeof(Point));
	}
	inline Tri(Point a, Point b, Point c) {
		p[0] = a;
		p[1] = b;
		p[2] = c;
	}
	
	inline Tri& operator=(const Tri& rhs) {
		memcpy(p, rhs.p, 3 * sizeof(Point));
		return *this;
	}
};

class STL {
public:
	std::list<Tri> mesh;

	STL() {}
	STL(std::filesystem::path file);
	STL(STL& rhs) {
		mesh = rhs.mesh;
	}

	STL& operator=(STL& rhs) {
		mesh = rhs.mesh;
		return *this;
	}

	void write(std::filesystem::path file);

	void box(float sizeX, float sizeY, float sizeZ);
	void offset(float x, float y, float z);
	void rotate();
	void rotate(int times);
	void mirror();
	void merge(STL& stl);
	void merge(STL* stl);
};


