#include <iostream>
#include <vector>
#include <algorithm>

char *get_char(std::string *word, char *b1, size_t l1, char *b2, size_t l2, size_t index) {
    if (index < word->size()) {
        return &(*word)[index];
    } else if (index - word->size() < l1) {
        return &b1[index - word->size()];
    } else {
        return &b2[index - word->size() - l1];
    }
}

void z_function(std::string *word, char *b1, size_t l1, char *b2, size_t l2, std::vector<int> &indexes) {
    size_t size = l1 + l2 + word->size();
    std::vector<size_t> zf(size);
    zf[0] = 0;

    for (size_t i = 1, l = 0, r = 0; i < size; i++) {
        if (i <= r) {
            zf[i] = std::min(r - i + 1, zf[i - l]);
        }
        while (i + zf[i] < size &&
            *get_char(word, b1, l1, b2, l2, zf[i]) == *get_char(word, b1, l1, b2, l2, i + zf[i])) {
            zf[i]++;
        }
        if (i + zf[i] - 1 > r) {
            l = i;
            r = i + zf[i] - 1;
        }
    }

    indexes.clear();
    for (int i = word->size(); i < size; i++) {
        if (zf[i] >= word->size()) {
            indexes.push_back(i - word->size() - l1);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cerr << "Too few arguments, filename and a word needed";
        return 0;
    }
    freopen("output", "w", stdout);

    std::string word = argv[2];
    const size_t BUFFER_SIZE = word.size() + 200;
    FILE *file = fopen(argv[1], "r");
    char buffer[2][BUFFER_SIZE];
    int cur = 0;

    long long count = 0;
    size_t last_read = BUFFER_SIZE;
    while (last_read == BUFFER_SIZE) {
        last_read = fread(buffer[cur], 1, BUFFER_SIZE, file);
        std::vector<int> pf;
        if (count != 0) {
            z_function(&word, buffer[cur ^ 1] + (BUFFER_SIZE - word.size() + 1), word.size() - 1,
                       buffer[cur], last_read, pf);
        } else {
            z_function(&word, nullptr, 0, buffer[cur], last_read, pf);
        }
        for (int index : pf) {
            std::cout << count + index << '\n';
        }

        count += last_read;
        cur ^= 1;
    }

    fclose(file);
    return 0;

}