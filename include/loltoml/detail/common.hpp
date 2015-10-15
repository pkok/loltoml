#ifndef LOLTOML_DETAIL_COMMON_HPP
#define LOLTOML_DETAIL_COMMON_HPP

#include "loltoml/version.hpp"

#define LOLTOML_VERSION_NAMESPACE LOLTOML_VERSION_NAMESPACE_HELPER(LOLTOML_MAJOR_VERSION, LOLTOML_MINOR_VERSION, LOLTOML_PATCH_VERSION)
#define LOLTOML_VERSION_NAMESPACE_HELPER(x, y, z) LOLTOML_VERSION_NAMESPACE_HELPER2(x, y, z)
#define LOLTOML_VERSION_NAMESPACE_HELPER2(x, y, z) v_##x##_##y##_##z

#define LOLTOML_OPEN_NAMESPACE namespace loltoml { inline namespace LOLTOML_VERSION_NAMESPACE {
#define LOLTOML_CLOSE_NAMESPACE }}

#endif // LOLTOML_DETAIL_COMMON_HPP
