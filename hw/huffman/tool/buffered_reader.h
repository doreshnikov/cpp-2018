//
// Created by amathael on 10.06.18.
//

#ifndef CPP_BUFFERED_READER_H
#define CPP_BUFFERED_READER_H

#include <fstream>

#include "../lib/_literals.h"
#include "../lib/custom_reader.h"

class buffered_reader : public custom_reader {

public:

    buffered_reader() = delete;

    explicit buffered_reader(const char *filename);

    ~buffered_reader();

    symbol get_symbol() override;

    uint8_t get_byte() override;

    uint8_t get_bit() override;

    uint64_t get_size() override;

    void complete_byte() override;

    bool eof() override;

    bool next_is_last() override;

    void to_start() override;

    void close() override;

private:

    void fill_buffer(uint32_t pos);

    uint8_t *_buffer;
    std::ifstream _input;
    uint64_t _length;
    bool _eof;
    uint32_t _pos;
    uint32_t _last;
    uint8_t _bit;

};

#endif //CPP_BUFFERED_READER_H
