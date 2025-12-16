/**
 * @file RubiksCube.h
 * @author [Ваше Имя], БГУИР, ППОИС, лаб. №11, вар. 1.5.3
 * @brief Реализация класса "Кубик Рубика" (3×3×3)
 * @details Поддерживает случайное перемешивание, загрузку из файла,
 * поворот граней и проверку собранности.
 * @see RubiksCube.cpp
 */

#ifndef RUBIKS_CUBE_H
#define RUBIKS_CUBE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

class RubiksCube {
public:
    static const int WHITE   = 0;
    static const int YELLOW  = 1;
    static const int RED     = 2;
    static const int ORANGE  = 3;
    static const int BLUE    = 4;
    static const int GREEN   = 5;

    RubiksCube();
    RubiksCube(const RubiksCube& other);
    RubiksCube& operator=(const RubiksCube& other);
    ~RubiksCube();

    bool loadFromFile(const string& filename);
    void randomize(int moves = 100);
    void rotateFace(int face, bool clockwise = true);
    bool isSolved() const;

    // Доступ к ячейкам (для тестов)
    int getCell(int face, int row, int col) const;

    // Операторы сравнения
    bool operator==(const RubiksCube& other) const;
    bool operator!=(const RubiksCube& other) const;

    // Потоковые операторы
    friend ostream& operator<<(ostream& os, const RubiksCube& cube);
    friend istream& operator>>(istream& is, RubiksCube& cube);

private:
    int* cells; // 6 * 9 = 54

    void copyCells(const int* src);
    void rotateFaceCW(int face);
    void rotateFaceCCW(int face);
    void updateAdjacent(int face, bool clockwise);
    bool validateData() const;

    static int seed;
    static int randInt(int min, int max);
};

#endif
