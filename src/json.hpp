#pragma once

#include <unordered_map>
#include <vector>

#include "external_construct.hpp"
#include "meta_helper.hpp"
#include "value_type.h"

namespace otter {
template <typename T>
bool is_lvalue(T&) {
  return true;
}
template <typename T>
bool is_lvalue(T&&) {
  return false;
}

class json {
 public:
  template <typename T, typename... Args>
  static T* create(Args&&... args) {
    T* ptr = new T(std::forward<Args>(args)...);
    return ptr;
  }
  union json_value;
  using json_object = std::unordered_map<std::string, json_value>;
  using json_array = std::vector<json_value>;
  using json_string = std::string;
  using json_int = int64_t;
  using json_float = float;
  using json_bool = bool;
  union json_value {
    /* data */
    json_object* obj;
    json_array* arr;
    json_string* str;
    json_int num_int;
    json_float num_float;
    json_bool boolean;

    // default constructor (for null values)
    json_value() { obj = nullptr; }
    json_value(json_int val) { num_int = val; }
    json_value(json_float val) { num_float = val; }
    json_value(json_bool val) { boolean = val; }
    json_value(const json_object& val) : obj(create<json_object>(val)) {}
    json_value(json_object&& val) : obj(create<json_object>(std::move(val))) {}
    json_value(const json_array& val) : arr(create<json_array>(val)) {}
    json_value(json_array&& val) : arr(create<json_array>(std::move(val))) {}
    json_value(const json_string& val) : str(create<json_string>(val)) {}
    json_value(json_string&& val) : str(create<json_string>(std::move(val))) {}

    void release(json_type t) {
      switch (t) {
        case json_type::JSON_OBJECT:
          /* TODO(creepy-otter): a temporary delete for json_object,
           * do this recursively or flatten the object */
          if (obj != nullptr) delete obj;
          break;
        case json_type::JSON_ARRAY:
          if (arr != nullptr) delete arr;
          break;
        case json_type::JSON_STRING:
          if (str != nullptr) delete str;
        default:
          break;
      }
    }
  };

  /* data */
  json_value value_;
  json_type type_;

 public:
  json() : type_{json_type::JSON_NULL} {};
  json(const json& val) : type_{val.type_} {
    switch (type_) {
      case json_type::JSON_OBJECT:
        value_ = *val.value_.obj;
        break;
      case json_type::JSON_ARRAY:
        value_ = *val.value_.arr;
        break;
      case json_type::JSON_STRING:
        value_ = *val.value_.str;
        break;
      case json_type::JSON_NUMBER_INT:
        value_ = val.value_.num_int;
        break;
      case json_type::JSON_NUMBER_FLOAT:
        value_ = val.value_.num_float;
        break;
      case json_type::JSON_BOOLEAN:
        value_ = val.value_.boolean;
        break;
      case json_type::JSON_NULL:
        value_.obj = nullptr;
        break;
      default:
        break;
    }
  }

  ~json() { value_.release(type_); }

  template <typename CompatibleType>
  json(CompatibleType&& val) {
    internal::init_json(*this, std::forward<CompatibleType>(val));
  }

  // template <typename CompatibleType, typename std::enalbe_if_t<, int> = 0>
  // void construct_value(CompatibleType&& val) {}

  void debug_print_str() { std::cout << *value_.str << std::endl; }
  void debug_print_int() { std::cout << value_.num_int << std::endl; }
};

}  // namespace otter
