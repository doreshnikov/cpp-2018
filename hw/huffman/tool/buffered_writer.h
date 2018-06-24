//
// Created by amathael on 10.06.18.
//

#ifndef CPP_BUFFERED_WRITER_H
#define CPP_BUFFERED_WRITER_H

#include <fstream>
#include "../lib/_literals.h"
#include "../lib/custom_writer.h"

class buffered_writer : public custom_writer {

public:

    buffered_writer() = delete;

    explicit buffered_writer(const char *filename);

    ~buffered_writer();

    void write_symbol(symbol s) override;

    void write_byte(uint8_t byte) override;

    void write_bits(uint8_t bits, uint8_t amount) override;

    void write_bit(uint8_t bit) override;

    void complete_byte() override;

    void close() override;

private:

    void dump_buffer();
    void clear_buffer();

    uint8_t *_buffer;
    std::ofstream _output;
    uint32_t _pos;
    uint8_t _bit;

};

#endif //CPP_BUFFERED_WRITER_H
