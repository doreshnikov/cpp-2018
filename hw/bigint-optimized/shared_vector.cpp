//
// Created by amathael on 16.06.18.
//

#include <algorithm>
#include <cstring>
#include "shared_vector.h"

shared_vector::shared_vector() : shared_vector(0) {}

shared_vector::shared_vector(size_t size) : shared_vector(size, 0) {}

shared_vector::shared_vector(size_t size, atom fill) : _size(size), _is_small(size <= shared_vector::SMALL_SIZE) {
    if (!_is_small) {
        new(&_storage.big_obj) dynamic_storage(_size);
    }
    std::fill(begin(), begin() + _size, fill);
}

shared_vector::shared_vector(shared_vector const &other) : _size(other._size), _is_small(other._is_small) {
    if (_is_small) {
        std::copy(other.begin(), other.begin() + _size, begin());
    } else {
        new(&_storage.big_obj) dynamic_storage(other._storage.big_obj);
    }
}

shared_vector::shared_vector(std::vector<atom> const &other) : shared_vector(other.size()) {
    std::copy(other.begin(), other.end(), begin());
}

shared_vector &shared_vector::operator=(shared_vector other) {
    swap(other);
    return *this;
}

shared_vector::~shared_vector() {
    if (!_is_small) {
        _storage.big_obj.~dynamic_storage();
    }
}

void shared_vector::swap(shared_vector &other) {
    std::swap(_size, other._size);
    std::swap(_is_small, other._is_small);

    char *data[sizeof(dynamic_storage)];
    mempcpy(data, &_storage, sizeof(dynamic_storage));
    mempcpy(&_storage, &other._storage, sizeof(dynamic_storage));
    mempcpy(&other._storage, data, sizeof(dynamic_storage));
}

bool shared_vector::empty() const {
    return _size == 0;
}

size_t shared_vector::size() const {
    return _size;
}

atom &shared_vector::operator[](size_t index) {
    cut();
    return begin()[index];
}

atom const &shared_vector::operator[](size_t index) const {
    return begin()[index];
}

void shared_vector::push_back(atom element) {
    cut();
    if (_is_small && _size == shared_vector::SMALL_SIZE) {
        auto data = new atom[_size + 1];
        std::copy(begin(), begin() + _size, data);
        data[_size++] = element;

        new(&_storage.big_obj) dynamic_storage(_size, data);
        _is_small = false;
    } else {
        if (!_is_small) {
            _storage.big_obj.ensure_capacity(_size);
        }
        begin()[_size++] = element;
    }
}

void shared_vector::pop_back() {
    cut();
    _size--;
}

atom &shared_vector::back() {
    cut();
    return *(begin() + _size - 1);
}

atom const &shared_vector::back() const {
    return *(begin() + _size - 1);
}

void shared_vector::dynamic_storage::ensure_capacity(size_t size) {
    if (size == _capacity) {
        _capacity *= 2;
        auto data = new atom[size];
        std::copy(_head.get(), _head.get() + size, data);
        _head = std::shared_ptr<atom>(data, std::default_delete<atom[]>());
    }
}

void shared_vector::cut() {
    if (!_is_small && !_storage.big_obj._head.unique()) {
        auto data = new atom[_storage.big_obj._capacity];
        std::copy(begin(), begin() + _size, data);
        _storage.big_obj._head.reset(data, std::default_delete<atom[]>());
    }
}

atom *shared_vector::begin() const {
    return _is_small ? const_cast<atom *>(_storage.small_obj) : _storage.big_obj._head.get();
}
