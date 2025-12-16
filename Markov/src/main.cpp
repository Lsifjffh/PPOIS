#include "Markov.h"
#include <windows.h>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    Markov m;

    // Загрузка из файла: первая строка — лента, остальные — правила
    if (m.loadFromFile("program.txt") != 0) {
        cout << "Error: Cannot load program.txt\n";
        return 1;
    }

    bool log = (argc > 1 && strcmp(argv[1], "-log") == 0);
    m.run(log);

    cout << "\nResult: '" << m.getTape() << "'\n";
    return 0;
}