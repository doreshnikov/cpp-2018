//
// Created by isuca on 23-Jun-18.
//
#ifndef CPP_CIRCULAR_BUFFER_H
#define CPP_CIRCULAR_BUFFER_H

#include <type_traits>
#include <iterator>
#include <cstddef>
#include <memory>
#include <vector>

template<class T>
class circular_buffer {

private:

    template<class V>
    class buffer_iterator : public std::iterator<std::random_access_iterator_tag, V> {

    public:

        friend class circular_buffer<T>;

        typedef typename std::conditional<std::is_const<V>::value, circular_buffer<T> const, circular_buffer<T>>::type
            storage;

        buffer_iterator() :
            _obj(nullptr),
            _range_index(0) {}

        template<class W, class = typename std::enable_if<std::is_const<V>::value || !std::is_const<W>::value>::type>
        buffer_iterator(const buffer_iterator<W> &other) :
            _obj(other._obj),
            _range_index(other._range_index) {}

        template<class W, class = typename std::enable_if<std::is_const<V>::value || !std::is_const<W>::value>::type>
        buffer_iterator<V> &operator=(const buffer_iterator<W> &other) {
            _obj = other._obj;
            _range_index = other._range_index;
            return *this;
        }

        template<class W>
        int operator-(const buffer_iterator<W> &other) const {
            if (_obj != other._obj) {
                throw std::runtime_error("Subtraction of different object iterators");
            }
            return static_cast<int>(_range_index) - static_cast<int>(other._range_index);
        }

        buffer_iterator<V> operator-(int index) const {
            if (index < 0) {
                return *this + (-index);
            }
            if (index > _range_index) {
                throw std::runtime_error("Out-of-bounds iterator arithmetics subtracting");
            }
            return buffer_iterator(_obj, _range_index - index);
        }

        buffer_iterator<V> &operator-=(int index) {
            return *this = *this - index;
        }

        buffer_iterator<V> operator+(int index) const {
            if (index < 0) {
                return *this - (-index);
            }
            if (index + _range_index > _obj->size()) {
                throw std::runtime_error("Out-of-bounds iterator arithmetics");
            }
            return buffer_iterator(_obj, _range_index + index);
        }

        buffer_iterator<V> &operator+=(int index) {
            return *this = *this + index;
        }

        buffer_iterator<V> operator[](size_t index) const {
            return *(this + index);
        }

        buffer_iterator<V> &operator++() {
            return *this += 1;
        }

        buffer_iterator<V> &operator--() {
            return *this -= 1;
        }

        buffer_iterator<V> operator++(int) {
            buffer_iterator<V> res(*this);
            ++*this;
            return res;
        }

        buffer_iterator<V> operator--(int) {
            buffer_iterator<V> res(*this);
            --*this;
            return res;
        }

        template<class W>
        bool operator==(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index == other._range_index;
        }

        template<class W>
        bool operator!=(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index != other._range_index;
        }

        template<class W>
        bool operator<(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index < other._range_index;
        }

        template<class W>
        bool operator<=(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index <= other._range_index;
        }

        template<class W>
        bool operator>(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index > other._range_index;
        }

        template<class W>
        bool operator>=(buffer_iterator<W> const &other) const {
            return _obj == other._obj && _range_index >= other._range_index;
        }

        V &operator*() const {
            return (*_obj)[_range_index];
        }

        V &operator->() const {
            return (*_obj)[_range_index];
        }

    private:

        buffer_iterator(storage *obj, size_t index) :
            _obj(obj),
            _range_index(index) {};

        storage *_obj;
        size_t _range_index;

    };

public:

    template<class V, class W>
    friend bool operator==(buffer_iterator<V> const &left, std::reverse_iterator<buffer_iterator<W>> const &right) {
        return &(*left) == &(*right);
    };
    typedef buffer_iterator<T> iterator;
    typedef buffer_iterator<T const> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    template<class V, class W>
    friend bool operator==(std::reverse_iterator<buffer_iterator<W>> const &left, buffer_iterator<V> const &right) {
        return &(*left) == &(*right);
    };

    circular_buffer() : circular_buffer(DEFAULT_SIZE) {};

    explicit circular_buffer(size_t initial_capacity) :
        _size(0),
        _capacity(initial_capacity * 2),
        _data(reinterpret_cast<T *>(new char[_capacity * sizeof(T)])),
        _begin(_data),
        _end(_begin) {}

    circular_buffer(size_t size, T const &fill) : circular_buffer(size) {
        _end = _begin + _size;
        std::fill(begin(), end(), fill);
    }

    circular_buffer(std::initializer_list<T> initial_values) : circular_buffer(initial_values.size()) {
        for (T const &value : initial_values) {
            push_back(value);
        }
    }

    circular_buffer(circular_buffer<T> const &other) : circular_buffer(other._size) {
        for (T const &it : other) {
            push_back(it);
        }
    }

    circular_buffer<T> &operator=(circular_buffer<T> const &other) {
        if (this != &other) {
            circular_buffer<T> tmp(other);
            swap(tmp);
        }
    }

    ~circular_buffer() {
        while (_size) {
            pop_front();
        }
        delete[] reinterpret_cast<char *>(_data);
    }

    bool push_front(T const &value) noexcept {
        _begin = prev(_begin);
        _size++;
        try {
            new(_begin) T(value);
            if (_size == _capacity) {
                try {
                    enlarge();
                    return true;
                } catch (...) {
                    pop_front();
                    return false;
                }
            }
        } catch (...) {
            _begin = next(_begin);
            _size--;
            return false;
        }
    }

    bool push_back(T const &value) noexcept {
        try {
            new(_end) T(value);
        } catch (...) {
            return false;
        }
        _size++;
        _end = next(_end);
        if (_size == _capacity) {
            try {
                enlarge();
                return true;
            } catch (...) {
                pop_back();
                return false;
            }
        }
    }

    bool pop_front() noexcept {
        if (_size == 0) {
            return false;
        }
        _begin->~T();
        _size--;
        _begin = next(_begin);
        return true;
    }

    bool pop_back() noexcept {
        if (_size == 0) {
            return false;
        }
        _end = prev(_end);
        _end->~T();
        _size--;
        return true;
    }

    iterator insert(const_iterator pos, T const &value) {
        size_t index = pos._range_index;
        std::vector<T> tmp;
        if (index < _size / 2) {
            for (size_t i = 0; i < index; i++) {
                tmp.push_back(front());
                pop_front();
            }
            push_front(value);
            while (!tmp.empty()) {
                push_front(tmp.back());
                tmp.pop_back();
            }
        } else {
            for (; _size > index;) {
                tmp.push_back(back());
                pop_back();
            }
            push_back(value);
            while (!tmp.empty()) {
                push_back(tmp.back());
                tmp.pop_back();
            }
        }

        return iterator(this, index);
    }

    iterator erase(const_iterator pos) {
        if (pos == end()) {
            return end();
        }
        size_t index = pos._range_index;
        std::vector<T> tmp;
        if (index < _size / 2) {
            for (size_t i = 0; i < index; i++) {
                tmp.push_back(front());
                pop_front();
            }
            pop_front();
            while (!tmp.empty()) {
                push_front(tmp.back());
                tmp.pop_back();
            }
        } else {
            for (; _size > index + 1;) {
                tmp.push_back(back());
                pop_back();
            }
            pop_back();
            while (!tmp.empty()) {
                push_back(tmp.back());
                tmp.pop_back();
            }
        }

        return iterator(this, index);
    }

    T const &front() const noexcept {
        return operator[](0);
    }

    T &front() noexcept {
        return operator[](0);
    }

    T const &back() const noexcept {
        return operator[](size() - 1);
    }

    T &back() noexcept {
        return operator[](size() - 1);
    }

    T const &operator[](size_t index) const {
        size_t where = (index + (_begin - _data)) % _capacity;
        return const_cast<T const &>(_data[where]);
    }

    T &operator[](size_t index) {
        size_t where = (index + (_begin - _data)) % _capacity;
        return _data[where];
    }

    size_t size() const noexcept {
        return _size;
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    void clear() noexcept {
        circular_buffer empty;
        swap(empty);
    }

    iterator begin() noexcept {
        return iterator(this, 0);
    }

    const_iterator begin() const noexcept {
        return const_iterator(this, 0);
    }

    iterator end() noexcept {
        return iterator(this, _size);
    }

    const_iterator end() const noexcept {
        return const_iterator(this, _size);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

private:

    void pointer_swap(T *&left, T *&right) {
        T *tmp = left;
        left = right;
        right = tmp;
    }

    void enlarge() {
        auto data = reinterpret_cast<T *>(new char[_capacity * 2 * sizeof(T)]);
        size_t i = 0;
        for (auto t = begin(); t < end(); t++) {
            new(data + (i++)) T(*t);
        }
        circular_buffer create(data, _capacity);
        swap(create);
    }

    template<class V>
    friend void swap(circular_buffer<V> &left, circular_buffer<V> &right);

    void swap(circular_buffer<T> &other) noexcept {
        pointer_swap(_data, other._data);
        pointer_swap(_begin, other._begin);
        pointer_swap(_end, other._end);
        std::swap(_capacity, other._capacity);
        std::swap(_size, other._size);
    }

    circular_buffer(T *data, size_t size) :
        _capacity(size * 2),
        _size(size),
        _data(data),
        _begin(_data),
        _end(_begin + size) {}

    T *next(T *ptr) const noexcept {
        return ptr == _data + _capacity - 1 ? _data : ptr + 1;
    }

    T *prev(T *ptr) const noexcept {
        return ptr == _data ? _data + _capacity - 1 : ptr - 1;
    }

    const static size_t DEFAULT_SIZE = 16;
    size_t _capacity;
    size_t _size;
    T *_data;
    T *_begin;
    T *_end;

};

template<class T>
void swap(circular_buffer<T> &left, circular_buffer<T> &right) {
    left.swap(right);
}

#endif //CPP_CIRCULAR_BUFFER_H
