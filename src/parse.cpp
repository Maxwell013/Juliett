#include <juliett/impl/parse.hpp>

#include <lima/lima.hpp>

#include <cstdlib>

namespace jltt {

Parser::Parser(const std::filesystem::path &path) : _file(), _state(STATE_NONE), _root(nullptr) {
    _file.open(path);
    _state = !_file.is_open() ? STATE_ERROR : STATE_OPEN;
}

void Parser::start() {
    LOG_ASSERT(_state == STATE_OPEN, "Failed to parse file : file not open.");

    _state = STATE_PARSING;
    char token = get();
    switch (token) {
        case '{':
            _root = new JValue(JType::OBJECT);
            break;
        case '[':
            _root = new JValue(JType::ARRAY);
            break;
        default:
            _state = STATE_ERROR;
            return;
    }
    bool success = parse(_root);
    _state = success ? STATE_SUCCESS : STATE_ERROR;
    if (!success) delete _root;
}

void Parser::clean() {
    delete _root;
    _state = STATE_CLEAN;
}

ParserState Parser::state() const {
    return _state;
}

JValue *Parser::root() const {
    return _root;
}

char Parser::get(bool skipWhiteSpace) {
    if (!skipWhiteSpace) return _file.get();
    char ch = _file.get();
    switch (ch) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            return get();
        default:
            return ch;
    }
}

char Parser::peek(bool skipWhiteSpace) {
    if (!skipWhiteSpace) return _file.peek();
    char ch = _file.peek();
    switch (ch) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            get(false);
            return peek();
        default:
            return ch;
    }
}

bool Parser::expect(char ch) {
    return ch == peek();
}

bool Parser::parse(JValue *current) {
    switch (current->type) {
        case JType::OBJECT:
            return parseObject(current);
        case JType::ARRAY:
            return parseArray(current);
        default:
            return false;
    }
}


bool Parser::parseObject(JValue *current) {
    JValue* key = nullptr;
    JValue* value = nullptr;
    JObject* object = current->as<JObject>();

    if (expect('}')) {
        get();
        return true;
    }

    while (true) {

        if (!expect('"')) return false;
        get();

        key = new JValue(JType::STRING);

        if (!parseString(key)) return false;

        if (!expect(':')) return false;
        get();

        if (!parseElement(value)) return false;

        object->emplace(*key->as<JString>(), value);

        delete key;

        switch (get()) {
            case ',': break;
            case '}': return true;
            default: return false;
        }
    }

    return true;
}

bool Parser::parseArray(JValue *current) {
    JValue* element = nullptr;
    JArray* array = current->as<JArray>();

    if (expect(']')) {
        get();
        return true;
    }

    while (true) {
        if (!parseElement(element)) return false;

        array->push_back(element);

        switch (get()) {
            case ',': break;
            case ']': return true;
            default: return false;
        }
    }
}

bool Parser::parseElement(JValue *&element) {
    const char token = get();
    switch (token) {
    case '"':
        element = new JValue(JType::STRING);
        return parseString(element);
    case 't':
        element = new JValue(JType::BOOLEAN);
        *element->as<JBoolean>() = true;
        return parseLitteral("true");
    case 'f':
        element = new JValue(JType::BOOLEAN);
        *element->as<JBoolean>() = false;
        return parseLitteral("false");
    case 'n':
        element = new JValue(JType::NULL_VALUE);
        return parseLitteral("null");
    case '[':
        element = new JValue(JType::ARRAY);
        return parseArray(element);
    case '{':
        element = new JValue(JType::OBJECT);
        return parseObject(element);
    default:
        if (contains(NUMERICALS, token) || token == '-') {
            element = new JValue(JType::NUMBER);
            return parseNumber(element, token);
        }
        return false;
    }
}

bool Parser::parseString(JValue *str) {
    char ch;
    while (true) {
        ch = get(false);
        switch (ch) {
            case '"':
                return true;
            case '\n':
            case '\r':
                return false;
            default:
                str->as<JString>()->push_back(ch);
                break;
        }
    }
}

bool Parser::parseLitteral(std::string litteral) {
    for (size_t i = 1; i < litteral.length(); i++)
        if (get() != litteral[i]) return false;
    return true;
}

bool Parser::parseNumber(JValue *number, const char token) {
    char ch = token;
    std::string num;
    while (true) {
        num += ch;
        ch = peek(false);
        if (!contains(TERMINALS, ch)) {
            get();
            continue;
        }

        try {
            *number->as<JNumber>() = std::stod(num);
        } catch (const std::exception) {
            return false;
        }

        return true;
    }
}

} // namespace jltt
