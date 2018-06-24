//
// Created by amathael on 10.06.18.
//

#include "buffered_reader.h"

buffered_reader::buffered_reader(const char *filename) : _eof(false), _length(0) {
    _input.open(filename, std::ios::in | std::ios::binary);
    if (!_input) {
        throw custom_reader::opening_exception();
    }
    _buffer = new uint8_t[BUFFER_SIZE];
    fill_buffer(0);
}

buffered_reader::~buffered_reader() {
    delete[] _buffer;
    _input.close();
}

symbol buffered_reader::get_symbol() {
    symbol s = 0;
    for (uint32_t i = 0; i < SYMBOL_BYTES; i++) {
        (s <<= 8) |= get_byte();
    }
    return s;
}

uint8_t buffered_reader::get_byte() {
    if (eof()) {
        throw custom_reader::eof_exception();
    }
    uint8_t byte = _buffer[_pos++] << _bit;
    uint8_t bit = _bit;
    if (_pos == _last) {
        fill_buffer(0);
    }
    if (bit != 0) {
        if (eof()) {
            throw custom_reader::eof_exception();
        }
        byte += (_buffer[_pos] >> (BYTE_BITS - bit)); // NOLINT
        _bit = bit;
    }
    return byte;
}

uint8_t buffered_reader::get_bit() {
    if (_bit == 0 && eof()) {
        throw custom_reader::eof_exception();
    }
    auto res = static_cast<uint8_t>((_buffer[_pos] >> (BYTE_BITS - _bit - 1u)) & 1u); // NOLINT
    _bit++;
    if (_bit == 8) {
        _bit = 0;
        _pos++;
        if (_pos == _last) {
            fill_buffer(0);
        }
    }
    return res;
}

uint64_t buffered_reader::get_size() {
    return _length;
}

void buffered_reader::complete_byte() {
    while (_bit != 0) {
        get_bit();
    }
}

bool buffered_reader::eof() {
    return _eof && _pos == _last;
}

bool buffered_reader::next_is_last() {
    if (_eof) {
        return _pos + 1 == _last;
    } else {
        if (_pos + 1 < _last) {
            return false;
        }
        _buffer[0] = _buffer[_pos];
        fill_buffer(_last - _pos);
        return _eof && _last == 1;
    }
}

void buffered_reader::to_start() {
    _input.clear();
    _input.seekg(0, std::ios::beg);
    _eof = false;
    fill_buffer(0);
}

void buffered_reader::close() {
    _input.close();
}

void buffered_reader::fill_buffer(uint32_t pos) {
    _input.read((char *) _buffer + pos, BUFFER_SIZE - pos);
    auto read = static_cast<uint32_t>(_input.gcount());
    if (read < BUFFER_SIZE - pos) {
        _eof = true;
    } else {
        _length += read;
    }
    _pos = 0;
    _bit = 0;
    _last = pos + read;
}
