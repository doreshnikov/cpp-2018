//
// Created by isuca on 23-Mar-18.
//

#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <vector>
#include <string>
#include "digit_types.h"
#include "shared_vector.h"

struct big_integer {

    big_integer();

    big_integer(const big_integer &other);

    big_integer(int a); // NOLINT
    big_integer(long long a); // NOLINT
    big_integer(unsigned int a); // NOLINT
    explicit big_integer(std::string const &str);

    big_integer(bool negative, std::vector<atom> const &data);
    big_integer(bool negative, shared_vector const &data);

    ~big_integer();

    big_integer &operator=(const big_integer &other);

    big_integer &operator+=(const big_integer &rhs);

    big_integer &operator-=(const big_integer &rhs);

    big_integer &operator*=(const big_integer &rhs);

    big_integer &operator/=(const big_integer &rhs);

    big_integer &operator%=(const big_integer &rhs);

    big_integer &operator&=(const big_integer &rhs);

    big_integer &operator|=(const big_integer &rhs);

    big_integer &operator^=(const big_integer &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    friend big_integer operator+(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator-(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator*(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator/(const big_integer &lhs, const big_integer &rhs);

    big_integer short_divide(atom rhs) const;

    friend big_integer operator%(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator&(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator|(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator^(const big_integer &lhs, const big_integer &rhs);

    friend big_integer operator<<(const big_integer &lhs, int rhs);

    friend big_integer operator>>(const big_integer &lhs, int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    const big_integer operator++(int);

    big_integer &operator--();

    const big_integer operator--(int);

    friend bool operator==(const big_integer &lhs, const big_integer &rhs);

    friend bool operator!=(const big_integer &lhs, const big_integer &rhs);

    friend bool operator<(const big_integer &lhs, const big_integer &rhs);

    friend bool operator>(const big_integer &lhs, const big_integer &rhs);

    friend bool operator<=(const big_integer &lhs, const big_integer &rhs);

    friend bool operator>=(const big_integer &lhs, const big_integer &rhs);

    bool is_zero() const;

    std::string to_string() const;

    big_integer abs() const;

    atom get_atom(size_t index) const;

    atom get_bitwise_atom(size_t index) const;

private:

    bool negative = false;

    shared_vector data;

    void swap(big_integer &b);

    void strip();

    big_integer to_bitwise() const;

    big_integer from_bitwise() const;

    friend big_integer division(const big_integer &lhs, const big_integer &rhs, bool part);

    template <class FunctorT>
    friend big_integer binary_applicator(const big_integer &lhs, const big_integer &rhs, FunctorT f);

};

std::string to_string(const big_integer &bi);

#endif //BIG_INTEGER_H
