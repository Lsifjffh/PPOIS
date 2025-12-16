#ifndef MARKOV_H
#define MARKOV_H

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

class Markov {
public:
    static const int MAX_RULES = 32;
    static const int MAX_LEN = 64;
    static const int TAPE_MAX = 512;

    Markov();      // ✅ Объявление конструктора
    ~Markov();     // ✅ Объявление деструктора

    int addRule(const char* from, const char* to, bool is_final);
    int loadFromFile(const char* filename);
    void setTape(const char* input);
    int step();
    int run(bool log = false);
    void printRules() const;
    const char* getTape() const;

private:
    struct Rule {
        char from[MAX_LEN];
        char to[MAX_LEN];
        bool is_final;
    };

    Rule rules[MAX_RULES];
    int rule_count;
    char tape[TAPE_MAX];
    bool finished;

    int strLen(const char* s) const;
    int strFind(const char* text, const char* pattern) const;
    void strCopy(char* dst, const char* src) const;
    int findRule(const char* s, int& pos) const;
    void applyRule(int rule_idx, int pos);
};

#endif
