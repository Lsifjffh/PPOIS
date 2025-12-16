#include "RubiksCube.h"
using namespace std;

int main() {
    RubiksCube cube;
    int choice, face, dir, n;
    string filename;

    cout << "=== КУБИК РУБИКА ===\n";

    do {
        cout << "\n1. Показать\n2. Перемешать\n3. Загрузить\n4. Повернуть\n5. Собран?\n0. Выход\n> ";
        cin >> choice;

        switch (choice) {
        case 1: cout << cube; break;
        case 2:
            cin >> n;
            cube.randomize(n);
            break;
        case 3:
            cin >> filename;
            if (cube.loadFromFile(filename)) {
                cout << "OK\n";
            } else {
                cout << "ERR\n";
            }
            break;
        case 4:
            cin >> face >> dir;
            if (face >= 0 && face < 6 && (dir == 0 || dir == 1)) {
                cube.rotateFace(face, dir == 0);
            }
            break;
        case 5:
            cout << (cube.isSolved() ? "YES\n" : "NO\n");
            break;
        case 0:
            break;
        default:
            break;
        }
    } while (choice != 0);

    return 0;
}