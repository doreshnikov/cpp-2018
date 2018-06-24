//
// Created by amathael on 09.06.18.
//

#ifndef CPP_HUFFMAN_TREE_H
#define CPP_HUFFMAN_TREE_H

#include "_literals.h"
#include "code.h"
#include "frequency_info.h"
#include "header_data.h"
#include "custom_reader.h"
#include "custom_writer.h"

class huffman {

public:

    huffman();

    ~huffman();

    class feed_exception : std::exception {
    };

    class identical_codes_exception : std::exception {
    };

    void encode(custom_reader &in, custom_writer &out);

    bool decode(custom_reader &in, custom_writer &out);

private:

    class tree_node;

    typedef tree_node *node_ptr;

    friend bool operator<(tree_node const &lhs, tree_node const &rhs);

    struct tree_node {

        tree_node();

        tree_node(symbol s, uint64_t count);

        tree_node(node_ptr l, node_ptr r);

        ~tree_node();

        bool is_terminal() const;

        void add(symbol s, code &c, uint32_t depth);

        struct comparator {
            bool operator()(node_ptr &lhs, node_ptr &rhs) {
                return lhs->_count > rhs->_count;
            }
        };

        void collect(header_data &header, code c) const;

        node_ptr _l;
        node_ptr _r;
        bool _filled;
        symbol _s;

        uint64_t _count;

    };

    void initialize(header_data &header);

    void initialize(frequency_info &frequency);

    bool feed(symbol &s, uint8_t bit);

    header_data _header;
    std::vector<code> _codes;
    node_ptr _root;
    node_ptr _walk;

    void create_header();

    void count_codes();

};

#endif //CPP_HUFFMAN_TREE_H
