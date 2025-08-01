#pragma once

#include <cstddef>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>

namespace jltt {

struct JValue;
enum class JType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_VALUE
};

typedef nullptr_t JNull;
typedef bool JBoolean;
typedef double JNumber;
typedef std::string JString;
typedef std::vector<JValue*> JArray;
typedef std::pair<JString, JValue*> JPair;
typedef std::unordered_map<JString, JValue*> JObject;

struct JValue {
    JType type;
    void *data;

    JValue();
    JValue(JType type);
    ~JValue();

    std::string tostr() const;
    size_t length() const;
    JValue* at(size_t index);
    JValue* at(JString key);

    template<typename Type>
    constexpr bool is() const {
        switch (type) {
        case JType::OBJECT:
            return std::is_same_v<Type, JObject>;
        case JType::ARRAY:
            return std::is_same_v<Type, JArray>;
        case JType::STRING:
            return std::is_same_v<Type, JString>;
        case JType::NUMBER:
            return std::is_same_v<Type, JNumber>;
        case JType::BOOLEAN:
            return std::is_same_v<Type, JBoolean>;
        default:
            return false;
        }
    }

    template<typename Type>
    constexpr Type* as() {
        if (is<Type>())
            return static_cast<Type*>(data);
        exit(EXIT_FAILURE);
    }
};

std::ostream& operator<<(std::ostream& stream, const JValue jvalue);
std::ostream& operator<<(std::ostream& stream, const JValue *jvalue);
std::ostream& operator<<(std::ostream& stream, const JType jtype);

} // namespace jltt
