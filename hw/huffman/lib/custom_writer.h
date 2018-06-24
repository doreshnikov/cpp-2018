//
// Created by amathael on 10.06.18.
//

#ifndef CPP_CUSTOM_WRITER_H
#define CPP_CUSTOM_WRITER_H

#include "_literals.h"

class custom_writer {

public:

    virtual void write_symbol(symbol s) = 0;

    virtual void write_byte(uint8_t b) = 0;

    virtual void write_bits(uint8_t bits, uint8_t amount) = 0;

    virtual void write_bit(uint8_t bit) = 0;

    virtual void complete_byte() = 0;

    virtual void close() = 0;

};

#endif //CPP_CUSTOM_WRITER_H
