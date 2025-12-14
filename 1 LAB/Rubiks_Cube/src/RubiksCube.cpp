#include "RubiksCube.h"

int RubiksCube::seed = 12345;

int RubiksCube::randInt(int min, int max) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return min + (seed % (max - min + 1));
}

RubiksCube::RubiksCube() {
    faces = new int[54];
    for (int f = 0; f < 6; ++f)
        for (int i = 0; i < 9; ++i)
            faces[f * 9 + i] = f;
}

RubiksCube::RubiksCube(const RubiksCube& other) {
    faces = new int[54];
    memcpy(faces, other.faces, 54 * sizeof(int));
}

RubiksCube& RubiksCube::operator=(const RubiksCube& other) {
    if (this == &other) return *this;
    delete[] faces;
    faces = new int[54];
    memcpy(faces, other.faces, 54 * sizeof(int));
    return *this;
}

RubiksCube::~RubiksCube() {
    delete[] faces;
}

void RubiksCube::rotateFaceCW(int f) {
    int b = f * 9;
    int t[9];
    memcpy(t, &faces[b], 9 * sizeof(int));
    faces[b + 0] = t[6]; faces[b + 1] = t[3]; faces[b + 2] = t[0];
    faces[b + 3] = t[7]; faces[b + 4] = t[4]; faces[b + 5] = t[1];
    faces[b + 6] = t[8]; faces[b + 7] = t[5]; faces[b + 8] = t[2];
}

void RubiksCube::rotateFaceCCW(int f) {
    int b = f * 9;
    int t[9];
    memcpy(t, &faces[b], 9 * sizeof(int));
    faces[b + 0] = t[2]; faces[b + 1] = t[5]; faces[b + 2] = t[8];
    faces[b + 3] = t[1]; faces[b + 4] = t[4]; faces[b + 5] = t[7];
    faces[b + 6] = t[0]; faces[b + 7] = t[3]; faces[b + 8] = t[6];
}

void RubiksCube::updateAdjacent(int f, bool cw) {
    const int adj[6][4] = {
        {BLUE,   RED,   GREEN, ORANGE},   // Y
        {BLUE,   ORANGE, GREEN, RED},     // W
        {YELLOW, GREEN, WHITE, BLUE},     // R
        {YELLOW, BLUE,  WHITE, GREEN},    // O
        {YELLOW, RED,   WHITE, ORANGE},   // B
        {YELLOW, ORANGE,WHITE, RED}       // G
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
        for (int k = 0; k < 3; ++k) temp[k] = faces[a0 * 9 + i0[k]];
        for (int k = 0; k < 3; ++k) faces[a0 * 9 + i0[k]] = faces[a3 * 9 + i3[k]];
        for (int k = 0; k < 3; ++k) faces[a3 * 9 + i3[k]] = faces[a2 * 9 + i2[k]];
        for (int k = 0; k < 3; ++k) faces[a2 * 9 + i2[k]] = faces[a1 * 9 + i1[k]];
        for (int k = 0; k < 3; ++k) faces[a1 * 9 + i1[k]] = temp[k];
    } else {
        for (int k = 0; k < 3; ++k) temp[k] = faces[a0 * 9 + i0[k]];
        for (int k = 0; k < 3; ++k) faces[a0 * 9 + i0[k]] = faces[a1 * 9 + i1[k]];
        for (int k = 0; k < 3; ++k) faces[a1 * 9 + i1[k]] = faces[a2 * 9 + i2[k]];
        for (int k = 0; k < 3; ++k) faces[a2 * 9 + i2[k]] = faces[a3 * 9 + i3[k]];
        for (int k = 0; k < 3; ++k) faces[a3 * 9 + i3[k]] = temp[k];
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
        int c = faces[f * 9];
        for (int i = 1; i < 9; ++i)
            if (faces[f * 9 + i] != c) return false;
    }
    return true;
}

bool RubiksCube::operator==(const RubiksCube& o) const {
    return memcmp(faces, o.faces, 54 * sizeof(int)) == 0;
}

bool RubiksCube::operator!=(const RubiksCube& o) const {
    return !(*this == o);
}

int RubiksCube::loadFromFile(const char* filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return -1;

    int temp[54];
    char buf[256];
    for (int line = 0; line < 6; ++line) {
        if (!f.getline(buf, sizeof(buf))) return -2;
        char* tok = strtok(buf, " \t\r\n");
        for (int i = 0; i < 9; ++i) {
            if (!tok) return -2;
            int v = atoi(tok);
            if (v < 0 || v > 5) return -2;
            temp[line * 9 + i] = v;
            tok = strtok(nullptr, " \t\r\n");
        }
        if (tok) return -2;
    }

    int cnt[6] = {0};
    for (int i = 0; i < 54; ++i) cnt[temp[i]]++;
    for (int c = 0; c < 6; ++c) if (cnt[c] != 9) return -2;

    memcpy(faces, temp, 54 * sizeof(int));
    return 0;
}

void RubiksCube::randomize(int n) {
    if (n < 0) n = 0;
    for (int i = 0; i < n; ++i) {
        int f = randInt(0, 5);
        bool cw = (randInt(0, 1) == 0);
        rotateFace(f, cw);
    }
}

std::ostream& operator<<(std::ostream& os, const RubiksCube& c) {
    const char* names[] = {"Жёлтая", "Белая", "Красная", "Оранжевая", "Синяя", "Зелёная"};
    const char colorChar[6] = {'Y','W','R','O','B','G'};
    for (int f = 0; f < 6; ++f) {
        os << "\n=== " << names[f] << " ===\n";
        for (int r = 0; r < 3; ++r) {
            for (int k = 0; k < 3; ++k) {
                int idx = f * 9 + r * 3 + k;
                os << colorChar[c.faces[idx]] << " ";
            }
            os << "\n";
        }
    }
    return os;
}
