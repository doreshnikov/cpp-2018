//
// Created by amathael on 16.06.18.
//

#ifndef CPP_STORAGE_H
#define CPP_STORAGE_H

#include <utility>
#include <memory>
#include <vector>
#include "digit_types.h"

class shared_vector {

public:

    shared_vector();

    explicit shared_vector(size_t size);

    explicit shared_vector(std::vector<atom> const &other);

    shared_vector(size_t size, atom fill);

    shared_vector(shared_vector const &other);

    shared_vector &operator=(shared_vector other);

    ~shared_vector();

    void swap(shared_vector &other);

    bool empty() const;

    size_t size() const;

    atom &operator[](size_t index);

    atom const &operator[](size_t index) const;

    void push_back(atom element);

    void pop_back();

    atom &back();

    atom const &back() const;

private:

    size_t _size;
    bool _is_small;

    struct dynamic_storage {

        size_t _capacity;
        std::shared_ptr<atom> _head;

        dynamic_storage() : _capacity(0), _head(nullptr) {};

        explicit dynamic_storage(size_t size) :
                _capacity(2 * size), _head(new atom[_capacity], std::default_delete<atom[]>()) {};

        dynamic_storage(size_t size, atom *array) : _capacity(2 * size), _head(array, std::default_delete<atom[]>()) {};

        void ensure_capacity(size_t size);

    };

    const static size_t SMALL_SIZE = sizeof(dynamic_storage) / sizeof(atom);

    union union_storage {

        atom small_obj[shared_vector::SMALL_SIZE];
        dynamic_storage big_obj;

        union_storage() {};

        ~union_storage() {};

    } _storage;

    void cut();

    atom *begin() const;

};

#endif //CPP_STORAGE_H
