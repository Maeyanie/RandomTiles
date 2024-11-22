#include "RandomTiles.h"
#include <fstream>

using namespace std;
using namespace std::filesystem;

struct STLEntry {
	Point nrm;
	Tri tri;
	uint16_t att;
};

STL::STL(std::filesystem::path path) {
	ifstream in(path, ifstream::in | ifstream::binary);

	// Header
	in.seekg(80);

	uint32_t tricount = 0;
	if (!in.read(reinterpret_cast<char*>(&tricount), 4)) {
		cerr << "Truncated file reading triangle count from: " << path << endl;
		exit(__LINE__);
	}

	bool first = true;
	STLEntry entry;
	Point min;
	min.x = min.y = min.z = 0.0f;

	for (uint32_t i = 0; i < tricount; i++) {
		if (!in.read(reinterpret_cast<char*>(&entry), 50)) {
			cerr << "Truncated file reading triangle " << i << " from: " << path << endl;
			exit(__LINE__);
		}

		mesh.push_back(entry.tri);

		if (first) {
			min.x = entry.tri.p[0].x;
			min.y = entry.tri.p[0].y;
			min.z = entry.tri.p[0].z;
			first = false;
		}

		for (int j = 0; j < 3; j++) {
			if (entry.tri.p[j].x < min.x) min.x = entry.tri.p[j].x;
			if (entry.tri.p[j].y < min.y) min.y = entry.tri.p[j].y;
			if (entry.tri.p[j].z < min.z) min.z = entry.tri.p[j].z;
		}
	}
	cout << "Loaded file " << path << " with " << tricount << " tris." << endl;

	for (auto& i : mesh) {
		for (int j = 0; j < 3; j++) {
			i.p[j].x -= min.x;
			i.p[j].y -= min.y;
			i.p[j].z -= min.z;
		}
	}
	cout << "Applied offset: " << min << endl;
}

void STL::write(filesystem::path file) {
	ofstream out(file, ofstream::out | ofstream::binary);

	char header[80];
	memset(header, 0, 80);
	out.write(header, 80);

	uint32_t tricount = (uint32_t)mesh.size();
	out.write(reinterpret_cast<char*>(&tricount), 4);

	STLEntry entry;
	memset(&entry, 0, sizeof(STLEntry));
	for (auto& i : mesh) {
		entry.tri = i;
		out.write(reinterpret_cast<char*>(&entry), 50);
	}
}

void STL::box(float x, float y, float z) {
	// Front
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ x, 0, 0 }), Point({ x, 0, z })));
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ x, 0, z }), Point({ 0, 0, z })));

	// Back
	mesh.emplace_back(Tri(Point({ 0, y, 0 }), Point({ x, y, z }), Point({ x, y, 0 })));
	mesh.emplace_back(Tri(Point({ 0, y, 0 }), Point({ 0, y, z }), Point({ x, y, z })));

	// Left
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ 0, y, z }), Point({ 0, y, 0 })));
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ 0, 0, z }), Point({ 0, y, z })));

	// Right
	mesh.emplace_back(Tri(Point({ x, 0, 0 }), Point({ x, y, 0 }), Point({ x, y, z })));
	mesh.emplace_back(Tri(Point({ x, 0, 0 }), Point({ x, y, z }), Point({ x, 0, z })));

	// Bottom
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ x, 0, 0 }), Point({ x, y, 0 })));
	mesh.emplace_back(Tri(Point({ 0, 0, 0 }), Point({ x, y, 0 }), Point({ 0, y, 0 })));

	// Top
	mesh.emplace_back(Tri(Point({ 0, 0, z }), Point({ x, y, z }), Point({ x, 0, z })));
	mesh.emplace_back(Tri(Point({ 0, 0, z }), Point({ 0, y, z }), Point({ x, y, z })));
}

void STL::offset(float x, float y, float z) {
	for (auto& i : mesh) {
		for (int j = 0; j < 3; j++) {
			i.p[j].x += x;
			i.p[j].y += y;
			i.p[j].z += z;
		}
	}
}

void STL::rotate() {
	for (auto& i : mesh) {
		for (int j = 0; j < 3; j++) {
			float x = i.p[j].x;
			float y = i.p[j].y;
			i.p[j].x = TILESIZE - y;
			i.p[j].y = x;
		}
	}
}
void STL::rotate(int times) {
	for (int i = 0; i < times; i++) rotate();
}

void STL::mirror() {
	for (auto& i : mesh) {
		for (int j = 0; j < 3; j++) {
			i.p[j].x = TILESIZE - i.p[j].x;
		}
		Point temp = i.p[1];
		i.p[1] = i.p[2];
		i.p[2] = temp;
	}
}

void STL::merge(STL& other) {
	mesh.splice(mesh.end(), other.mesh);
}
void STL::merge(STL* other) {
	mesh.splice(mesh.end(), other->mesh);
}