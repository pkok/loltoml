#include "common.hpp"

#include <fstream>

TEST(ValidParse, ComplexOne) {
    std::ifstream input(TESTS_ROOT "documents/complex.toml");
    comments_skipper_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},

        {sax_event_t::key, "key1"},
        {sax_event_t::integer, 1323},
        {sax_event_t::key, "228"},
        {sax_event_t::integer, 228},

        {sax_event_t::table, {"table"}},
        {sax_event_t::key, "key"},
        {sax_event_t::string, "value"},

        {sax_event_t::table, {"table", "subtable"}},
        {sax_event_t::key, "key"},
        {sax_event_t::string, "another value"},

        {sax_event_t::table, {"x", "y", "z", "w"}},

        {sax_event_t::table, {"table", "inline"}},
        {sax_event_t::key, "name"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "first"},
        {sax_event_t::string, "Tom"},
        {sax_event_t::key, "last"},
        {sax_event_t::string, "Preston-Werner"},
        {sax_event_t::finish_inline_table, 2},
        {sax_event_t::key, "point"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "x"},
        {sax_event_t::integer, 1},
        {sax_event_t::key, "y"},
        {sax_event_t::integer, 2},
        {sax_event_t::finish_inline_table, 2},

        {sax_event_t::table, {"string", "basic"}},
        {sax_event_t::key, "basic"},
        {sax_event_t::string, "I'm a string. \"You can quote me\". Name\tJosé\nLocation\tSF."},

        {sax_event_t::table, {"string", "multiline"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::string, "One\nTwo"},
        {sax_event_t::key, "key2"},
        {sax_event_t::string, "One\nTwo"},
        {sax_event_t::key, "key3"},
        {sax_event_t::string, "One\nTwo"},

        {sax_event_t::table, {"string", "multiline", "continued"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::string, "The quick brown fox jumps over the lazy dog."},
        {sax_event_t::key, "key2"},
        {sax_event_t::string, "The quick brown fox jumps over the lazy dog."},
        {sax_event_t::key, "key3"},
        {sax_event_t::string, "The quick brown fox jumps over the lazy dog."},

        {sax_event_t::table, {"string", "literal"}},
        {sax_event_t::key, "winpath"},
        {sax_event_t::string, "C:\\Users\\nodejs\\templates"},
        {sax_event_t::key, "winpath2"},
        {sax_event_t::string, "\\\\ServerX\\admin$\\system32\\"},
        {sax_event_t::key, "quoted"},
        {sax_event_t::string, "Tom \"Dubs\" Preston-Werner"},
        {sax_event_t::key, "regex"},
        {sax_event_t::string, "<\\i\\c*\\s*>"},

        {sax_event_t::table, {"string", "literal", "multiline"}},
        {sax_event_t::key, "regex2"},
        {sax_event_t::string, "I [dw]on't need \\d{2} apples"},
        {sax_event_t::key, "lines"},
        {sax_event_t::string, "The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n"},

        {sax_event_t::table, {"integer"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::integer, 99},
        {sax_event_t::key, "key2"},
        {sax_event_t::integer, 42},
        {sax_event_t::key, "key3"},
        {sax_event_t::integer, 0},
        {sax_event_t::key, "key4"},
        {sax_event_t::integer, -17},

        {sax_event_t::table, {"integer", "underscores"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::integer, 1000},
        {sax_event_t::key, "key2"},
        {sax_event_t::integer, 5349221},
        {sax_event_t::key, "key3"},
        {sax_event_t::integer, 12345},

        {sax_event_t::table, {"float", "fractional"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::floating_point, 1.0},
        {sax_event_t::key, "key2"},
        {sax_event_t::floating_point, 3.1415},
        {sax_event_t::key, "key3"},
        {sax_event_t::floating_point, -0.01},

        {sax_event_t::table, {"float", "exponent"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::floating_point, 5.0e22},
        {sax_event_t::key, "key2"},
        {sax_event_t::floating_point, 1000000.0},
        {sax_event_t::key, "key3"},
        {sax_event_t::floating_point, -0.02},

        {sax_event_t::table, {"float", "both"}},
        {sax_event_t::key, "key"},
        {sax_event_t::floating_point, 6.626e-34},

        {sax_event_t::table, {"float", "underscores"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::floating_point, 9224617.445991228313},
        {sax_event_t::key, "key2"},
        {sax_event_t::floating_point, 1.0e100},

        {sax_event_t::table, {"boolean"}},
        {sax_event_t::key, "True"},
        {sax_event_t::boolean, true},
        {sax_event_t::key, "False"},
        {sax_event_t::boolean, false},

        {sax_event_t::table, {"datetime"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::datetime, "1979-05-27T07:32:00Z"},
        {sax_event_t::key, "key2"},
        {sax_event_t::datetime, "1979-05-27T00:32:00-07:00"},
        {sax_event_t::key, "key3"},
        {sax_event_t::datetime, "1979-05-27T00:32:00.999999-07:00"},

        {sax_event_t::table, {"array"}},
        {sax_event_t::key, "key1"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::key, "key2"},
        {sax_event_t::start_array},
        {sax_event_t::string, "red"},
        {sax_event_t::string, "yellow"},
        {sax_event_t::string, "green"},
        {sax_event_t::finish_array, 3},
        {sax_event_t::key, "key3"},
        {sax_event_t::start_array},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::finish_array, 2},
        {sax_event_t::start_array},
        {sax_event_t::integer, 3},
        {sax_event_t::integer, 4},
        {sax_event_t::integer, 5},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_array, 2},
        {sax_event_t::key, "key4"},
        {sax_event_t::start_array},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::finish_array, 2},
        {sax_event_t::start_array},
        {sax_event_t::string, "a"},
        {sax_event_t::string, "b"},
        {sax_event_t::string, "c"},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_array, 2},
        {sax_event_t::key, "key5"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::key, "key6"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::finish_array, 2},

        {sax_event_t::table_array_item, {"products"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "Hammer"},
        {sax_event_t::key, "sku"},
        {sax_event_t::integer, 738594937},

        {sax_event_t::table_array_item, {"products"}},

        {sax_event_t::table_array_item, {"products"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "Nail"},
        {sax_event_t::key, "sku"},
        {sax_event_t::integer, 284758393},
        {sax_event_t::key, "color"},
        {sax_event_t::string, "gray"},

        {sax_event_t::table_array_item, {"fruit"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "apple"},

        {sax_event_t::table, {"fruit", "physical"}},
        {sax_event_t::key, "color"},
        {sax_event_t::string, "red"},
        {sax_event_t::key, "shape"},
        {sax_event_t::string, "round"},

        {sax_event_t::table_array_item, {"fruit", "variety"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "red delicious"},

        {sax_event_t::table_array_item, {"fruit", "variety"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "granny smith"},

        {sax_event_t::table_array_item, {"fruit"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "banana"},

        {sax_event_t::table_array_item, {"fruit", "variety"}},
        {sax_event_t::key, "name"},
        {sax_event_t::string, "plantain"},

        {sax_event_t::finish_document}
    }; // phew

    EXPECT_EQ(expected_events, handler.events);
}