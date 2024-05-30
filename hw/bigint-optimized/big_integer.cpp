#include "big_integer.h"
#include "shared_vector.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

std::string to_string(const big_integer &bi) {
    return bi.to_string();
}

overflow_atom to_overflow(atom number) {
    return static_cast<overflow_atom>(number);
}

atom to_atom(overflow_atom number) {
    return static_cast<atom>(number);
}

bool big_integer::is_zero() const {
    return data.size() == 1 && data[0] == 0;
}

atom big_integer::get_atom(size_t index) const {
    return index < data.size() ? data[index] : 0;
}

atom big_integer::get_bitwise_atom(size_t index) const {
    return negative ? get_atom(index) ^ ATOM_MAX : get_atom(index);
}

big_integer::big_integer() : negative(false), data(1) {}

big_integer::big_integer(const big_integer &other) = default;

big_integer::big_integer(int a) : negative(a < 0) {
    data = big_integer(static_cast<atom>(llabs(a))).data;
}

big_integer::big_integer(long long a) : negative(a < 0) {
    auto b = static_cast<unsigned long long int>(llabs(a));
    while (b > 0) {
        data.push_back(static_cast<atom>(b & ATOM_MAX));
        b >>= ATOM_SIZE;
    }
    if (data.empty()) {
        data.push_back(0);
    }
}

big_integer::big_integer(unsigned int a) : negative(false), data(1, a) {}

big_integer::big_integer(std::string const &str) {
    size_t index = str[0] == '-' ? 1 : 0;
    bool neg = index == 1;
    big_integer res(0);

    for (; index < str.size(); index++) {
        (res *= 10) += str[index] - '0';
    }
    res.negative = neg;
    swap(res);
}

big_integer::big_integer(bool negative, std::vector<atom> const &data) : negative(negative), data(data) {
    if (data.back() == 0) {
        strip();
    }
}

big_integer::big_integer(bool negative, shared_vector const &data) : negative(negative), data(data) {
    if (data.back() == 0) {
        strip();
    }
}

big_integer::~big_integer() {
    negative = false;
}

big_integer &big_integer::operator=(const big_integer &other) {
    big_integer temp(other);
    swap(temp);
    return *this;
}

big_integer &big_integer::operator+=(const big_integer &rhs) {
    return *this = *this + rhs;
}

big_integer operator+(const big_integer &lhs, const big_integer &rhs) {
    if (lhs.negative == rhs.negative) {
        bool carry = false;
        size_t size = std::max(lhs.data.size(), rhs.data.size());
        std::vector<atom> data(size, 0);

        for (size_t index = 0; index < size || carry; index++) {
            if (index == data.size()) {
                data.push_back(0);
            }
            overflow_atom sum = to_overflow(lhs.get_atom(index)) + rhs.get_atom(index) + carry;
            data[index] = to_atom(sum & ATOM_MAX);
            carry = sum > ATOM_MAX;
        }

        return big_integer(lhs.negative, data);
    } else {
        return lhs - (-rhs);
    }
}

big_integer &big_integer::operator-=(const big_integer &rhs) {
    return *this = *this - rhs;
}

big_integer operator-(const big_integer &lhs, const big_integer &rhs) {
    if (lhs.negative == rhs.negative) {
        if (lhs.abs() < rhs.abs()) {
            return -(rhs - lhs);
        }
        bool carry = false;
        std::vector<atom> data(lhs.data.size());

        for (size_t index = 0; index < data.size(); index++) {
            overflow_atom sub = to_overflow(rhs.get_atom(index)) + carry;
            carry = sub > lhs.get_atom(index);
            if (carry) {
                data[index] = to_atom((sub - lhs.get_atom(index) - 1) ^ ATOM_MAX);
            } else {
                data[index] = to_atom(lhs.get_atom(index) - sub);
            }
        }

        return big_integer(lhs.negative, data);
    } else {
        return lhs + (-rhs);
    }
}

big_integer &big_integer::operator*=(const big_integer &rhs) {
    return *this = *this * rhs;
}

big_integer operator*(const big_integer &lhs, const big_integer &rhs) {
    std::vector<atom> data(lhs.data.size() + rhs.data.size() + 1, 0);

    for (size_t i = 0; i < lhs.data.size(); i++) {
        atom carry = 0;
        for (size_t j = 0; j < rhs.data.size() || carry > 0; j++) {
            overflow_atom current = to_overflow(lhs.get_atom(i)) * rhs.get_atom(j) + data[i + j] + carry;
            data[i + j] = to_atom(current & ATOM_MAX);
            carry = to_atom(current >> ATOM_SIZE);
        }
    }

    return big_integer(lhs.negative ^ rhs.negative, data);
}

big_integer big_integer::short_divide(atom rhs) const {
    std::vector<atom> div_data;
    overflow_atom mod = 0;

    for (size_t index = 0; index < data.size(); index++) {
        size_t idx = data.size() - index - 1;
        overflow_atom current = (mod << ATOM_SIZE) + get_atom(idx);
        atom current_div = to_atom(current / rhs);
        mod = current % rhs;
        div_data.push_back(current_div);
    }

    std::reverse(div_data.begin(), div_data.end());
    return big_integer(false, div_data);
}

big_integer division(const big_integer &lhs, const big_integer &rhs, bool part) {
    if (rhs.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    big_integer alhs = lhs.abs(), arhs = rhs.abs();
    big_integer res(0), mod(0);
    if (alhs < arhs) {
        res = 0;
        mod = alhs;
    } else if (alhs == arhs) {
        res = 1;
        mod = 0;
    } else if (arhs.data.size() == 1) {
        res = alhs.short_divide(arhs.data[0]);
        mod = alhs - res * arhs;
    } else {
        atom f = to_atom((1LL << ATOM_SIZE) / to_overflow(arhs.data.back() + 1)); // NOLINT
        alhs *= f;
        arhs *= f;

        size_t n = alhs.data.size(), m = arhs.data.size();
        std::vector<atom> data(n - m + 1);
        mod = alhs >> ((n - m + 1) * ATOM_SIZE);

        overflow_atom top = arhs.data.back();
        for (size_t index = 0; index <= n - m; index++) {
            size_t idx = n - m - index;
            mod <<= ATOM_SIZE;
            mod.data[0] = alhs.data[idx];

            overflow_atom mod_top = mod.data.back();
            if (mod.data.size() > m) {
                mod_top <<= ATOM_SIZE;
                mod_top += mod.data[mod.data.size() - 2];
            }

            overflow_atom guess = std::min(mod_top / top, to_overflow(ATOM_MAX));
            big_integer res_guess = arhs * to_atom(guess);
            while (mod < res_guess) {
                guess--;
                res_guess -= arhs;
            }

            data[idx] = to_atom(guess);
            mod -= res_guess;
        }

        res = big_integer(false, data);
    }
    if (lhs.negative != rhs.negative) {
        res = -res;
    }

    return part ? res : lhs - rhs * res;
}

big_integer &big_integer::operator/=(const big_integer &rhs) {
    return *this = *this / rhs;
}

big_integer operator/(const big_integer &lhs, const big_integer &rhs) {
    return division(lhs, rhs, true);
}

big_integer &big_integer::operator%=(const big_integer &rhs) {
    return *this = *this % rhs;
}

big_integer operator%(const big_integer &lhs, const big_integer &rhs) {
    return division(lhs, rhs, false);
}

big_integer big_integer::to_bitwise() const {
    return negative ? *this + 1 : *this;
}

big_integer big_integer::from_bitwise() const {
    if (!negative) {
        return *this;
    } else {
        shared_vector inverted(data);
        for (size_t idx = 0; idx < inverted.size(); idx++) {
            inverted[idx] = ~inverted[idx];
        }
        return big_integer(true, inverted) - 1;
    }
}

template<class FunctorT>
big_integer binary_applicator(const big_integer &lhs, const big_integer &rhs, FunctorT f) {
    std::vector<atom> data(std::max(lhs.data.size(), rhs.data.size()), 0);
    big_integer left = lhs.to_bitwise(), right = rhs.to_bitwise();

    for (size_t index = 0; index < data.size(); index++) {
        data[index] = f(left.get_bitwise_atom(index), right.get_bitwise_atom(index));
    }

    return big_integer(static_cast<bool>(f(static_cast<atom>(lhs.negative), static_cast<atom>(rhs.negative))), data)
            .from_bitwise();
}

big_integer &big_integer::operator&=(const big_integer &rhs) {
    return *this = *this & rhs;
}

big_integer operator&(const big_integer &lhs, const big_integer &rhs) {
    return binary_applicator(lhs, rhs, [](atom l, atom r) -> atom { return l & r; });
}

big_integer &big_integer::operator|=(const big_integer &rhs) {
    return *this = *this | rhs;
}

big_integer operator|(const big_integer &lhs, const big_integer &rhs) {
    return binary_applicator(lhs, rhs, [](atom l, atom r) -> atom { return l | r; });
}

big_integer &big_integer::operator^=(const big_integer &rhs) {
    return *this = *this ^ rhs;
}

big_integer operator^(const big_integer &lhs, const big_integer &rhs) {
    return binary_applicator(lhs, rhs, [](atom l, atom r) -> atom { return l ^ r; });
}

big_integer &big_integer::operator<<=(int rhs) {
    return *this = *this << rhs;
}

big_integer operator<<(const big_integer &lhs, int rhs) {
    if (rhs < 0) {
        return lhs >> (-rhs);
    }
    size_t skip = rhs / ATOM_SIZE;
    size_t move = rhs % ATOM_SIZE;
    big_integer left = lhs.to_bitwise();

    std::vector<atom> data(left.data.size() + skip + 1, 0);
    if (lhs.negative) {
        data.back() ^= (ATOM_MAX << move);
    }
    for (size_t index = 0; index < left.data.size(); index++) {
        data[index + skip] += left.get_bitwise_atom(index) << move;
        if (move != 0) {
            data[index + skip + 1] += left.get_bitwise_atom(index) >> (ATOM_SIZE - move);
        }
    }
    return big_integer(lhs.negative, data).from_bitwise();
}

big_integer &big_integer::operator>>=(int rhs) {
    return *this = *this >> rhs;
}

big_integer operator>>(const big_integer &lhs, int rhs) {
    if (rhs < 0) {
        return lhs << (-rhs);
    }
    size_t skip = rhs / ATOM_SIZE;
    size_t move = rhs % ATOM_SIZE;
    big_integer left = lhs.to_bitwise();

    std::vector<atom> data(left.data.size() - skip, 0);
    for (size_t index = 0; index < data.size(); index++) {
        data[index] += left.get_bitwise_atom(index + skip) >> move;
        if (move != 0) {
            data[index] += left.get_bitwise_atom(index + skip + 1) << (ATOM_SIZE - move);
        }
    }
    return big_integer(lhs.negative, data).from_bitwise();
}

big_integer big_integer::operator+() const {
    return big_integer(*this);
}

big_integer big_integer::operator-() const {
    return big_integer(!this->negative, this->data);
}

big_integer big_integer::operator~() const {
    return -*this - 1;
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

const big_integer big_integer::operator++(int) {
    big_integer res(*this);
    *this += 1;
    return res;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

const big_integer big_integer::operator--(int) {
    big_integer res(*this);
    *this -= 1;
    return res;
}

bool operator==(const big_integer &lhs, const big_integer &rhs) {
    if (lhs.is_zero() || rhs.is_zero()) {
        return lhs.is_zero() == rhs.is_zero();
    }
    if (lhs.data.size() != rhs.data.size() || lhs.negative != rhs.negative) {
        return false;
    }
    for (size_t idx = 0; idx < lhs.data.size(); idx++) {
        if (lhs.data[idx] != rhs.data[idx]) {
            return false;
        }
    };
    return true;
}

bool operator!=(const big_integer &lhs, const big_integer &rhs) {
    return !(lhs == rhs);
}

bool operator<(const big_integer &lhs, const big_integer &rhs) {
    if (lhs.negative != rhs.negative) {
        return lhs.negative && (!lhs.is_zero() || !rhs.is_zero());
    }
    bool abs_lesser = false;
    if (lhs.data.size() != rhs.data.size()) {
        abs_lesser = lhs.data.size() < rhs.data.size();
    } else {
        for (size_t index = 0; index < lhs.data.size(); index++) {
            size_t idx = lhs.data.size() - index - 1;
            if (lhs.get_atom(idx) != rhs.get_atom(idx)) {
                abs_lesser = lhs.get_atom(idx) < rhs.get_atom(idx);
                break;
            }
        }
    }
    return abs_lesser != lhs.negative;
}

bool operator>(const big_integer &lhs, const big_integer &rhs) {
    return rhs < lhs;
}

bool operator<=(const big_integer &lhs, const big_integer &rhs) {
    return !(lhs > rhs);
}

bool operator>=(const big_integer &lhs, const big_integer &rhs) {
    return !(lhs < rhs);
}

std::string big_integer::to_string() const {
    if (*this == 0) {
        return "0";
    }

    std::string str;
    big_integer copy = abs();
    while (copy > 0) {
        big_integer op = copy / 10;
        big_integer digit = copy - 10 * op;
        str.push_back(static_cast<char>('0' + digit.data[0]));
        copy = op;
    }
    if (negative) {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}

big_integer big_integer::abs() const {
    return big_integer(false, data);
}

void big_integer::strip() {
    while (!data.empty() && data.back() == 0) {
        data.pop_back();
    }
    if (data.empty()) {
        data.push_back(0);
    }
}

void big_integer::swap(big_integer &b) {
    big_integer temp(*this);
    negative = b.negative;
    data = b.data;
    b.negative = temp.negative;
    b.data = temp.data;
}

#pragma clang diagnostic pop
