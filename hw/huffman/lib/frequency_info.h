//
// Created by amathael on 09.06.18.
//

#ifndef CPP_FREQUENCY_INFO_H
#define CPP_FREQUENCY_INFO_H

#include <unordered_map>
#include <vector>

#include "_literals.h"

class frequency_info {

public:

    frequency_info();

    void add_symbol(symbol s);

    uint64_t count() const;

    std::vector<std::pair<symbol, uint64_t>> get_data() const;

private:

    uint64_t _count;
    std::vector<uint64_t> _data;

};

#endif //CPP_FREQUENCY_INFO_H
