#ifndef LOLTOML_DETAIL_PARSER_HPP
#define LOLTOML_DETAIL_PARSER_HPP

#include "loltoml/detail/common.hpp"
#include "loltoml/detail/input_stream.hpp"
#include "loltoml/error.hpp"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <string>
#include <vector>

LOLTOML_OPEN_NAMESPACE

namespace detail {


inline std::string escape_char(char ch) {
    if (ch == '\\') {
        return "\\\\";
    } else if (ch == '\'') {
        return "\\\'";
    } else if (ch == '\"') {
        return "\\\"";
    } else if (ch == '\b') {
        return "\\b";
    } else if (ch == '\t') {
        return "\\t";
    } else if (ch == '\r') {
        return "\\r";
    } else if (ch == '\n') {
        return "\\n";
    } else if (std::isprint(ch)) {
        return std::string(1, ch);
    } else {
        const char *hex_digits = "0123456789abcdef";
        char result[5] = "\\xYY";

        result[2] = hex_digits[static_cast<unsigned char>(ch) / 16];
        result[3] = hex_digits[static_cast<unsigned char>(ch) % 16];

        return result;
    }
}


inline bool iscontrol(char ch) {
    return static_cast<unsigned char>(ch) < 32;
}


inline bool is_word_character(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '+' ||
           ch == '-' ||
           ch == '_' ||
           ch == '.';
}


inline bool is_key_character(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '-' ||
           ch == '_';
}


typedef std::vector<std::string>::const_iterator key_iterator_t;


template<class Handler>
class parser_t {
    input_stream_t input;
    Handler &handler;

public:
    parser_t(std::istream &input, Handler &handler) :
        input(input),
        handler(handler)
    { }

    void parse() {
        handler.start_document();

        parse_expression();

        while (!input.eof()) {
            parse_new_line();
            parse_expression();
        }

        handler.finish_document();
    }

private:
    enum class toml_type_t {
        string,
        integer,
        floating_point,
        boolean,
        datetime,
        array,
        table,
        symbol,
    };

    std::size_t last_char_offset() const {
        std::size_t processed = input.processed();
        return (processed == 0) ? 0 : (processed - 1);
    }

    template<std::size_t N>
    char parse_chars(const char (&expected)[N]) {
        static_assert(N > 0, "No expected characters specified");
        assert(expected[N - 1] == '\0');

        char result = input.get();
        for (std::size_t i = 0; i + 1 < N; ++i) {
            if (result == expected[i]) {
                return result;
            }
        }

        std::string list = "\'" + escape_char(expected[0]) + "\'";

        for (std::size_t i = 1; i + 1 < N; ++i) {
            list += ", \'" + escape_char(expected[i]) + "\'";
        }

        throw parser_error_t("Expected one of the following symbols: " + list, last_char_offset());
    }

    void skip_spaces() {
        while (!input.eof() && (input.peek() == ' ' || input.peek() == '\t')) {
            input.get();
        }
    }

    void parse_comment() {
        assert(input.peek() == '#');
        input.get();

        std::string comment;
        while (input.peek() == '\t' || !iscontrol(input.peek())) {
            comment.push_back(input.get());
        }

        handler.comment(comment);
    }

    void parse_new_line() {
        char ch = input.get();

        if (ch == '\r') {
            ch = input.get();
        }

        if (ch != '\n') {
            throw parser_error_t("Expected new-line", last_char_offset());
        }
    }

    void skip_spaces_and_empty_lines() {
        while (!input.eof()) {
            skip_spaces();

            if (input.peek() == '#') {
                parse_comment();
                parse_new_line();
            } else if (input.peek() == '\r' || input.peek() == '\n') {
                parse_new_line();
            } else {
                break;
            }
        }
    }

    void parse_expression() {
        skip_spaces();

        if (input.eof()) {
            return;
        } else if (input.peek() == '\r' || input.peek() == '\n') {
            return;
        } else if (input.peek() == '#') {
            parse_comment();
        } else if (input.peek() == '[') {
            parse_table_header();
            skip_spaces();
            if (!input.eof() && input.peek() == '#') {
                parse_comment();
            }
        } else {
            parse_kv_pair();
            skip_spaces();
            if (!input.eof() && input.peek() == '#') {
                parse_comment();
            }
        }
    }

    void parse_table_header() {
        assert(input.peek() == '[');
        input.get();

        bool array_item = false;

        if (input.peek() == '[') {
            input.get();
            array_item = true;
        }

        std::vector<std::string> path;
        while (true) {
            skip_spaces();

            path.emplace_back(parse_key());

            skip_spaces();

            if (input.peek() == ']') {
                input.get();

                if (array_item) {
                    parse_chars("]");
                }

                break;
            } else {
                parse_chars(".");
            }
        }

        if (array_item) {
            handler.array_table(path.cbegin(), path.cend());
        } else {
            handler.table(path.cbegin(), path.cend());
        }
    }

    void parse_kv_pair() {
        handler.key(parse_key());
        skip_spaces();
        parse_chars("=");
        skip_spaces();
        parse_value();
    }

    std::string parse_key() {
        std::string key;
        if (input.peek() == '"') {
            input.get();
            key = parse_basic_string();

            if (key.empty()) {
                throw parser_error_t("Expected a non-empty key", last_char_offset());
            }
        } else {
            // It must be at least one char.
            char ch = input.get();
            if (!is_key_character(ch)) {
                throw parser_error_t("Expected a non-empty key", last_char_offset());
            }

            key.push_back(ch);

            while (is_key_character(input.peek())) {
                key.push_back(input.get());
            }
        }

        return key;
    }

    toml_type_t parse_value() {
        switch (input.peek()) {
            case '{': {
                parse_inline_table();
                return toml_type_t::table;
            } break;
            case '[': {
                parse_array();
                return toml_type_t::array;
            } break;
            case '"': {
                parse_string();
                return toml_type_t::string;
            } break;
            case '\'': {
                parse_literal_string();
                return toml_type_t::string;
            } break;
            default: {
                return parse_bool_or_number_or_symbol();
            }
        }
    }

    void parse_array() {
        assert(input.peek() == '[');
        input.get();
        handler.start_array();
        skip_spaces_and_empty_lines();

        toml_type_t array_type;
        std::size_t size = 0;

        while (true) {
            if (input.peek() == ']') {
                input.get();
                handler.finish_array(size);
                return;
            }

            std::size_t item_offset = input.processed();
            toml_type_t current_item_type = parse_value();

            if (size > 0 && current_item_type != array_type) {
                throw parser_error_t("All array elements must be of the same type", item_offset);
            }

            ++size;
            array_type = current_item_type;

            // FIXME: Formal grammar from https://github.com/toml-lang/toml/pull/236 disallows new-lines between values and commas.
            skip_spaces_and_empty_lines();

            char ch = input.get();
            if (ch == ']') {
                handler.finish_array(size);
                return;
            } else if (ch == ',') {
                skip_spaces_and_empty_lines();
            } else {
                throw parser_error_t("Expected ',' or ']' after an array element", last_char_offset());
            }
        }
    }

    void parse_inline_table() {
        assert(input.peek() == '{');
        input.get();
        handler.start_inline_table();
        std::size_t size = 0;

        skip_spaces();

        if (input.peek() == '}') {
            input.get();
            handler.finish_inline_table(size);
            return;
        }

        while (true) {
            handler.key(parse_key());
            skip_spaces();
            parse_chars("=");
            skip_spaces();
            parse_value();
            skip_spaces();

            ++size;

            char ch = input.get();
            if (ch == '}') {
                handler.finish_inline_table(size);
                return;
            } else if (ch == ',') {
                skip_spaces();
            } else {
                throw parser_error_t("Expected ',' or '}' after an inline table element", last_char_offset());
            }
        }
    }

    unsigned int parse_hex_digit() {
        char ch = input.get();

        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            return ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
            return ch - 'a' + 10;
        } else {
            throw parser_error_t("Expected hex-digit", last_char_offset());
        }
    }

    uint32_t parse_4_digit_codepoint() {
        uint32_t codepoint = parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();

        return codepoint;
    }

    uint32_t parse_8_digit_codepoint() {
        uint32_t codepoint = parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();

        return codepoint;
    }

    void process_codepoint(uint32_t codepoint, std::size_t escape_sequence_offset, std::string &output) {
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            throw parser_error_t("Surrogate pairs are not allowed", escape_sequence_offset);
        }

        if (codepoint > 0x10FFFF) {
            throw parser_error_t("Codepoint must be less or equal than 0x10FFFF", escape_sequence_offset);
        }

        if (codepoint <= 0x7F) {
            output.push_back(static_cast<unsigned char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            output.push_back(static_cast<unsigned char>(0xC0 | (codepoint >> 6)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            output.push_back(static_cast<unsigned char>(0xE0 | (codepoint >> 12)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        } else {
            output.push_back(static_cast<unsigned char>(0xF0 | (codepoint >> 18)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 12) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        }
    }

    std::string parse_basic_string() {
        std::string result;

        while (true) {
            char ch = input.get();
            if (iscontrol(ch)) {
                throw parser_error_t("Control characters must be escaped", last_char_offset());
            } else if (ch == '"') {
                break;
            } else if (ch == '\\') {
                std::size_t escape_sequence_offset = last_char_offset();
                char ch = input.get();
                if (ch == 'b') {
                    result.push_back('\b');
                } else if (ch == 't') {
                    result.push_back('\t');
                } else if (ch == 'n') {
                    result.push_back('\n');
                } else if (ch == 'f') {
                    result.push_back('\f');
                } else if (ch == 'r') {
                    result.push_back('\r');
                } else if (ch == '"') {
                    result.push_back('"');
                } else if (ch == '\\') {
                    result.push_back('\\');
                } else if (ch == 'u') {
                    process_codepoint(parse_4_digit_codepoint(), escape_sequence_offset, result);
                } else if (ch == 'U') {
                    process_codepoint(parse_8_digit_codepoint(), escape_sequence_offset, result);
                } else {
                    throw parser_error_t("Invalid escape-sequence", escape_sequence_offset);
                }
            } else {
                result.push_back(ch);
            }
        }

        return result;
    }

    std::string parse_multiline_string() {
        std::string result;

        // Ignore first new-line after open quotes.
        if (input.peek() == '\r' || input.peek() == '\n') {
            parse_new_line();
        }

        while (true) {
            if (input.peek() == '\r' || input.peek() == '\n') {
                parse_new_line();
                result.push_back('\n');
                continue;
            }

            char ch = input.get();
            if (iscontrol(ch)) {
                throw parser_error_t("Control characters must be escaped", last_char_offset());
            } else if (ch == '"') {
                if (input.peek() == '"') {
                    input.get();
                    if (input.peek() == '"') {
                        input.get();
                        break;
                    }
                    result.push_back('"');
                }
                result.push_back('"');
            } else if (ch == '\\') {
                if (input.peek() == '\r' || input.peek() == '\n') {
                    parse_new_line();
                    while (std::isspace(input.peek())) {
                        input.get();
                    }
                    continue;
                }

                std::size_t escape_sequence_offset = last_char_offset();
                char ch = input.get();
                if (ch == 'b') {
                    result.push_back('\b');
                } else if (ch == 't') {
                    result.push_back('\t');
                } else if (ch == 'n') {
                    result.push_back('\n');
                } else if (ch == 'f') {
                    result.push_back('\f');
                } else if (ch == 'r') {
                    result.push_back('\r');
                } else if (ch == '"') {
                    result.push_back('"');
                } else if (ch == '\\') {
                    result.push_back('\\');
                } else if (ch == 'u') {
                    process_codepoint(parse_4_digit_codepoint(), escape_sequence_offset, result);
                } else if (ch == 'U') {
                    process_codepoint(parse_8_digit_codepoint(), escape_sequence_offset, result);
                } else {
                    throw parser_error_t("Invalid escape-sequence", escape_sequence_offset);
                }
            } else {
                result.push_back(ch);
            }
        }

        return result;
    }

    void parse_string() {
        assert(input.peek() == '"');
        input.get();

        if (input.peek() == '"') {
            input.get();

            if (input.peek() == '"') {
                input.get();
                handler.string(parse_multiline_string());
            } else {
                handler.string("");
            }
        } else {
            handler.string(parse_basic_string());
        }
    }

    void parse_literal_string() {
        assert(input.peek() == '\'');
        input.get();

        if (input.peek() == '\'') {
            input.get();

            if (input.peek() == '\'') {
                input.get();

                // Ignore first new-line after open quotes.
                if (input.peek() == '\r' || input.peek() == '\n') {
                    parse_new_line();
                }

                std::string string;
                while (true) {
                    if (input.peek() == '\r' || input.peek() == '\n') {
                        parse_new_line();
                        string.push_back('\n');
                        continue;
                    }

                    char ch = input.get();
                    if (ch == '\'') {
                        if (input.peek() == '\'') {
                            input.get();
                            if (input.peek() == '\'') {
                                input.get();
                                handler.string(string);
                                return;
                            }
                            string.push_back('\'');
                        }
                        string.push_back('\'');
                    } else if (iscontrol(ch) && ch != '\t') {
                        throw parser_error_t("Control characters are not allowed", last_char_offset());
                    } else {
                        string.push_back(ch);
                    }
                }
            } else {
                handler.string("");
            }
        } else {
            std::string string;

            while (true) {
                char ch = input.get();
                if (iscontrol(ch) && ch != '\t') {
                    throw parser_error_t("Control characters are not allowed", last_char_offset());
                } else if (ch == '\'') {
                    break;
                }

                string.push_back(ch);
            }

            handler.string(string);
        }
    }

    toml_type_t parse_bool_or_number_or_symbol() {
        std::string result;
        char ch = input.get();
        if (!is_word_character(ch)) {
            throw parser_error_t("Expected a non-empty symbol", last_char_offset());
        }

        result.push_back(ch);

        while (is_word_character(input.peek())) {
            result.push_back(input.get());
        }

        if (result == "true") {
            handler.boolean(true);
            return toml_type_t::boolean;
        }
        else if (result == "false") {
            handler.boolean(false);
            return toml_type_t::boolean;
        }
        else if (std::regex_match(result, std::regex("^\\d+$"))) {
            handler.integer(std::stoll(result));
            return toml_type_t::integer;
        }
        //else if (std::regex_match(result, std::regex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"))) {
        else if (std::regex_match(result, std::regex("^[-+]?((\\d*.\\d+)|(\\d+.\\d*)|\\d+)([eE][-+]?\\d+)?$"))) {
            handler.floating_point(std::stod(result));
            return toml_type_t::floating_point;
        }
        else if (std::regex_match(result, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
            handler.symbol(result);
            return toml_type_t::symbol;
        }
        throw parser_error_t("Invalid value", 0);
    }
};


} // namespace detail

LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_DETAIL_PARSER_HPP
