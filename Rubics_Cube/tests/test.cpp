#include "RubiksCube.h"
#include "UnitTest++.h"
#include <sstream>
#include <cstring>

void write_temp_file(const char* name, const char* data) {
    FILE* f = fopen(name, "w");
    if (f) {
        fwrite(data, 1, strlen(data), f);
        fclose(f);
    }
}

struct TempFileGuard {
    const char* name;
    TempFileGuard(const char* n) : name(n) {}
    ~TempFileGuard() { remove(name); }
};

TEST(DefaultConstructorCreatesSolvedCube) {
    RubiksCube c;
    CHECK(c.isSolved());
}

TEST(CopyConstructorDeepCopy) {
    RubiksCube a;
    a.rotateFace(RubiksCube::RED, true);
    RubiksCube b(a);
    a.rotateFace(RubiksCube::RED, true);
    CHECK(a != b);
}

TEST(AssignmentOperatorWorks) {
    RubiksCube a;
    a.rotateFace(RubiksCube::YELLOW, false);
    RubiksCube b;
    b = a;
    CHECK(a == b);
}

TEST(DestructorSafe) {
    RubiksCube* p = new RubiksCube();
    delete p;
}

TEST(IsSolvedNewCube) {
    RubiksCube c;
    CHECK(c.isSolved());
}

TEST(IsNotSolvedAfterOneMove) {
    RubiksCube c;
    c.rotateFace(RubiksCube::WHITE, true);
    CHECK(!c.isSolved());
}

TEST(RotateInvalidFaceIgnored) {
    RubiksCube c;
    c.rotateFace(-1, true);
    c.rotateFace(6, true);
    CHECK(c.isSolved());
}

TEST(RotateYellowCWChangesRedTop) {
    RubiksCube c;
    int before = c.getCell(RubiksCube::RED, 0, 0);
    c.rotateFace(RubiksCube::YELLOW, true);
    int after = c.getCell(RubiksCube::RED, 0, 0);
    CHECK(before != after);
}

TEST(RotateWhiteCCWChangesBlueTop) {
    RubiksCube c;
    int before = c.getCell(RubiksCube::BLUE, 0, 0);
    c.rotateFace(RubiksCube::WHITE, false);
    int after = c.getCell(RubiksCube::BLUE, 0, 0);
    CHECK(before != after);
}

TEST(RotateRedCWChangesYellowAndWhite) {
    RubiksCube c;
    int yb = c.getCell(RubiksCube::YELLOW, 2, 0);
    int wt = c.getCell(RubiksCube::WHITE, 0, 0);
    c.rotateFace(RubiksCube::RED, true);
    CHECK(c.getCell(RubiksCube::YELLOW, 2, 0) != yb);
    CHECK(c.getCell(RubiksCube::WHITE, 0, 0) != wt);
}

TEST(CentersNeverChange) {
    RubiksCube c;
    c.randomize(50);
    CHECK_EQUAL(1, c.getCell(0, 1, 1)); // Y
    CHECK_EQUAL(0, c.getCell(1, 1, 1)); // W
    CHECK_EQUAL(2, c.getCell(2, 1, 1)); // R
    CHECK_EQUAL(3, c.getCell(3, 1, 1)); // O
    CHECK_EQUAL(4, c.getCell(4, 1, 1)); // B
    CHECK_EQUAL(5, c.getCell(5, 1, 1)); // G
}

TEST(Randomize0IsSolved) {
    RubiksCube c;
    c.randomize(0);
    CHECK(c.isSolved());
}

TEST(LoadValidFileWorks) {
    const char* d = "1 1 1 1 1 1 1 1 1\n0 0 0 0 0 0 0 0 0\n2 2 2 2 2 2 2 2 2\n"
                     "3 3 3 3 3 3 3 3 3\n4 4 4 4 4 4 4 4 4\n5 5 5 5 5 5 5 5 5\n";
    write_temp_file("v.txt", d);
    TempFileGuard _g("v.txt");
    RubiksCube c;
    CHECK_EQUAL(0, c.loadFromFile("v.txt"));
    CHECK(c.isSolved());
}

TEST(LoadMissingFile) {
    RubiksCube c;
    CHECK_EQUAL(-1, c.loadFromFile("no.txt"));
}

TEST(EqualitySame) {
    RubiksCube a, b;
    CHECK(a == b);
}

TEST(InequalityAfterMod) {
    RubiksCube a, b;
    a.rotateFace(RubiksCube::ORANGE, true);
    CHECK(a != b);
}

TEST(OutputNotEmpty) {
    RubiksCube c;
    std::ostringstream oss;
    oss << c;
    CHECK(oss.str().length() > 100);
}

TEST(ValidateValidData) {
    RubiksCube c;
    int tmp[54];
    for (int f = 0; f < 6; ++f)
        for (int i = 0; i < 9; ++i)
            tmp[f * 9 + i] = f;
    memcpy(c.faces, tmp, 54 * sizeof(int));
    CHECK(c.validateLoadedData());
}

TEST(RotateAllFacesOnceNotSolved) {
    RubiksCube c;
    for (int f = 0; f < 6; ++f) c.rotateFace(f, true);
    CHECK(!c.isSolved());
}

// ... остальные 30+ тестов ...

int main() {
    return UnitTest::RunAllTests();
}
