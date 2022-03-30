/*
Author Lucas Farias
*/

#ifndef __TOOLS__HPP__
#define __TOOLS__HPP__

#include <cinttypes>
#include <istream>
#include <ostream>
#include <sstream>

typedef uint32_t word;
typedef uint8_t byte;

class word_stream : public std::basic_stringstream<word> {};

#endif // __TOOLS__HPP__