#include "RubiksCube.h"
using namespace std;

int RubiksCube::seed = 12345;

int RubiksCube::randInt(int min, int max) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return min + (seed % (max - min + 1));
}

RubiksCube::RubiksCube() {
    cells = new int[54];
    for (int f = 0; f < 6; ++f)
        for (int i = 0; i < 9; ++i)
            cells[f * 9 + i] = f;
}

RubiksCube::RubiksCube(const RubiksCube& other) {
    cells = new int[54];
    copyCells(other.cells);
}

RubiksCube& RubiksCube::operator=(const RubiksCube& other) {
    if (this == &other) return *this;
    delete[] cells;
    cells = new int[54];
    copyCells(other.cells);
    return *this;
}

RubiksCube::~RubiksCube() {
    delete[] cells;
}

void RubiksCube::copyCells(const int* src) {
    for (int i = 0; i < 54; ++i) cells[i] = src[i];
}

void RubiksCube::rotateFaceCW(int f) {
    int b = f * 9;
    int t[9];
    for (int i = 0; i < 9; ++i) t[i] = cells[b + i];
    cells[b + 0] = t[6]; cells[b + 1] = t[3]; cells[b + 2] = t[0];
    cells[b + 3] = t[7]; cells[b + 4] = t[4]; cells[b + 5] = t[1];
    cells[b + 6] = t[8]; cells[b + 7] = t[5]; cells[b + 8] = t[2];
}

void RubiksCube::rotateFaceCCW(int f) {
    int b = f * 9;
    int t[9];
    for (int i = 0; i < 9; ++i) t[i] = cells[b + i];
    cells[b + 0] = t[2]; cells[b + 1] = t[5]; cells[b + 2] = t[8];
    cells[b + 3] = t[1]; cells[b + 4] = t[4]; cells[b + 5] = t[7];
    cells[b + 6] = t[0]; cells[b + 7] = t[3]; cells[b + 8] = t[6];
}

void RubiksCube::updateAdjacent(int f, bool cw) {
    const int adj[6][4] = {
        {BLUE,   RED,   GREEN, ORANGE},
        {BLUE,   ORANGE, GREEN, RED},
        {YELLOW, GREEN, WHITE, BLUE},
        {YELLOW, BLUE,  WHITE, GREEN},
        {YELLOW, RED,   WHITE, ORANGE},
        {YELLOW, ORANGE,WHITE, RED}
    };

    const int idx[6][4][3] = {
        {{6,7,8}, {0,3,6}, {2,1,0}, {8,5,2}},
        {{2,1,0}, {0,3,6}, {6,7,8}, {8,5,2}},
        {{6,7,8}, {2,5,8}, {2,1,0}, {6,3,0}},
        {{6,7,8}, {6,3,0}, {2,1,0}, {2,5,8}},
        {{6,7,8}, {2,5,8}, {2,1,0}, {6,3,0}},
        {{6,7,8}, {6,3,0}, {2,1,0}, {2,5,8}}
    };

    int a0 = adj[f][0], a1 = adj[f][1], a2 = adj[f][2], a3 = adj[f][3];
    const int* i0 = idx[f][0];
    const int* i1 = idx[f][1];
    const int* i2 = idx[f][2];
    const int* i3 = idx[f][3];

    int temp[3];
    if (cw) {
        for (int k = 0; k < 3; ++k) temp[k] = cells[a0 * 9 + i0[k]];
        for (int k = 0; k < 3; ++k) cells[a0 * 9 + i0[k]] = cells[a3 * 9 + i3[k]];
        for (int k = 0; k < 3; ++k) cells[a3 * 9 + i3[k]] = cells[a2 * 9 + i2[k]];
        for (int k = 0; k < 3; ++k) cells[a2 * 9 + i2[k]] = cells[a1 * 9 + i1[k]];
        for (int k = 0; k < 3; ++k) cells[a1 * 9 + i1[k]] = temp[k];
    } else {
        for (int k = 0; k < 3; ++k) temp[k] = cells[a0 * 9 + i0[k]];
        for (int k = 0; k < 3; ++k) cells[a0 * 9 + i0[k]] = cells[a1 * 9 + i1[k]];
        for (int k = 0; k < 3; ++k) cells[a1 * 9 + i1[k]] = cells[a2 * 9 + i2[k]];
        for (int k = 0; k < 3; ++k) cells[a2 * 9 + i2[k]] = cells[a3 * 9 + i3[k]];
        for (int k = 0; k < 3; ++k) cells[a3 * 9 + i3[k]] = temp[k];
    }
}

void RubiksCube::rotateFace(int f, bool cw) {
    if (f < 0 || f > 5) return;
    if (cw) rotateFaceCW(f);
    else rotateFaceCCW(f);
    updateAdjacent(f, cw);
}

bool RubiksCube::isSolved() const {
    for (int f = 0; f < 6; ++f) {
        int c = cells[f * 9];
        for (int i = 1; i < 9; ++i)
            if (cells[f * 9 + i] != c) return false;
    }
    return true;
}

int RubiksCube::getCell(int face, int row, int col) const {
    return cells[face * 9 + row * 3 + col];
}

bool RubiksCube::operator==(const RubiksCube& o) const {
    for (int i = 0; i < 54; ++i)
        if (cells[i] != o.cells[i]) return false;
    return true;
}

bool RubiksCube::operator!=(const RubiksCube& o) const {
    return !(*this == o);
}

bool RubiksCube::validateData() const {
    int cnt[6] = {0};
    for (int i = 0; i < 54; ++i) {
        if (cells[i] < 0 || cells[i] > 5) return false;
        cnt[cells[i]]++;
    }
    for (int c = 0; c < 6; ++c)
        if (cnt[c] != 9) return false;
    return true;
}

bool RubiksCube::loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return false;

    int temp[54];
    string line;
    for (int i = 0; i < 6; ++i) {
        if (!getline(f, line)) return false;
        stringstream ss(line);
        for (int j = 0; j < 9; ++j) {
            if (!(ss >> temp[i * 9 + j])) return false;
            if (temp[i * 9 + j] < 0 || temp[i * 9 + j] > 5) return false;
        }
    }

    for (int i = 0; i < 54; ++i) cells[i] = temp[i];
    return validateData();
}

void RubiksCube::randomize(int n) {
    if (n < 0) n = 0;
    for (int i = 0; i < n; ++i) {
        int f = randInt(0, 5);
        bool cw = (randInt(0, 1) == 0);
        rotateFace(f, cw);
    }
}

ostream& operator<<(ostream& os, const RubiksCube& c) {
    const char* names[] = {"Y", "W", "R", "O", "B", "G"};
    for (int f = 0; f < 6; ++f) {
        os << "Face " << f << " (" << names[f] << "):\n";
        for (int r = 0; r < 3; ++r) {
            for (int k = 0; k < 3; ++k) {
                os << names[c.cells[f * 9 + r * 3 + k]] << " ";
            }
            os << "\n";
        }
    }
    return os;
}

istream& operator>>(istream& is, RubiksCube& c) {
    for (int i = 0; i < 54; ++i) {
        is >> c.cells[i];
    }
    return is;
}