#include "lima/lima.hpp"
#include <juliett/impl/value.hpp>

#include <cstdlib>
#include <iostream>

namespace jltt {


JValue::JValue() : type(JType::NULL_VALUE), data(nullptr) {}

JValue::JValue(JType type) : type(type), data(nullptr) {
    switch (type) {
    case JType::OBJECT:
        data = new JObject();
        break;
    case JType::ARRAY:
        data = new JArray();
        break;
    case JType::STRING:
        data = new JString();
        break;
    case JType::NUMBER:
        data = new JNumber();
        break;
    case JType::BOOLEAN:
        data = new JBoolean();
        break;
    default:
        break;
    }
}

JValue::~JValue() {
    if (data == nullptr) return;
    switch (type) {
    case JType::OBJECT:
        for (const JPair pair: *static_cast<JObject*>(data))
            delete pair.second;
        delete static_cast<JObject*>(data);
        data = nullptr;
        break;
    case JType::ARRAY:
        for (const JValue* element: *static_cast<JArray*>(data))
            delete element;
        delete static_cast<JArray*>(data);
        data = nullptr;
        break;
    case JType::STRING:
        delete static_cast<JString*>(data);
        data = nullptr;
        break;
    case JType::NUMBER:
        delete static_cast<JNumber*>(data);
        data = nullptr;
        break;
    case JType::BOOLEAN:
        delete static_cast<JBoolean*>(data);
        data = nullptr;
        break;
    default:
        break;
    }
}

std::string JValue::tostr() const {
    switch (type) {
    case JType::OBJECT:
        return "JObject of length " + std::to_string(static_cast<JObject*>(data)->size());
    case JType::ARRAY:
        return "JArray of length " + std::to_string(static_cast<JArray*>(data)->size());
    case JType::STRING:
        return *static_cast<JString*>(data);
    case JType::NUMBER:
        return std::to_string(*static_cast<JNumber*>(data));
    case JType::BOOLEAN:
        return *static_cast<JBoolean*>(data) ? "true" : "false";
    case JType::NULL_VALUE:
        return "JNull";
    }
}

size_t JValue::length() const {
    switch (type) {
    case JType::OBJECT:
        return static_cast<JObject*>(data)->size();
    case JType::ARRAY:
        return static_cast<JArray*>(data)->size();
    case JType::STRING:
        return static_cast<JString*>(data)->size();
    default:
        LOG_THROW("Cannot size JValue of type '", type, "'.");
    }
    return 0; // unreachable
}

JValue* JValue::at(size_t index) {

    LOG_ASSERT(type == JType::ARRAY, "Cannot index JValue of type '", type, "' with 'size_t'.");
    LOG_ASSERT(static_cast<JArray*>(data)->size() > index, "Index out of range (", index, ").");

    return static_cast<JArray*>(data)->at(index);
}

JValue* JValue::at(JString key) {
    LOG_ASSERT(type == JType::OBJECT, "Cannot index JValue of type '", type, "' with 'std::string'.");
    if (!static_cast<JObject*>(data)->contains(key)) return nullptr;

    return static_cast<JObject*>(data)->at(key);
}

std::ostream& operator<<(std::ostream& stream, const JValue jvalue) {
    return stream << jvalue.tostr();
}

std::ostream& operator<<(std::ostream& stream, const JValue *jvalue) {
    return stream << jvalue->tostr();
}

std::ostream& operator<<(std::ostream& stream, const JType jtype) {
    switch (jtype) {
        case JType::OBJECT:
            return stream << "JObject";
        case JType::ARRAY:
            return stream << "JArray";
        case JType::STRING:
            return stream << "JString";
        case JType::NUMBER:
            return stream << "JNumber";
        case JType::BOOLEAN:
            return stream << "JBoolean";
        case JType::NULL_VALUE:
            return stream << "JNull";
    }
}

} // namespace jltt
