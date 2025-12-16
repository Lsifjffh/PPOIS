#include "Markov.h"

Markov::Markov() {
    rule_count = 0;
    tape[0] = '\0';
    finished = false;
}

Markov::~Markov() {}

int Markov::strLen(const char* s) const {
    int i = 0;
    while (s[i]) i++;
    return i;
}

int Markov::strFind(const char* text, const char* pattern) const {
    int tl = strLen(text);
    int pl = strLen(pattern);
    if (pl == 0) return 0;
    for (int i = 0; i <= tl - pl; i++) {
        bool match = true;
        for (int j = 0; j < pl; j++) {
            if (text[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}

void Markov::strCopy(char* dst, const char* src) const {
    int i = 0;
    while (src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

int Markov::addRule(const char* from, const char* to, bool is_final) {
    if (rule_count >= MAX_RULES) return -1;
    if (strLen(from) == 0) return -1;
    strCopy(rules[rule_count].from, from);
    strCopy(rules[rule_count].to, to);
    rules[rule_count].is_final = is_final;
    rule_count++;
    return 0;
}

int Markov::loadFromFile(const char* filename) {
    ifstream f(filename);
    if (!f.is_open()) return -1;

    rule_count = 0;
    char line[256];

    // Первая строка — начальная лента
    if (!f.getline(line, sizeof(line))) return -1;
    setTape(line);  // ← Устанавливаем ленту сразу

    // Остальные строки — правила
    while (f.getline(line, sizeof(line)) && rule_count < MAX_RULES) {
        int len = strLen(line);
        if (len > 0 && line[len - 1] == '\r') line[len - 1] = '\0';

        char* arrow = nullptr;
        for (int i = 0; line[i]; i++) {
            if (line[i] == '-' && line[i + 1] == '>') {
                arrow = line + i;
                break;
            }
        }
        if (!arrow) continue;

        int left_len = arrow - line;
        if (left_len >= MAX_LEN) left_len = MAX_LEN - 1;
        char from[MAX_LEN];
        for (int i = 0; i < left_len; i++) from[i] = line[i];
        from[left_len] = '\0';

        char* right = arrow + 2;
        bool is_final = false;
        if (*right == '.') {
            is_final = true;
            right++;
        }
        char to[MAX_LEN];
        strCopy(to, right);

        addRule(from, to, is_final);
    }
    return 0;
}
void Markov::setTape(const char* input) {
    strCopy(tape, input);
    finished = false;
}

int Markov::findRule(const char* s, int& pos) const {
    for (int i = 0; i < rule_count; i++) {
        pos = strFind(s, rules[i].from);
        if (pos != -1) return i;
    }
    return -1;
}

void Markov::applyRule(int rule_idx, int pos) {
    char new_tape[TAPE_MAX];
    int tlen = strLen(tape);
    int plen = strLen(rules[rule_idx].from);
    int rlen = strLen(rules[rule_idx].to);

    // До шаблона
    for (int i = 0; i < pos; i++) new_tape[i] = tape[i];
    // Замена
    for (int i = 0; i < rlen; i++) new_tape[pos + i] = rules[rule_idx].to[i];
    // После
    for (int i = pos + plen; i < tlen; i++) {
        new_tape[pos + rlen + (i - pos - plen)] = tape[i];
    }
    new_tape[pos + rlen + tlen - pos - plen] = '\0';

    strCopy(tape, new_tape);
    if (rules[rule_idx].is_final) finished = true;
}

int Markov::step() {
    if (finished) return 1;

    int pos = 0;
    int idx = findRule(tape, pos);
    if (idx == -1) {
        finished = true;
        return 1;
    }

    applyRule(idx, pos);
    return 0;
}

int Markov::run(bool log) {
    if (log) {
        cout << "[Начало] " << tape << "\n";
    }

    int steps = 0;
    while (!finished && steps < 1000) {
        step();
        steps++;
        if (log && !finished) {
            cout << "[Шаг " << steps << "] " << tape << "\n";
        }
    }

    if (log) {
        cout << "[Конец] " << tape << "\n";
    }

    return steps >= 1000 ? -1 : 0;
}

void Markov::printRules() const {
    if (rule_count == 0) {
        cout << "No rules\n";
        return;
    }
    for (int i = 0; i < rule_count; i++) {
        cout << (i + 1) << ". " << rules[i].from << " -> ";
        if (rules[i].is_final) cout << ". ";
        cout << rules[i].to << "\n";
    }
}

const char* Markov::getTape() const {
    return tape;
}