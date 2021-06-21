#pragma once

struct intCoord {
	int x, y;
};

struct Tile {
    unsigned width;
    unsigned height;
    intCoord begin;
    intCoord end;
    int tileNum;
};
