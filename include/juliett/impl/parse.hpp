#pragma once

#include <cstddef>
#include <fstream>
#include <string>

#include <juliett/impl/value.hpp>

namespace jltt {

enum ParserState {
    STATE_NONE,
    STATE_OPEN,
    STATE_PARSING,
    STATE_SUCCESS,
    STATE_CLEAN,
    STATE_ERROR
};

class Parser {

public:

    Parser(const std::filesystem::path &path);
    void start();
    void clean();
    ParserState state() const;
    JValue *root() const;

private:
    std::ifstream _file;
    ParserState _state;
    JValue *_root;

    static constexpr char NUMERICALS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    static constexpr char TERMINALS[3] = { ',', '}', ']' };

    template<typename Type, size_t N>
    static constexpr bool contains(const Type (&array)[N], const Type &element) {
        for (size_t i = 0; i < N; i++)
            if (array[i] == element) return true;
        return false;
    }

    char get(bool skipWhiteSpace = true);
    char peek(bool skipWhiteSpace = true);
    bool expect(char ch);

    bool parse(JValue *current);
    bool parseObject(JValue *current);
    bool parseArray(JValue *current);
    bool parseElement(JValue *&element);
    bool parseString(JValue *str);
    bool parseLitteral(std::string litteral);
    bool parseNumber(JValue *number, const char token);
};

} // namespace jltt
