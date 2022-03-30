#include "md5.hpp"

#include <vector>
#include <cmath>

// Sizes in bytes. 56 bytes = 448 bits. 64 bytes = 512 bits.
#define _32_BITS_L 4
#define _64_BITS_L 8
#define _448_BITS_L 56
#define _512_BITS_L 64

#define WORDS_PER_BLOCK 16

const std::array<word, 64> table = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

word MD5::circular_shift_left(word x, word s) {
    uint8_t n_s = 32 - s;
    return ((x << s) | (x >> n_s));
}

word MD5::digest_f(word x, word y, word z) {
    return (x & y) | (~x & z);
}

word MD5::digest_g(word x, word y, word z) {
    return (x & z) | (y & ~(z));
}

word MD5::digest_h(word x, word y, word z) {
    return x ^ y ^ z;
}

word MD5::digest_i(word x, word y, word z) {
    return y ^ (x | ~z);
}

void MD5::calculate_message_with_padding(
    word_stream& padded_message,
    std::istream& message,
    uint64_t length
) {
    uint64_t length_in_words = uint64_t(length / sizeof(word));
    uint64_t bytes_leftover = length % sizeof(word);

    // Copying message into word_stream
    if (length_in_words){ 
        std::vector<word> message_block = std::vector<word>(length_in_words, 0);
        message.read(reinterpret_cast<char*>(message_block.data()), length_in_words * sizeof(word));
        padded_message.write(message_block.data(), length_in_words);
    }

    // Padding
    // First padded word
    word first_pad = 0;
    if (bytes_leftover) {
        message.read(reinterpret_cast<char*>(&first_pad), bytes_leftover);
    }
    word first_pad_byte = 0x80 << (bytes_leftover * 8);
    first_pad = first_pad | first_pad_byte;
    padded_message.write(&first_pad, 1);

    // Zero paddings
    // -1 for the word written above
    // -2 for the 2 words of length written at the end
    int64_t words_for_padding = ((WORDS_PER_BLOCK - length_in_words) % WORDS_PER_BLOCK) - 3;
    if (words_for_padding < 0) {
        words_for_padding += WORDS_PER_BLOCK;
    }
    std::vector<word> zeros(words_for_padding, 0);
    padded_message.write(zeros.data(), words_for_padding);

    // Original message length
    union {
        struct {
            word a;
            word b;
        };
        uint64_t c;
    } length_to_words;
    length_to_words.c = length * 8;

    padded_message.write(&(length_to_words.a), 1);
    padded_message.write(&(length_to_words.b), 1);
}

void MD5::block(
    std::array<word, 4>& result,
    const std::array<word, 16>& message
) {
    word& A = result[0];
    word& B = result[1];
    word& C = result[2];
    word& D = result[3];

    word AA = A;
    word BB = B;
    word CC = C;
    word DD = D;

    round1(AA, BB, CC, DD, message);
    round2(AA, BB, CC, DD, message);
    round3(AA, BB, CC, DD, message);
    round4(AA, BB, CC, DD, message);

    A = A + AA;
    B = B + BB;
    C = C + CC;
    D = D + DD;
}


word MD5::invert_byte_order(word _word) {
    union {
        word w;
        struct {
            uint8_t a, b, c, d;
        };
    } inverter;
    inverter.w = _word;
    std::swap(inverter.a, inverter.d);
    std::swap(inverter.b, inverter.c);
    return inverter.w;
}

word MD5::round_exp(
    word a,
    word b,
    word c,
    word d,
    word _word,
    uint8_t s,
    uint8_t i,
    std::function<word(word, word, word)> digest_function
) {
    return b + circular_shift_left(a + digest_function(b, c, d) + _word + table[i], s);
}

void MD5::round1(
    word& A,
    word& B,
    word& C,
    word& D,
    const std::array<word, 16>& _block
) {
    A = round_exp(A, B, C, D, _block[ 0],  7,  0, digest_f);
    D = round_exp(D, A, B, C, _block[ 1], 12,  1, digest_f);
    C = round_exp(C, D, A, B, _block[ 2], 17,  2, digest_f);
    B = round_exp(B, C, D, A, _block[ 3], 22,  3, digest_f);
    A = round_exp(A, B, C, D, _block[ 4],  7,  4, digest_f);
    D = round_exp(D, A, B, C, _block[ 5], 12,  5, digest_f);
    C = round_exp(C, D, A, B, _block[ 6], 17,  6, digest_f);
    B = round_exp(B, C, D, A, _block[ 7], 22,  7, digest_f);
    A = round_exp(A, B, C, D, _block[ 8],  7,  8, digest_f);
    D = round_exp(D, A, B, C, _block[ 9], 12,  9, digest_f);
    C = round_exp(C, D, A, B, _block[10], 17, 10, digest_f);
    B = round_exp(B, C, D, A, _block[11], 22, 11, digest_f);
    A = round_exp(A, B, C, D, _block[12],  7, 12, digest_f);
    D = round_exp(D, A, B, C, _block[13], 12, 13, digest_f);
    C = round_exp(C, D, A, B, _block[14], 17, 14, digest_f);
    B = round_exp(B, C, D, A, _block[15], 22, 15, digest_f);
}

void MD5::round2(
    word& A,
    word& B,
    word& C,
    word& D,
    const std::array<word, 16>& _block
) {
    A = round_exp(A, B, C, D, _block[ 1],  5, 16, digest_g);
    D = round_exp(D, A, B, C, _block[ 6],  9, 17, digest_g);
    C = round_exp(C, D, A, B, _block[11], 14, 18, digest_g);
    B = round_exp(B, C, D, A, _block[ 0], 20, 19, digest_g);
    A = round_exp(A, B, C, D, _block[ 5],  5, 20, digest_g);
    D = round_exp(D, A, B, C, _block[10],  9, 21, digest_g);
    C = round_exp(C, D, A, B, _block[15], 14, 22, digest_g);
    B = round_exp(B, C, D, A, _block[ 4], 20, 23, digest_g);
    A = round_exp(A, B, C, D, _block[ 9],  5, 24, digest_g);
    D = round_exp(D, A, B, C, _block[14],  9, 25, digest_g);
    C = round_exp(C, D, A, B, _block[ 3], 14, 26, digest_g);
    B = round_exp(B, C, D, A, _block[ 8], 20, 27, digest_g);
    A = round_exp(A, B, C, D, _block[13],  5, 28, digest_g);
    D = round_exp(D, A, B, C, _block[ 2],  9, 29, digest_g);
    C = round_exp(C, D, A, B, _block[ 7], 14, 30, digest_g);
    B = round_exp(B, C, D, A, _block[12], 20, 31, digest_g);
}

void MD5::round3(
    word& A,
    word& B,
    word& C,
    word& D,
    const std::array<word, 16>& _block
) {
    A = round_exp(A, B, C, D, _block[ 5],  4, 32, digest_h);
    D = round_exp(D, A, B, C, _block[ 8], 11, 33, digest_h);
    C = round_exp(C, D, A, B, _block[11], 16, 34, digest_h);
    B = round_exp(B, C, D, A, _block[14], 23, 35, digest_h);
    A = round_exp(A, B, C, D, _block[ 1],  4, 36, digest_h);
    D = round_exp(D, A, B, C, _block[ 4], 11, 37, digest_h);
    C = round_exp(C, D, A, B, _block[ 7], 16, 38, digest_h);
    B = round_exp(B, C, D, A, _block[10], 23, 39, digest_h);
    A = round_exp(A, B, C, D, _block[13],  4, 40, digest_h);
    D = round_exp(D, A, B, C, _block[ 0], 11, 41, digest_h);
    C = round_exp(C, D, A, B, _block[ 3], 16, 42, digest_h);
    B = round_exp(B, C, D, A, _block[ 6], 23, 43, digest_h);
    A = round_exp(A, B, C, D, _block[ 9],  4, 44, digest_h);
    D = round_exp(D, A, B, C, _block[12], 11, 45, digest_h);
    C = round_exp(C, D, A, B, _block[15], 16, 46, digest_h);
    B = round_exp(B, C, D, A, _block[ 2], 23, 47, digest_h);
}

void MD5::round4(
    word& A,
    word& B,
    word& C,
    word& D,
    const std::array<word, 16>& _block
) {
    A = round_exp(A, B, C, D, _block[ 0],  6, 48, digest_i);
    D = round_exp(D, A, B, C, _block[ 7], 10, 49, digest_i);
    C = round_exp(C, D, A, B, _block[14], 15, 50, digest_i);
    B = round_exp(B, C, D, A, _block[ 5], 21, 51, digest_i);
    A = round_exp(A, B, C, D, _block[12],  6, 52, digest_i);
    D = round_exp(D, A, B, C, _block[ 3], 10, 53, digest_i);
    C = round_exp(C, D, A, B, _block[10], 15, 54, digest_i);
    B = round_exp(B, C, D, A, _block[ 1], 21, 55, digest_i);
    A = round_exp(A, B, C, D, _block[ 8],  6, 56, digest_i);
    D = round_exp(D, A, B, C, _block[15], 10, 57, digest_i);
    C = round_exp(C, D, A, B, _block[ 6], 15, 58, digest_i);
    B = round_exp(B, C, D, A, _block[13], 21, 59, digest_i);
    A = round_exp(A, B, C, D, _block[ 4],  6, 60, digest_i);
    D = round_exp(D, A, B, C, _block[11], 10, 61, digest_i);
    C = round_exp(C, D, A, B, _block[ 2], 15, 62, digest_i);
    B = round_exp(B, C, D, A, _block[ 9], 21, 63, digest_i);
}

std::array<word, 4> MD5::digest(
    std::istream& message,
    uint64_t length
) {
    word_stream padded_message;
    calculate_message_with_padding(
        padded_message,
        message,
        length
    );

    uint64_t padded_l;
    padded_message.seekp(0, std::ios::end);
    padded_l = padded_message.tellp();
    padded_message.seekp(0, std::ios::beg);
    
    if (padded_l % 16)
        exit(3);

    std::array<word, 4> result = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476
    };

    std::array<word, 16> _block;
    for (uint64_t i = 0; i < padded_l; i += 16) {
        padded_message.read(_block.data(), 16);
        block(result, _block);
    }

    result[0] = invert_byte_order(result[0]);
    result[1] = invert_byte_order(result[1]);
    result[2] = invert_byte_order(result[2]);
    result[3] = invert_byte_order(result[3]);

    return result;
}