//
// Created by amathael on 10.06.18.
//

#include <iostream>
#include <cstring>
#include "buffered_writer.h"

buffered_writer::buffered_writer(const char *filename) : _pos(BUFFER_SIZE) {
    _output.open(filename, std::ios::out | std::ios::binary);
    _buffer = new uint8_t[BUFFER_SIZE];
    clear_buffer();
}

buffered_writer::~buffered_writer() {
    close();
    delete[] _buffer;
}

void buffered_writer::write_symbol(symbol s) {
    for (uint32_t i = 0; i < SYMBOL_BYTES; i++) {
        write_byte(static_cast<uint8_t>(s >> BYTE_BITS * (SYMBOL_BYTES - i - 1u) & (1 << BYTE_BITS) - 1)); // NOLINT
    }
}

void buffered_writer::write_byte(uint8_t byte) {
    (_buffer[_pos++] <<= BYTE_BITS - _bit) |= byte >> _bit;
    uint8_t bit = _bit;
    if (_pos == BUFFER_SIZE) {
        dump_buffer();
    }
    if (bit != 0) {
        _bit = bit;
        _buffer[_pos] = static_cast<uint8_t>(byte & (1ul << _bit) - 1);
    }
}

void buffered_writer::write_bits(uint8_t bits, uint8_t amount) {
    if (amount + _bit >= BYTE_BITS) {
        (_buffer[_pos++] <<= BYTE_BITS - _bit) |= bits >> _bit; // NOLINT
        uint8_t bit = _bit;
        if (_pos == BUFFER_SIZE) {
            dump_buffer();
        }
        _bit = amount + bit - BYTE_BITS;
        _buffer[_pos] = static_cast<uint8_t>((bits >> BYTE_BITS - amount) & (1u << _bit) - 1); // NOLINT
    } else {
        (_buffer[_pos] <<= amount) |= bits >> BYTE_BITS - amount; // NOLINT
        _bit += amount;
    }
}

void buffered_writer::write_bit(uint8_t bit) {
    (_buffer[_pos] <<= 1) |= bit;
    _bit++;
    if (_bit == BYTE_BITS) {
        _bit = 0;
        _pos++;
        if (_pos == BUFFER_SIZE) {
            dump_buffer();
        }
    }
}

void buffered_writer::complete_byte() {
    while (_bit != 0) {
        write_bit(0);
    }
}

void buffered_writer::close() {
    dump_buffer();
    _output.close();
}

void buffered_writer::dump_buffer() {
    _output.write(reinterpret_cast<const char *>(_buffer), _pos);
    clear_buffer();
}

void buffered_writer::clear_buffer() {
    memset(_buffer, 0, _pos);
    _pos = 0;
    _bit = 0;
}
