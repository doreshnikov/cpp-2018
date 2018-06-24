//
// Created by amathael on 09.06.18.
//

#include "header_data.h"

header_data::header_data() = default;

header_data::header_data(header_data const &other) = default;

void header_data::add_symbol(symbol s, code &c) {
    _data.emplace_back(s, c);
}

std::vector<std::pair<symbol, code>> header_data::get_data() const {
    return _data;
}

uint32_t header_data::count() const {
    return static_cast<uint32_t>(_data.size());
}
