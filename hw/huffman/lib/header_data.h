//
// Created by amathael on 09.06.18.
//

#ifndef CPP_HEADER_DATA_H
#define CPP_HEADER_DATA_H

#include "_literals.h"
#include "code.h"

class header_data {

public:

    header_data();

    header_data(header_data const &other);

    void add_symbol(symbol s, code &c);

    std::vector<std::pair<symbol, code>> get_data() const;

    uint32_t count() const;

private:

    std::vector<std::pair<symbol, code>> _data;

};

#endif //CPP_HEADER_DATA_H
