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

    if (argc != 3 && argc != 4) {
        cout << "Usage: ./bin/Project <input scene file> <output bmp file> [num of rounds]" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    SceneParser sceneParser(inputFile.c_str());
    Renderer renderer(&sceneParser);
    int numRounds = (argc == 3) ? 2500 : stoi(string(argv[3]));
    renderer.render(numRounds, outputFile);
    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

