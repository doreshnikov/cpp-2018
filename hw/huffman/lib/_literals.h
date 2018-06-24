//
// Created by amathael on 02.06.18.
//

#ifndef CPP_LITERALS_H
#define CPP_LITERALS_H

#include <cstdint>
#include <cstddef>

typedef uint8_t symbol;

const uint8_t BYTE_BITS = 8;
const uint8_t SYMBOL_BITS = 8 * sizeof(symbol);
const uint8_t SYMBOL_BYTES = SYMBOL_BITS / BYTE_BITS;

const symbol MAX_SYMBOL = static_cast<symbol>((1u << SYMBOL_BITS) - 1);
const uint32_t BUFFER_SIZE = static_cast<uint32_t>(SYMBOL_BYTES * 1024 * 1024);

#endif //CPP_LITERALS_H
