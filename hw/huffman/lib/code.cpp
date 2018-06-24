//
// Created by amathael on 09.06.18.
//

#include "code.h"

code::code() : _last_taken(0), _data(1, 0) {}

code::code(code const &other) = default;

code::code(code const &other, uint8_t last) : _last_taken(other._last_taken), _data(other._data) {
    push_back(last);
}

void code::push_back(uint8_t last) {
    _data.back() |= (last << BYTE_BITS - _last_taken - 1u);
    _last_taken = static_cast<uint8_t>((_last_taken + 1) % BYTE_BITS);
    if (_last_taken == 0) {
        _data.push_back(0);
    }
}

void code::append(code const &other) {
    for (uint32_t i = 0; i < other.length(); i++) {
        push_back(other.get(i));
    }
}

uint32_t code::length() const {
    return static_cast<uint32_t>(_last_taken + (_data.size() - 1) * BYTE_BITS);
}

uint32_t code::size() const {
    return static_cast<uint32_t>(_last_taken == 0 ? _data.size() - 1 : _data.size());
}

uint8_t code::get(uint32_t index) const {
    uint8_t block = _data[index / BYTE_BITS];
    block >>= (BYTE_BITS - index % BYTE_BITS - 1);
    return static_cast<uint8_t>(block & 1u);
}

uint8_t code::get_byte(uint32_t index) const {
    return _data[index];
}

std::string code::to_string() const {
    std::string res;
    for (uint32_t i = 0; i < length(); i++) {
        res.push_back(get(i) == 0 ? '0' : '1');
    }
    return res;
}
