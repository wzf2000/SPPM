#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "renderer.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3 && argc != 4 && argc != 5 && argc != 6) {
        cout << "Usage: ./bin/Project <input scene file> <output bmp file> [num of rounds] [num of photons] [checkpoint interval]" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    SceneParser sceneParser(inputFile.c_str());
    Renderer renderer(&sceneParser);
    int numRounds = (argc <= 3) ? 2500 : stoi(string(argv[3]));
    int numPhotons = (argc <= 4) ? 200000 : stoi(string(argv[4]));
    int ckpt_interval = (argc <= 5) ? 50 : stoi(string(argv[5]));
    renderer.render(numRounds, numPhotons, ckpt_interval, outputFile);
    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

