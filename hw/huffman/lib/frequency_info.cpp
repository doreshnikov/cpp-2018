//
// Created by amathael on 09.06.18.
//

#include "frequency_info.h"

frequency_info::frequency_info() : _count(0), _data(MAX_SYMBOL + 1ul, 0) {}

void frequency_info::add_symbol(symbol s) {
    _data[s]++;
    _count++;
}

uint64_t frequency_info::count() const {
    return _count;
}

std::vector<std::pair<symbol, uint64_t>> frequency_info::get_data() const {
    std::vector<std::pair<symbol, uint64_t>> result;
    symbol s = 0;
    for (auto cnt : _data) {
        if (cnt != 0) {
            result.emplace_back(s, cnt);
        }
        s++;
    }

    return result;
}
