//
// Created by amathael on 02.06.18.
//

#include <fstream>
#include <chrono>

#include "../../gtest/gtest.h"
#include "../tool/buffered_reader.h"
#include "../tool/buffered_writer.h"
#include "../lib/code.h"
#include "../lib/huffman.h"

namespace {

    char const *input = "tmp_input";
    char const *encode = "tmp_encode";
    char const *output = "tmp_output";

    bool compare_files(char const *f1, char const *f2) {
        std::ifstream file1(f1, std::ios::in | std::ios::binary);
        std::ifstream file2(f2, std::ios::in | std::ios::binary);
        if (!file1 || !file2) {
            file1.close();
            file2.close();
            return false;
        }
        while (!file1.eof() && !file2.eof()) {
            if (file1.get() != file2.get()) {
                file1.close();
                file2.close();
                return false;
            }
        }
        bool res = file1.eof() == file2.eof();
        file1.close();
        file2.close();
        return res;
    }

    bool test_special_file(char const *filename, bool log_time = true) {
        buffered_reader i(filename);
        buffered_writer e(encode);
        auto pre_encode = std::chrono::system_clock::now();
        huffman().encode(i, e);
        auto encoded = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - pre_encode).count();
        i.close();
        e.close();

        buffered_reader d(encode);
        buffered_writer o(output);
        auto pre_decode = std::chrono::system_clock::now();
        if (!huffman().decode(d, o)) {
            return false;
        }
        auto decoded = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - pre_decode).count();

        if (log_time) {
            std::cout << "[----------] e/d in " << encoded << ' ' << decoded << '\n';
        }
        d.close();
        o.close();

        return compare_files(filename, output);
    }

    bool test_string(char const *str) {
        std::ofstream inp(input, std::ios::out | std::ios::binary);
        inp.write(str, strlen(str));
        inp.close();

        return test_special_file(input, false);
    }

    std::vector<uint8_t> to_byte_vector(char const *bytes) {
        std::vector<uint8_t> res;
        int i = 0;
        uint8_t acc = 0;
        for (char c : std::string(bytes)) {
            if (c == ' ') {
                continue;
            }
            uint8_t half = 0;
            if ('0' <= c && c <= '9') {
                half = static_cast<uint8_t>(c - '0');
            } else {
                half = static_cast<uint8_t>(c - 'a' + 10);
            }
            if (i == 0) {
                acc = half;
            } else {
                res.push_back((acc << 4) | half); // NOLINT
                acc = 0;
            }
            i ^= 1;
        }
        return res;
    }

    bool try_decode(std::vector<uint8_t> data) {
        std::ofstream inp(encode, std::ios::out | std::ios::binary);
        inp.write(reinterpret_cast<const char *>(data.data()), data.size());
        inp.close();

        buffered_reader i(encode);
        buffered_writer o(output);
        return huffman().decode(i, o);
    }

}

TEST(correct_file_checker, incorrect) {
    ASSERT_FALSE(compare_files("eq1.txt", "eq-.txt"));
}

TEST(correct_file_checker, correct) {
    ASSERT_TRUE(compare_files("eq1.txt", "eq2.txt"));
}

TEST(correct_codes, small) {
    code c(code(), 1);
    ASSERT_EQ(c.to_string(), "1");
}

TEST(correct_codes, large) {
    code c(code(), 0);
    c.append(code(c));
    c.append(code(c));
    c.append(code(c));
    c.push_back(1);
    ASSERT_EQ(c.to_string(), "000000001");
}

TEST(correctness, empty) {
    test_string("");
}

TEST(correctness, singleton) {
    test_string("a");
    test_string("\1");
    test_string("0");
}

TEST(correctness, full_last_byte) {
    test_string("aaaaaaaa");
    test_string("ANIMEanime\nanimeANIME\n11");
}

TEST(correctness, random) {
    test_string("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqwerty1234");
    test_string("aaaaaaaabbbbbbbb111111111111111122222222222222222222222222222222");
}

TEST(correctness, vojna_i_mir_1) {
    test_special_file("vim1.txt");
}

TEST(correctness_broken, empty) {
    ASSERT_FALSE(try_decode(to_byte_vector("")));
}

TEST(correctness_broken, last_byte_lesser) {
    ASSERT_FALSE(try_decode(to_byte_vector("01 0c 69 03 00 65 03 20  6d 03 40 4d 03 60 61 04"
                                           "80 45 04 90 31 04 a0 49  04 b0 0a 04 c0 41 04 d0"
                                           "4e 04 e0 6e 04 f0 de b7  31 e1 1c 8f 08 ef 5b 9c"
                                           "aa 06")));
}

TEST(correctness, last_byte_ok) {
    // @formatter:off
    ASSERT_TRUE( try_decode(to_byte_vector("01 0c 69 03 00 65 03 20  6d 03 40 4d 03 60 61 04"
                                           "80 45 04 90 31 04 a0 49  04 b0 0a 04 c0 41 04 d0"
                                           "4e 04 e0 6e 04 f0 de b7  31 e1 1c 8f 08 ef 5b 9c"
                                           "aa 08")));
    // @formatter:on
}

TEST(correctness_broken, last_byte_larger) {
    ASSERT_FALSE(try_decode(to_byte_vector("01 0c 69 03 00 65 03 20  6d 03 40 4d 03 60 61 04"
                                           "80 45 04 90 31 04 a0 49  04 b0 0a 04 c0 41 04 d0"
                                           "4e 04 e0 6e 04 f0 de b7  31 e1 1c 8f 08 ef 5b 9c"
                                           "aa 0a")));
}

TEST(correctness_broken, random) {
    ASSERT_FALSE(try_decode(to_byte_vector("01 02 00 03 00 01 03 f0  10 01 08")));
}

TEST(correctness_broken, identical_codes) {
    ASSERT_FALSE(try_decode(to_byte_vector("01 02 00 03 00 01 03 0f  00 00 01")));
}

TEST(correctness_speed, 10Mb_uniform_data) {
    test_special_file("test10MbUniform.test");
}

TEST(correctness_speed, 80Mb_uniform_data) {
    test_special_file("test80MbUniform.test");
}

TEST(correctness_speed, 100Mb_actual_data) {
    test_special_file("test100MbActual.test");
}
