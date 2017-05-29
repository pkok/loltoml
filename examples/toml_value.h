#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace ConfigReader {
/**
 * Stores a single value of any of the following types:
 *
 * - bool
 * - std::string
 * - std::int64_t
 * - double
 *
 * This class accomplishes this by storing the data in a union, and storing
 * type information in the form of an enum value.
 *
 * The class provides constructors for each of the above mentioned types, any
 * signed integer type, and float.  Type conversion operators have been defined
 * to automatically cast the stored value to the requested type.  A
 * `std::domain_error` is raised when the stored type is not convertible to the
 * target type.
 *
 */
class TOMLValue {
public:
  /**
   * Represents the type stored in the `TOMLValue`'s union.
   */
  enum class Type : unsigned char {
    empty,
    boolean,
    string,
    // TODO: Uncomment when implementing support for TOML datetime values
    //datetime,
    integer,
    floating_point
  };

  /**
   * Retrieves the type that is currently stored.
   *
   * Depending on the return value of this function, you can extract the
   * stored value by casting to the corresponding type:
   *
   * - `Type::empty`: no cast available
   * - `Type::boolean`: cast to `bool`
   * - `Type::string`: cast to `std::string`
   * - `Type::integer`: cast to `std::int64_t`. Casting to `short`, `int`,
   *   `long`,`std::int8_t`, `std::int16_t`, `std::int32_t`, `float`, or
   *   `double` is accepted but beware for overflow errors.
   * - `Type::floating_point`: cast to `double`. Casting to `short`,
   *   `int`, `long`, `std::int8_t`, `std::int16_t`, `std::int32_t`,
   *   `std::int64_t`, or `float` is accepted but beware for overflow
   *   errors and rounding.
   */
  Type type() const noexcept {
    return type_;
  }

  /**
   * Creates a new TOMLValue without any value stored.
   */
  TOMLValue()
    : type_(Type::empty), as_bool_(false)
  {}

  /**
   * Creates a new TOMLValue that stores a `bool`.
   * 
   * @param value The value to store.
   */
  TOMLValue(bool value)
    : type_(Type::boolean), as_bool_(value)
  {}

  /**
   * Creates a new TOMLValue that stores a 64-bit integer.
   * 
   * @param value The value to store.
   */
  TOMLValue(std::int8_t value)
    : type_(Type::integer), as_integer_(static_cast<std::int64_t>(value))
  {}

  /**
   * Creates a new TOMLValue that stores a 64-bit integer.
   * 
   * @param value The value to store.
   */
  TOMLValue(std::int16_t value)
    : type_(Type::integer), as_integer_(static_cast<std::int64_t>(value))
  {}

  /**
   * Creates a new TOMLValue that stores a 64-bit integer.
   * 
   * @param value The value to store.
   */
  TOMLValue(std::int32_t value)
    : type_(Type::integer), as_integer_(static_cast<std::int64_t>(value))
  {}

  /**
   * Creates a new TOMLValue that stores a 64-bit integer.
   * 
   * @param value The value to store.
   */
  TOMLValue(std::int64_t value)
    : type_(Type::integer), as_integer_(value)
  {}

  /**
   * Creates a new TOMLValue that stores a `double`.
   * 
   * @param value The value to store.
   */
  TOMLValue(float value)
    : type_(Type::floating_point), as_floating_point_(double(value))
  {}

  /**
   * Creates a new TOMLValue that stores a `double`.
   * 
   * @param value The value to store.
   */
  TOMLValue(double value)
    : type_(Type::floating_point), as_floating_point_(value)
  {}

  /**
   * Creates a new TOMLValue that stores a `std::string`.
   * 
   * @param value The value to store.
   */
  TOMLValue(const std::string& value)
    : type_(Type::string), as_string_(value)
  {}

  /**
   * Type conversion operator to `bool`.
   *
   * If `type_` does not equal `TOMLValue::Type::boolean`, this will
   * raise a `std::domain_error`.
   */
  operator bool() const {
    if (type_ != Type::boolean) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to bool");
    }
    return as_bool_;
  }

  /**
   * Type conversion operator to `std::int8_t`.
   *
   * `std::int8_t` is commonly implemented as the same type as
   * `signed char`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of overflow errors and, if
   * `type_ == TOMLValue::Type::floating_point`, rounding.
   */
  operator std::int8_t() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to std::int8_t");
    }
    if (type_ == Type::floating_point) {
      return static_cast<std::int8_t>(as_floating_point_);
    }
    return static_cast<std::int8_t>(as_integer_);
  }

  /**
   * Type conversion operator to `std::int16_t`.
   *
   * `std::int16_t` is commonly implemented as the same type as `short`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of overflow errors and, if
   * `type_ == TOMLValue::Type::floating_point`, rounding.
   */
  operator std::int16_t() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to std::int16_t");
    }
    if (type_ == Type::floating_point) {
      return static_cast<std::int16_t>(as_floating_point_);
    }
    return static_cast<std::int16_t>(as_integer_);
  }

  /**
   * Type conversion operator to `std::int32_t`.
   *
   * `std::int32_t` is commonly implemented as the same type as `int`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of overflow errors and, if
   * `type_ == TOMLValue::Type::floating_point`, rounding.
   */
  operator std::int32_t() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to std::int32_t");
    }
    if (type_ == Type::floating_point) {
      return static_cast<std::int32_t>(as_floating_point_);
    }
    return static_cast<std::int32_t>(as_integer_);
  }

  /**
   * Type conversion operator to `std::int64_t`.
   *
   * `std::int64_t` is commonly implemented as the same type as `long`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of rounding if
   * `type_ == TOMLValue::Type::floating_point`.
   */
  operator std::int64_t() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to std::int64_t");
    }
    if (type_ == Type::floating_point) {
      return static_cast<std::int64_t>(as_floating_point_);
    }
    return as_integer_;
  }

  /**
   * Type conversion operator to `float`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of overflow errors and, if
   * `type_ == TOMLValue::Type::floating_point`, rounding.
   */
  operator float() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to float");
    }
    if (type_ == Type::integer) {
      return static_cast<float>(as_integer_);
    }
    return static_cast<float>(as_floating_point_);
  }

  /**
   * Type conversion operator to `double`.
   *
   * This will succeed if `type_` equals either
   * `TOMLValue::Type::integer` or `TOMLValue::Type::floating_point`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   *
   * Beware of overflow errors if
   * `type_ == TOMLValue::Type::integer`.
   */
  operator double() const {
    if (type_ != Type::integer && type_ != Type::floating_point) {
      throw std::domain_error("A TOMLValue with type_ == " + type_string() + " cannot be cast to double");
    }
    if (type_ == Type::integer) {
      return static_cast<double>(as_integer_);
    }
    return as_floating_point_;
  }

  /**
   * Type conversion operator to `std::string`.
   *
   * This will succeed if `type_` equals `TOMLValue::Type::string`.
   * Otherwise, this function raises an exception of type
   * `std::domain_error`.
   */
  operator std::string() const {
    if (type_ != Type::string) {
      throw std::domain_error("This TOMLValue does not contain a string object.");
    }
    return as_string_;
  }

  /** 
   * Copy constructor.
   */
  TOMLValue(const TOMLValue& other)
    : type_(other.type_)
  {
    switch (other.type_) {
      case Type::empty:
        break;
      case Type::boolean:
        as_bool_ = other.as_bool_;
        break;
      case Type::string:
        // Explicit placement new required because
        // as_string_ is part of a union and has a user-defined
        // constructor or destructor.
        new (&as_string_) std::basic_string<char>;
        as_string_ = other.as_string_;
        break;
      // TODO: Uncomment when implementing support for TOML datetime values
      //case Type::datetime:
      //    new (&as_datetime_) datetime_t();
      //    as_datetime_ = other.as_datetime_;
      //    break;
      case Type::integer:
        as_integer_ = other.as_integer_;
        break;
      case Type::floating_point:
        as_floating_point_ = other.as_floating_point_;
        break;
    }
  }

  /**
   * Assignment operator.
   *
   * Because different single-argument constructors are provided, only a
   * single assignment operator is needed.
   *
   * Expressions like `Value v = 3.f;` will expand to
   * `Value v = Value(3.f);`, on which this assignment operator can
   * operate.
   * 
   * @return A reference to the new value of *this.
   */
  TOMLValue& operator=(const TOMLValue& other) {
    // If a union member with a user-defined constructor or destructor
    // is deactivated, it needs to be explicitly destructed.
    this->~TOMLValue();
    switch (other.type_) {
      case Type::empty:
        break;
      case Type::boolean:
        as_bool_ = other.as_bool_;
        break;
      case Type::string:
        // Explicit placement new required because
        // as_string_ is part of a union and has a user-defined
        // constructor or destructor.
        new (&as_string_) std::basic_string<char>;
        as_string_ = other.as_string_;
        break;
      // TODO: Uncomment when implementing support for TOML datetime values
      //case Type::datetime:
      //    new (&as_datetime_) datetime_t();
      //    as_datetime_ = other.as_datetime_;
      //    break;
      case Type::integer:
        as_integer_ = other.as_integer_;
        break;
      case Type::floating_point:
        as_floating_point_ = other.as_floating_point_;
        break;
    }
    type_ = other.type_;
    return *this;
  }

  /**
   * Non-default destructor, because of the union.
   *
   * When a union stores values of types with user-defined constructors
   * and/or destructors, "explicit destructor and placement new are
   * generally required."
   *
   * See (this article)[http://en.cppreference.com/w/cpp/language/union].
   */
  ~TOMLValue() {
    switch (type_) {
      case Type::empty:
      case Type::boolean:
      case Type::integer:
      case Type::floating_point:
        break;
      case Type::string:
        as_string_.~basic_string<char>();
        break;
      // TODO: Uncomment when implementing support for TOML datetime values
      //case Type::datetime:
      //    as_datetime_.~datetime_t();
      //    break;
    }
  }

private:
  /**
   * Stores the type information of the active member of the union.
   */
  Type type_;

  /**
   * Stores the data.
   *
   * Type information is found in `type_`.
   *
   * Access to these values is granted by casting to the correct type.
   */
  union {
    bool as_bool_;
    std::int64_t as_integer_;
    double as_floating_point_;
    std::string as_string_;
    // TODO: implement as_datetime_ with a std::chrono::time_point
    //datetime_t as_datetime_;
  };

  std::string type_string() const {
    switch (type_) {
      case TOMLValue::Type::empty:
        return "TOMLValue::Type::empty";
      case TOMLValue::Type::boolean:
        return "TOMLValue::Type::boolean";
      case TOMLValue::Type::string:
        return "TOMLValue::Type::string";
        // TODO: Uncomment when implementing support for TOML datetime values
        //case TOMLValue::Type::datetime:
        //  return "TOMLValue::Type::datetime";
      case TOMLValue::Type::integer:
        return "TOMLValue::Type::integer";
      case TOMLValue::Type::floating_point:
        return "TOMLValue::Type::floating_point";
    }
  }
};


/**
 * Transform a string to a serializable format.
 *
 * This will escape special characters, like newline.
 * 
 * Copied from the original loltoml project.
 *
 * @param text The text without escaped special characters.
 * @result The text with escaped special characters.
 */
inline std::string escape_string(const std::string& text) {
  const char *hex_digits = "0123456789abcdef";
  std::string result;

  for (char ch : text) {

    if (ch == '\\') {
      result += "\\\\";
    } else if (ch == '\"') {
      result += "\\\"";
    } else if (ch == '\b') {
      result += "\\b";
    } else if (ch == '\t') {
      result += "\\t";
    } else if (ch == '\r') {
      result += "\\r";
    } else if (ch == '\n') {
      result += "\\n";
    } else if (static_cast<unsigned char>(ch) < 32) {
      result.push_back('\\');
      result.push_back('u');
      result.push_back('0');
      result.push_back('0');
      result.push_back(hex_digits[static_cast<unsigned char>(ch) / 16]);
      result.push_back(hex_digits[static_cast<unsigned char>(ch) % 16]);
    } else {
      result.push_back(ch);
    }
  }

  return result;
}
}


namespace std {
  /**
   * Easy string convert function for `TOMLValue`.
   * 
   * @param value The TOMLValue of which a string representation is requested.
   * @return The string representation.
   */
  inline std::string to_string(const ConfigReader::TOMLValue& value) {
    switch (value.type()) {
      case ConfigReader::TOMLValue::Type::empty:
        return "void";
      case ConfigReader::TOMLValue::Type::boolean:
        return to_string(bool(value));
      case ConfigReader::TOMLValue::Type::string:
        return std::string(value);
      // TODO: Uncomment when implementing support for TOML datetime values
      //case ConfigReader::TOMLValue::Type::datetime:
      //    return value.as_datetime_;
      case ConfigReader::TOMLValue::Type::integer:
        return to_string(std::int64_t(value));
      case ConfigReader::TOMLValue::Type::floating_point:
        return to_string(double(value));
    }
  }
}


/**
 * Stream operator for `TOMLValue`s.
 * 
 * @param stream The stream to write the value to.
 * @param value The TOMLValue to write.
 * @return The same stream as was provided as argument.
 */
inline std::ostream& operator<<(std::ostream& stream, ConfigReader::TOMLValue value) noexcept {
  switch (value.type()) {
    case ConfigReader::TOMLValue::Type::empty:
      return stream << "void";
    case ConfigReader::TOMLValue::Type::boolean:
      return stream << bool(value);
    case ConfigReader::TOMLValue::Type::string:
      return stream << "\"" << escape_string(value) << "\"";
    // TODO: Uncomment when implementing support for TOML datetime values
    //case TOMLValue::Type::datetime:
    //    return stream << value.as_datetime_;
    case ConfigReader::TOMLValue::Type::integer:
      return stream << std::int64_t(value);
    case ConfigReader::TOMLValue::Type::floating_point:
      return stream << double(value);
  }
}
