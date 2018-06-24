//
// Created by amathael on 09.06.18.
//

#include <queue>
#include <iostream>

#include "huffman.h"
#include "custom_writer.h"
#include "custom_reader.h"

huffman::tree_node::tree_node() : _s(0), _count(0), _l(nullptr), _r(nullptr), _filled(false) {}

huffman::tree_node::tree_node(symbol s, uint64_t count) : _s(s), _count(count), _l(nullptr), _r(nullptr),
                                                          _filled(true) {}

huffman::tree_node::tree_node(node_ptr l, node_ptr r) : _s(0), _count(l->_count + r->_count), _l(l), _r(r),
                                                        _filled(false) {}

huffman::tree_node::~tree_node() {
    delete _l;
    delete _r;
}

bool huffman::tree_node::is_terminal() const {
    return _l == nullptr && _r == nullptr;
}

bool operator<(huffman::tree_node const &lhs, huffman::tree_node const &rhs) {
    return lhs._count < rhs._count;
}

void huffman::tree_node::add(symbol s, code &c, uint32_t depth) {
    if (depth < c.length()) {
        if (c.get(depth) == 0) {
            if (_l == nullptr) {
                _l = new tree_node;
            }
            _l->add(s, c, depth + 1);
        } else {
            if (_r == nullptr) {
                _r = new tree_node;
            }
            _r->add(s, c, depth + 1);
        }
    } else {
        if (this->_filled) {
            throw identical_codes_exception();
        }
        this->_s = s;
        this->_filled = true;
    }
}

void huffman::tree_node::collect(header_data &header, code c) const {
    if (is_terminal()) {
        header.add_symbol(_s, c);
    } else {
        if (_l != nullptr) {
            _l->collect(header, code(c, 0));
        }
        if (_r != nullptr) {
            _r->collect(header, code(c, 1));
        }
    }
}

huffman::huffman() : _walk(nullptr), _root(nullptr) {}

void huffman::initialize(frequency_info &frequency) {
    std::priority_queue<node_ptr, std::vector<node_ptr>, huffman::tree_node::comparator> queue;
    for (auto &elem : frequency.get_data()) {
        queue.push(new tree_node(elem.first, elem.second));
    }

    if (queue.size() == 1) {
        queue.push(new tree_node(static_cast<symbol>(queue.top()->_s + 1), 0));
    }

    while (!queue.empty()) {
        if (queue.size() == 1) {
            _root = queue.top();
            queue.pop();
        } else {
            node_ptr l = queue.top();
            queue.pop();
            node_ptr r = queue.top();
            queue.pop();
            queue.push(new tree_node(l, r));
        }
    }
    create_header();
}

void huffman::initialize(header_data &header) {
    _root = new tree_node;
    _header = header;

    for (auto &elem : header.get_data()) {
        _root->add(elem.first, elem.second, 0);
    }
    count_codes();
}

huffman::~huffman() {
    delete _root;
}

void huffman::create_header() {
    _root->collect(_header, code());
    count_codes();
}

void huffman::count_codes() {
    _codes = std::vector<code>(MAX_SYMBOL + 1ul);
    for (const auto &p : _header.get_data()) {
        _codes[p.first] = p.second;
    }
}

bool huffman::feed(symbol &s, uint8_t bit) {
    if (_walk == nullptr) {
        _walk = _root;
    }
    if (bit == 0) {
        _walk = _walk->_l;
    } else {
        _walk = _walk->_r;
    }
    if (_walk == nullptr) {
        throw feed_exception();
    }
    if (_walk->is_terminal()) {
        s = _walk->_s;
        _walk = nullptr;
        return true;
    }
    return false;
}

void huffman::encode(custom_reader &in, custom_writer &out) {
    frequency_info frequency;
    while (!in.eof()) {
        symbol s = in.get_symbol();
        frequency.add_symbol(s);
    }

    if (frequency.count() == 0) {
        out.write_byte(0);
        out.close();
        return;
    } else {
        out.write_byte(1);
    }

    in.to_start();
    initialize(frequency);

    uint64_t length = 0;
    out.write_symbol(static_cast<symbol>(_header.count()));
    for (const auto &s : _header.get_data()) {
        out.write_symbol(s.first);
        out.write_symbol(static_cast<symbol>(s.second.length()));
        uint32_t c = s.second.size();
        for (uint32_t i = 0; i < c; i++) {
            out.write_byte(s.second.get_byte(i));
        }
    }

    while (!in.eof()) {
        code const &cur = _codes[in.get_symbol()];
        uint32_t len = cur.length();
        length += len;
        uint32_t c = cur.size();
        for (uint32_t i = 0; i < c; i++) {
            if (len >= 8) {
                out.write_byte(cur.get_byte(i));
                len -= 8;
            } else {
                out.write_bits(cur.get_byte(i), static_cast<uint8_t>(len));
            }
        }
    }

    out.complete_byte();
    auto last = static_cast<uint8_t>(length % 8);
    out.write_byte(last == 0 ? static_cast<uint8_t>(8) : last);
    out.close();
}

bool huffman::decode(custom_reader &in, custom_writer &out) {
    try {
        header_data header;

        if (in.get_byte() == 0) {
            return in.eof();
        }

        symbol cnt_s = in.get_symbol();
        uint32_t count = cnt_s == 0 ? 1u << SYMBOL_BITS : cnt_s;

        for (uint32_t c = 0; c < count; c++) {
            symbol s = in.get_symbol();
            uint32_t len = in.get_symbol();
            code cur;
            for (uint32_t i = 0; i < len; i++) {
                cur.push_back(in.get_bit());
            }
            header.add_symbol(s, cur);
            in.complete_byte();
        }

        initialize(header);
        uint8_t acc = 0, idx = 0;
        if (in.next_is_last()) {
            return false;
        }
        while (!in.next_is_last()) {
            symbol s = 0;
            bool full = true;
            do {
                if (idx == 0) {
                    acc = in.get_byte();
                    idx = 8;
                }
                if (in.next_is_last()) {
                    full = false;
                    break;
                }
                idx--;
            } while (!feed(s, static_cast<uint8_t>((acc >> idx) & 1))); // NOLINT
            if (full) {
                out.write_symbol(s);
            }
        }

        uint8_t cnt = in.get_byte();
        uint32_t shift = 8;
        while (cnt != 0) {
            symbol s;
            do {
                if (cnt == 0 || shift == 0) {
                    return false;
                }
                cnt--;
                shift--;
            } while (!feed(s, static_cast<uint8_t>(acc >> shift & 1u))); // NOLINT
            out.write_symbol(s);
        }

        return true;
    } catch (custom_reader::eof_exception &e) {
        return false;
    } catch (huffman::feed_exception &e) {
        return false;
    } catch (huffman::identical_codes_exception &e) {
        return false;
    }
}