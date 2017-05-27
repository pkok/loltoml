#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wdocumentation-pedantic"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wweak-vtables"
#include <loltoml/parse.hpp>
#pragma clang diagnostic pop

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "toml_value.hpp"

template <typename X, typename Y>
inline std::ostream& operator<<(std::ostream& stream, std::map<X, Y> map) noexcept {
    stream << "{ ";
    bool first = true;
    for (auto i : map) {
        X x = i.first;
        Y y = i.second;
        if (!first) {
            stream << ", ";
        }
        stream << x << " = " << y;
        first = false;
    }
    return stream << " }";
}


class unsupported_expression_error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};


struct ConfigHandler {
    // The Handler interface implementation.
    void start_document() { }

    void finish_document() { }

    // Ignore comments.
    void comment(const std::string &) { }

    void array_table(loltoml::key_iterator_t /*begin*/, loltoml::key_iterator_t /*end*/) __attribute__((noreturn)) {
        throw unsupported_expression_error("array table");
    }

    void table(loltoml::key_iterator_t /*begin*/, loltoml::key_iterator_t /*end*/) __attribute__((noreturn)) {
        throw unsupported_expression_error("table");
    }

    void start_array() __attribute__((noreturn)) {
        throw unsupported_expression_error("array");
    }

    void finish_array(std::size_t) __attribute__((noreturn)) {
        throw unsupported_expression_error("array");
    }

    void start_inline_table() __attribute__((noreturn)) {
        throw unsupported_expression_error("inline table");
    }

    void finish_inline_table(std::size_t) __attribute__((noreturn)) {
        throw unsupported_expression_error("inline table");
    }

    void key(const std::string &key) {
        curr_key_ = key;
    }

    void boolean(bool value) {
        configs_[curr_key_] = value;
    }

    void string(const std::string &value) {
        configs_[curr_key_] = TOMLValue(value);
    }

    void datetime(const std::string &/*value*/) __attribute__((noreturn)) {
        throw unsupported_expression_error("datetime");
        //configs_[curr_key_] = TOMLValue(value_type::datetime, value);
    }

    void integer(long long value) {
        configs_[curr_key_] = static_cast<std::int64_t>(value);
    }

    void floating_point(double value) {
        configs_[curr_key_] = value;
    }

    void symbol(const std::string &value) {
        if (configs_.find(value) == configs_.end()) {
            throw std::runtime_error("Identifier \"" + value
                    + "\" on right hand side of =, but undefined.");
        }
        configs_.emplace(curr_key_, configs_.at(value));
    }

    std::map<std::string, TOMLValue> configs_;

    private:
        std::string curr_key_;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "How to call:\n"
            << "\t" << argv[0] << " toml_file"
            << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream f(argv[1]);
    ConfigHandler handler;
    try {
        loltoml::parse(f, handler);
        std::cout << std::boolalpha;
        std::cout << handler.configs_ << std::endl;
    }
    catch (const loltoml::parser_error_t& e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
