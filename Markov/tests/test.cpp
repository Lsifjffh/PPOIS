#include "Markov.h"
#include "UnitTest++.h"
#include <fstream>
#include <cstring>

// Вспомогательная функция: запись временного файла
void write_temp_file(const char* name, const char* data) {
    std::ofstream f(name);
    f << data;
    f.close();
}

// RAII-обёртка для удаления файла после теста
struct TempFileGuard {
    const char* name;
    TempFileGuard(const char* n) : name(n) {}
    ~TempFileGuard() { std::remove(name); }
};

// ——— 1. Конструктор и базовое состояние ———
TEST(Constructor_CreatesEmptyTape) {
    Markov m;
    CHECK(strcmp(m.getTape(), "") == 0);
}

TEST(Constructor_HasZeroRules) {
    Markov m;
    // Проверим через вызов printRules — не упадёт и не выведет правила
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    m.printRules();
    std::cout.rdbuf(old);
    CHECK(oss.str().find("No rules") != std::string::npos);
}

// ——— 2. Добавление правил ———
TEST(AddRule_ValidRule_AddsSuccessfully) {
    Markov m;
    CHECK(m.addRule("a", "b", false) == 0);
    m.setTape("a");
    m.step();
    CHECK(strcmp(m.getTape(), "b") == 0);
}

TEST(AddRule_EmptyLeftPart_ReturnsError) {
    Markov m;
    CHECK(m.addRule("", "b", false) == -1);
}

TEST(AddRule_TooManyRules_ReturnsError) {
    Markov m;
    for (int i = 0; i < 32; ++i) {
        CHECK(m.addRule("a", "b", false) == 0);
    }
    CHECK(m.addRule("x", "y", false) == -1);
}

// ——— 3. Загрузка из файла ———
TEST(LoadFromFile_ValidFile_LoadsRulesAndTape) {
    const char* data =
        "abc\n"
        "a -> b\n"
        "b -> c\n"
        "c -> .\n";
    write_temp_file("t1.txt", data);
    TempFileGuard _g("t1.txt");

    Markov m;
    CHECK(m.loadFromFile("t1.txt") == 0);
    CHECK(strcmp(m.getTape(), "abc") == 0);
}

TEST(LoadFromFile_MissingFile_ReturnsError) {
    Markov m;
    CHECK(m.loadFromFile("___no_such_file___") == -1);
}

TEST(LoadFromFile_InvalidRuleFormat_IgnoresBadLines) {
    const char* data =
        "start\n"
        "a -> b\n"
        "bad line without arrow\n"
        "c -> .\n";
    write_temp_file("t2.txt", data);
    TempFileGuard _g("t2.txt");

    Markov m;
    CHECK(m.loadFromFile("t2.txt") == 0);
    // Должно загрузить 2 правила (a->b и c->.)
    m.setTape("a");
    m.step(); // a->b
    m.step(); // нет правила для b → останов
    CHECK(strcmp(m.getTape(), "b") == 0);
}

// ——— 4. Применение правил ———
TEST(Step_AppliesFirstMatchingRule) {
    Markov m;
    m.addRule("a", "x", false);
    m.addRule("a", "y", false); // должен примениться первый
    m.setTape("a");
    m.step();
    CHECK(strcmp(m.getTape(), "x") == 0);
}

TEST(Step_NoMatchingRule_Stops) {
    Markov m;
    m.addRule("x", "y", false);
    m.setTape("a");
    CHECK(m.step() == 1); // 1 = завершено
    CHECK(strcmp(m.getTape(), "a") == 0);
}

TEST(FinalRule_StopsExecution) {
    Markov m;
    m.addRule("a", "b", true); // финальное
    m.setTape("a");
    m.step();
    CHECK(m.isFinished());
    CHECK(strcmp(m.getTape(), "b") == 0);
}

// ——— 5. Интерпретация (run) ———
TEST(Run_SimpleChain_Completes) {
    Markov m;
    m.addRule("a", "b", false);
    m.addRule("b", "c", false);
    m.addRule("c", "", true);
    m.setTape("a");
    CHECK(m.run(false) == 0); // 0 = успех
    CHECK(strcmp(m.getTape(), "") == 0);
}

TEST(Run_TooManySteps_ReturnsError) {
    Markov m;
    m.addRule("a", "aa", false); // экспоненциальный рост
    m.setTape("a");
    CHECK(m.run(false) == -1); // -1 = >1000 шагов
}

// ——— 6. Работа с лентой ———
TEST(SetTape_LongString_HandlesUpTo511) {
    Markov m;
    char long_str[512];
    for (int i = 0; i < 511; ++i) long_str[i] = 'a';
    long_str[511] = '\0';
    m.setTape(long_str);
    CHECK(strcmp(m.getTape(), long_str) == 0);
}

TEST(SetTape_EmptyString_SetsEmptyTape) {
    Markov m;
    m.setTape("");
    CHECK(strcmp(m.getTape(), "") == 0);
}

// ——— 7. Печать правил ———
TEST(PrintRules_ValidRules_PrintsCorrectly) {
    Markov m;
    m.addRule("a", "b", false);
    m.addRule("b", "c", true);

    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    m.printRules();
    std::cout.rdbuf(old);

    std::string out = oss.str();
    CHECK(out.find("1. a -> b") != std::string::npos);
    CHECK(out.find("2. b -> . c") != std::string::npos);
}

// ——— 8. Edge cases ———
TEST(Step_EmptyTape_NoRules_AppliesNothing) {
    Markov m;
    m.setTape("");
    CHECK(m.step() == 1); // завершено
}

TEST(Run_EmptyTape_EmptyResult) {
    Markov m;
    m.setTape("");
    m.run(false);
    CHECK(strcmp(m.getTape(), "") == 0);
}

TEST(LoadFromFile_TrimsSpaces) {
    const char* data =
        "  abc  \n"
        "  a  ->  b  \n"
        "b -> .c\n";
    write_temp_file("t3.txt", data);
    TempFileGuard _g("t3.txt");

    Markov m;
    CHECK(m.loadFromFile("t3.txt") == 0);
    CHECK(strcmp(m.getTape(), "abc") == 0); // пробелы обрезаны
    m.run(false);
    CHECK(strcmp(m.getTape(), "c") == 0); // a->b->.c → c (финальное, но не удаляет c)
}

// ——— 9. Циклы и рекурсия ———
TEST(CyclicRules_StopsAfter1000Steps) {
    Markov m;
    m.addRule("a", "a", false); // бесконечный цикл
    m.setTape("a");
    CHECK(m.run(false) == -1);
}

// ——— 10. Подстрока и позиция ———
TEST(FindRule_LeftmostMatch) {
    Markov m;
    m.addRule("a", "X", false);
    m.setTape("aaa");
    m.step();
    CHECK(strcmp(m.getTape(), "Xaa") == 0); // замена первой 'a'
}

TEST(FindRule_Overlapping_NotApplied) {
    Markov m;
    m.addRule("aa", "X", false);
    m.setTape("aaa");
    m.step();
    CHECK(strcmp(m.getTape(), "Xa") == 0); // "aa" в позиции 0 → замена
}

// ——— 11. Финальные правила ———
TEST(FinalRule_MiddleOfProgram_StopsImmediately) {
    Markov m;
    m.addRule("x", "y", false);
    m.addRule("a", "b", true); // финальное
    m.addRule("b", "c", false);
    m.setTape("a");
    m.run(false);
    CHECK(strcmp(m.getTape(), "b") == 0); // после b не должно быть c
}

// ——— 12. Дополнительные тесты ———
TEST(AddRule_NullString) {
    Markov m;
    CHECK(m.addRule(nullptr, "b", false) == -1);
}

TEST(GetTape_NonEmpty_ReturnsCorrectPtr) {
    Markov m;
    m.setTape("hello");
    const char* p = m.getTape();
    CHECK(strcmp(p, "hello") == 0);
}

TEST(Step_ReturnsCode) {
    Markov m;
    m.setTape("x");
    CHECK(m.step() == 1); // завершено
    CHECK(m.step() == 1); // уже завершено
}

TEST(Run_LogMode_DoesNotCrash) {
    Markov m;
    m.addRule("a", "b", false);
    m.setTape("a");
    CHECK(m.run(true) == 0); // true = log — не падает
}

TEST(LoadFromFile_UTF8_NoBOM_Works) {
    // Создаём файл в UTF-8 без BOM
    const char* data = "привет\nп -> р\n";
    write_temp_file("utf8.txt", data);
    TempFileGuard _g("utf8.txt");

    Markov m;
    CHECK(m.loadFromFile("utf8.txt") == 0);
    CHECK(strcmp(m.getTape(), "привет") == 0);
}

TEST(EmptyRuleFile_OnlyTape) {
    write_temp_file("empty.txt", "start\n");
    TempFileGuard _g("empty.txt");
    Markov m;
    CHECK(m.loadFromFile("empty.txt") == 0);
    CHECK(strcmp(m.getTape(), "start") == 0);
    m.run(false);
    CHECK(strcmp(m.getTape(), "start") == 0);
}

TEST(MultipleFinalRules_FirstStops) {
    Markov m;
    m.addRule("a", "b", true);
    m.addRule("a", "c", true);
    m.setTape("a");
    m.run(false);
    CHECK(strcmp(m.getTape(), "b") == 0); // первый финальный
}

TEST(Run_WithLog_CapturesOutput) {
    Markov m;
    m.addRule("a", "b", false);
    m.setTape("a");
    
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    m.run(true);
    std::cout.rdbuf(old);

    std::string log = oss.str();
    CHECK(log.find("[Начало] a") != std::string::npos);
    CHECK(log.find("[Шаг]") != std::string::npos);
    CHECK(log.find("[Конец] b") != std::string::npos);
}

// ——— 13. Покрытие методов ———
TEST(Coverage_AllMethodsCalled) {
    Markov m;
    m.addRule("a", "b", false);
    m.setTape("a");
    m.step();
    m.run(false);
    m.printRules();
    // Просто проверка: не упало → все методы вызваны
}

// ——— 14. Точка входа ———
int main() {
    return UnitTest::RunAllTests();
}
