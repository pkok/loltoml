#include <iostream>

#include "toml_handler.h"


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


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "How to call:\n"
            << "\t" << argv[0] << " toml_file"
            << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream f(argv[1]);
    try {
        std::map<std::string, ConfigReader::TOMLValue> configs = ConfigReader::parse(f);
        std::cout << std::boolalpha;
        std::cout << configs << std::endl;
    }
    catch (const loltoml::parser_error_t& e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
