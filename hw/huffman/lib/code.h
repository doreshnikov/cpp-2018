//
// Created by amathael on 09.06.18.
//

#ifndef CPP_CODE_H
#define CPP_CODE_H

#include <vector>
#include <string>

#include "_literals.h"

class code {

public:

    code();
    code(code const &other);
    code(code const &other, uint8_t last);

    void push_back(uint8_t last);
    void append(code const &other);

    uint32_t length() const;
    uint32_t size() const;
    uint8_t get(uint32_t index) const;
    uint8_t get_byte(uint32_t index) const;

    std::string to_string() const;

private:

    uint8_t _last_taken;
    std::vector<uint8_t> _data;

};

#endif //CPP_CODE_H
