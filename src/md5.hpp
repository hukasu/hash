/*
Author: Lucas Farias
Reference https://www.rfc-editor.org/rfc/pdfrfc/rfc1321.txt.pdf
*/

#ifndef __MD5__HPP__
#define __MD5__HPP__

#include <array>
#include <functional>

#include "tools.hpp"

class MD5 {
    static word circular_shift_left(word x, word s);
    static word digest_f(word x, word y, word z);
    static word digest_g(word x, word y, word z);
    static word digest_h(word x, word y, word z);
    static word digest_i(word x, word y, word z);

    void calculate_message_with_padding(
        word_stream& padded_message,
        std::istream& message,
        uint64_t length
    );

    void block(
        std::array<word, 4>& result,
        const std::array<word, 16>& message);

    word invert_byte_order(word _word);

    word round_exp(
        word a,
        word b,
        word c,
        word d,
        word _word,
        uint8_t s,
        uint8_t i,
        std::function<word(word, word, word)> digest_function
    );

    void round1(
        word& A,
        word& B,
        word& C,
        word& D,
        const std::array<word, 16>& message
    );

    void round2(
        word& A,
        word& B,
        word& C,
        word& D,
        const std::array<word, 16>& message
    );

    void round3(
        word& A,
        word& B,
        word& C,
        word& D,
        const std::array<word, 16>& message
    );

    void round4(
        word& A,
        word& B,
        word& C,
        word& D,
        const std::array<word, 16>& message
    );

public:
    std::array<word, 4> digest(
        std::istream& message,
        uint64_t length
    );
};

#endif // __MD5__HPP__