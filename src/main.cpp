/*
Author: Lucas Farias
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include <functional>

#include "md5.hpp"

uint64_t open_file(std::ifstream& stream, std::string filepath) {
    stream = std::ifstream();
    stream.open(filepath);

    uint64_t length;
    stream.seekg(0, std::ios::end);
    length = uint64_t(stream.tellg());
    stream.seekg(0, std::ios::beg);

    return length;
}

int run_md5(
    std::istream& message,
    uint64_t length,
    std::string hash
) {
    MD5 md5;
    std::array<word, 4> digest = md5.digest(message, length);

    std::stringstream result;
    result 
        << std::setfill('0')
        << std::setw(8)
        << std::hex
        << digest[0]
        << digest[1]
        << digest[2]
        << digest[3];

    if (hash.empty()) {
        std::cout << result.str() << std::endl;
        return 0;
    } else {
        std::cout << result.str() << " : " << hash << std::endl;
        return !(result.str() == hash);
    }
}

int main(int argc, char** argv) {
    std::string algorithm;
    std::string mode;
    std::string filepath_or_text;
    std::string hash;
    
    if (argc == 4) {
        algorithm = argv[1];
        mode = argv[2];
        filepath_or_text = argv[3];
    } else if (argc == 5) {
        algorithm = argv[1];
        mode = argv[2];
        filepath_or_text = argv[3];
        hash = argv[4];
    } else {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << "<algorithm>  {-f} <filepath> <hash>?" << std::endl;
        std::cout << argv[0] << "<algorithm>  {-t} <text> <hash>?" << std::endl;
        return 2;
    }

    std::function<int(std::istream&, uint64_t, std::string)> alg_runner;
    if (algorithm == "MD5") {
        alg_runner = run_md5;
    } else if (algorithm == "md5") {
        // Alias for MD5
        alg_runner = run_md5;
    } else {
        std::cout << "Invalid algorithm \"" << algorithm << "\"." << std::endl;
        std::cout << "Know algorithms are: MD5." << std::endl;
        return 2;
    }

    uint64_t message_length;
    if (mode == "-f") {
        std::ifstream file_stream;
        message_length = open_file(file_stream, filepath_or_text);

        return alg_runner(file_stream, message_length, hash);
    } else if (mode == "-t") {
        std::istringstream string_stream(filepath_or_text);
        message_length = filepath_or_text.length();
        
        return alg_runner(string_stream, message_length, hash);
    } else {
        std::cout << "Invalid mode \"" << mode << "\"." << std::endl;
        std::cout << "Know modes are: -t, -f." << std::endl;
        return 2;
    }

}

// "args": [
//     "MD5",
//     "-t",
//     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
// ]

// "args": [
//     "-sABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
// ]