#ifndef CPP_DIGIT_TYPES_H
#define CPP_DIGIT_TYPES_H

#include <cstdlib>
#include <cstdint>

typedef unsigned int atom;
typedef unsigned long long overflow_atom;
const size_t ATOM_SIZE = sizeof(atom) * 8;
const atom ATOM_MAX = UINT32_MAX;

#endif //CPP_DIGIT_TYPES_H
