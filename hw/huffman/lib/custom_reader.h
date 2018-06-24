//
// Created by amathael on 10.06.18.
//

#ifndef CPP_CUSTOM_READER_H
#define CPP_CUSTOM_READER_H

#include <exception>
#include "_literals.h"

class custom_reader {

public:

    custom_reader() = default;

    virtual symbol get_symbol() = 0;

    virtual uint8_t get_byte() = 0;

    virtual uint8_t get_bit() = 0;

    virtual uint64_t get_size() = 0;

    virtual void complete_byte() = 0;

    virtual bool eof() = 0;

    virtual bool next_is_last() = 0;

    virtual void to_start() = 0;

    virtual void close() = 0;

    class eof_exception : std::exception {
    };

    class opening_exception : std::exception {
    };

};

#endif //CPP_CUSTOM_READER_H
