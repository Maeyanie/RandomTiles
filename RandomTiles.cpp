#include "RandomTiles.h"
#include <random>

using namespace std;

int main(int argc, char* argv[])
{
	int sizeX = 2;
	int sizeY = 2;
	filesystem::path sourcedir = "I:/3dprint/Tiles from the Crypt/Decrepit Dungeons/DECREPIT DUNGEONS KIT/Floor Tiles/OpenForge/";
	//filesystem::path sourcedir = ".";
	filesystem::path targetfile = sourcedir / "random_tile.stl";

	switch (argc) {
	case 0:
	case 1:
		break;
	case 2:
		sizeX = sizeY = atoi(argv[1]);
		break;
	case 3:
		sizeX = atoi(argv[1]);
		sizeY = atoi(argv[2]);
		break;
	case 4:
		sizeX = atoi(argv[1]);
		sizeY = atoi(argv[2]);
		sourcedir = argv[3];
		targetfile = sourcedir / "random_tile.stl";
		break;
	default:
		sizeX = atoi(argv[1]);
		sizeY = atoi(argv[2]);
		sourcedir = argv[3];
		targetfile = argv[4];
		break;
	}

	vector<filesystem::path> sources;
	for (auto const& i : filesystem::directory_iterator(sourcedir)) {
		if (i.path().extension() == ".stl") {
			cout << "Found STL file: " << i << endl;
			sources.push_back(i);
		}
	}
	cout << "Total sources: " << sources.size() << endl;
	if (sources.size() == 0) return 1;

	STL target;
	target.box(sizeX * TILESIZE, sizeY * TILESIZE, 1.0f);

	mt19937 rng(time(NULL));
	uniform_int_distribution<> rngsource(0, sources.size()-1);
	uniform_int_distribution<> rngrotate(0, 3);
	uniform_int_distribution<> rngmirror(0, 1);
	for (int x = 0; x < sizeX; x++) {
		for (int y = 0; y < sizeY; y++) {
			STL source(sources[rngsource(rng)]);
			source.rotate(rngrotate(rng));
			if (rngmirror(rng)) source.mirror();
			source.offset(x * TILESIZE, y * TILESIZE, 0.0f);
			target.merge(source);
		}
	}

	target.write(targetfile);
	return 0;
}
