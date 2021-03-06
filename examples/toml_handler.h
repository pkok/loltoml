#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wdocumentation-pedantic"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wweak-vtables"
#include "loltoml/parse.hpp"
#pragma clang diagnostic pop

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

#include "toml_value.h"

namespace ConfigReader {
  /**
   * Create a map of the key-value pairs in a "TOML+" configuration file.
   * 
   * [TOML](https://github.com/toml-lang/toml) is a minimal configuration file 
   * format.  Our specifications required that a key can be set to the value of 
   * another, previously defined, key:
   * 
   *    ```
   *    first = true
   *    second = first  # first = true,  second = true
   *    first = false   # first = false, second = true
   *    ```
   * 
   * Our parser is forked from [loltoml](https://github.com/andrusha97/loltoml)
   * at commit c2a2042.  Support for these "symbol values" has been implemented,
   * as well as handled in `TOMLHandler`.
   * 
   * *Note:* There is no full support for full TOML yet.  To see the supported
   * sections of the TOML specification, see the documentation of the 
   * `TOMLHandler`.
   * 
   * @seealso TOML specification: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md 
   * @seealso `TOMLHandler`
   * @param input An input stream that stores the TOML data.
   * @return A map of the keys in the TOML data, to their values.
   */
  inline std::map<std::string, TOMLValue> parse(std::istream& input);

  /** Create a map of the key-value pairs in a "TOML+" configuration file.
   * 
   * @param filename Name of the TOML file.
   * @return A map of the keys in the TOML data, to their values.
   */
  inline std::map<std::string, TOMLValue> parse(const std::string& filename);


  /** 
   * Error thrown when the `TOMLHandler` encounters unsupported TOML expressions.
   */
  class UnsupportedTOMLExpressionError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };


  /**
   * Stores parsed data of a TOML file in a `std::map<string, TOMLValue>`.
   * 
   * This class implements the interface required by the `loltoml` parser.  Each 
   * function is called on a parsing event.  It is required to deliver an 
   * implementation of each of these functions.  Not handling that event equals
   * providing an empty implementation.
   * 
   * The handler has only implemented a part of the TOML v0.4.0 specification.
   * The following sections of the TOML v0.4.0 specification are handled:
   *   - [Comment](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#comment)
   *   - [String](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#string)
   *   - [Integer](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#integer)
   *   - [Float](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#float)
   *   - [Boolean](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#boolean)
   *   
   * The following sections of the TOML v0.4.0 specification have not yet been 
   * implemented in the handler:
   *   - [Datetime](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#datetime), 
   *     including "Offset Date-Time", "Local Date-Time", "Local Date" and "Local 
   *     Time" - [see here](https://github.com/toml-lang/toml/blob/709e9e9ee91ba3c10f8613241b1497349803f4f4/README.md)
   *   - [Array](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#array)
   *   - [Table](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#table)
   *   - [Inline Table](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#inline-table)
   *   - [Array of Tables](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md#array-of-tables)
   * 
   * This class is used by `parseConfig` for `loltoml::parse`.
   */
  class TOMLHandler {
  public:
    /**
     * No initialisation on start of document needed.
     */
    void start_document() { }

    /**
     * No finalisation on end of document needed.
     */
    void finish_document() { }

    /**
     * Do not store comments.
     */
    void comment(const std::string&) { }

    /**
     * Unsupported yet!
     */
    void array_table(loltoml::key_iterator_t /*begin*/, loltoml::key_iterator_t /*end*/) __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("array table");
    }

    /**
     * Unsupported yet!
     */
    void table(loltoml::key_iterator_t /*begin*/, loltoml::key_iterator_t /*end*/) __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("table");
    }

    /**
     * Unsupported yet!
     */
    void start_array() __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("array");
    }

    /**
     * Unsupported yet!
     */
    void finish_array(std::size_t) __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("array");
    }

    /**
     * Unsupported yet!
     */
    void start_inline_table() __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("inline table");
    }

    /**
     * Unsupported yet!
     */
    void finish_inline_table(std::size_t) __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("inline table");
    }

    /**
     * Register the key for the next value insertion in the `std::map`.
     */
    void key(const std::string &key) {
      curr_key_ = key;
    }

    /**
     * Store `curr_key_` with this value with this type.
     */
    void boolean(bool value) {
      configs_[curr_key_] = value;
    }

    /**
     * Store `curr_key_` with this value with this type.
     */
    void string(const std::string& value) {
      configs_[curr_key_] = TOMLValue(value);
    }

    /**
     * Unsupported yet!
     */
    void datetime(const std::string& /*value*/) __attribute__((noreturn)) {
      throw UnsupportedTOMLExpressionError("datetime");
    }

    /**
     * Store `curr_key_` with this value with this type.
     */
    void integer(long long value) {
      configs_[curr_key_] = static_cast<std::int64_t>(value);
    }

    /**
     * Store `curr_key_` with this value with this type.
     */
    void floating_point(double value) {
      configs_[curr_key_] = value;
    }

    /**
     * Store `curr_key_` with this value with this type.
     */
    void symbol(const std::string &value) {
      if (configs_.find(value) == configs_.end()) {
        throw std::runtime_error("Identifier \"" + value
            + "\" on right hand side of =, but undefined.");
      }
      configs_.emplace(curr_key_, configs_.at(value));
    }

    /**
     * The storage for all configuration key-value pairs.
     */
    std::map<std::string, TOMLValue> configs_;

  private:
    /**
     * Store the next value under this key.
     */
    std::string curr_key_;
  };


  // This function might throw the following exceptions:
  //   - loltoml::parser_error_t if the stream does not represent a proper TOML
  //     file.
  //   - UnsupportedTOMLExpressionError if the TOMLHandler does not know how to
  //     handle a TOML expression, such as DateTime.
  //   - any std::exception that might be thrown when handling streams
  inline std::map<std::string, TOMLValue> parse(std::istream& input) {
    ConfigReader::TOMLHandler handler;
    loltoml::parse(input, handler);
    return handler.configs_;
  }

  inline std::map<std::string, TOMLValue> parse(const std::string& filename) {
    std::ifstream file(filename);
    return parse(file);
  }
}
