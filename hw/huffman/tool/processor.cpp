//
// Created by amathael on 02.06.18.
//

#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>

#include "../lib/huffman.h"
#include "buffered_reader.h"
#include "buffered_writer.h"

char const *usage = "Usage: ./huffman (-e|-d|-r) <from> <to>\n"
                    " : -e for encoding\n"
                    " : -d for decoding\n"
                    " : -r for encoding and decoding using buffer\n";

int main(int argc, char **argv) {

    if (argc != 4) {
        std::cout << usage;
        return 0;
    }

    buffered_reader in(argv[2]);
    buffered_writer out(argv[3]);
    auto start = std::chrono::system_clock::now();
    std::cout << "Starting..\n";
    if (strcmp(argv[1], "-e") == 0) {
        huffman().encode(in, out);
    } else if (strcmp(argv[1], "-d") == 0) {
        if (!huffman().decode(in, out)) {
            std::cout << "It seems like your input file is damaged or incorrect\n";
        }
    } else if (strcmp(argv[1], "-r") == 0) {
        buffered_writer tmp_out("tmp_encode");
        huffman().encode(in, tmp_out);
        tmp_out.close();

        buffered_reader tmp_in("tmp_encode");
        huffman().decode(tmp_in, out);
    } else {
        std::cout << "Invalid flag\n" << usage;
        return 0;
    }

    auto finish = std::chrono::system_clock::now();
    std::cout << "Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << '\n';

    return 0;

}