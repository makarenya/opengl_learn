#include "errors.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <array>

#define BUILDER_PROPERTY2(T1, T2, NAME) \
auto &Set##NAME(const T1 &v1, const T2 &v2) { \
    NAME##_ = std::make_tuple(v1, v2); \
    return *this; \
} \
std::tuple<T1, T2> NAME##_

#define BUILDER_PROPERTY3(T1, T2, T3, NAME) \
auto &Set##NAME(const T1 &v1, const T2 &v2, const T3 &v3) { \
    NAME##_ = std::make_tuple(v1, v2, v3); \
    return *this; \
} \
std::tuple<T1, T2, T3> NAME##_

#define BUILDER_PROPERTY4(T1, T2, T3, T4, NAME) \
auto &Set##NAME(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { \
    NAME##_ = std::make_tuple(v1, v2, v3, v4); \
    return *this; \
} \
std::tuple<T1, T2, T3, T4> NAME##_

#define BUILDER_LIST(TYPE, NAME) \
auto &Add##NAME(const TYPE &v) { \
    NAME##s_.emplace_back(v); \
    return *this; \
} \
std::vector<TYPE> NAME##s_

#define BUILDER_LIST2(T1, T2, NAME) \
auto &Add##NAME(const T1 &v1, const T2 &v2) { \
    NAME##s_.emplace_back(std::make_tuple(v1, v2)); \
    return *this; \
} \
std::vector<std::tuple<T1, T2>> NAME##s_

#define BUILDER_LIST3(T1, T2, T3, NAME) \
auto &Add##NAME(const T1 &v1, const T2 &v2, const T3 &v3) { \
    NAME##s_.emplace_back(std::forward_as_tuple(v1, v2, v3)); \
    return *this; \
} \
std::vector<std::tuple<T1, T2, T3>> NAME##s_

#define BUILDER_PROPERTY(TYPE, NAME) \
auto &Set##NAME(const TYPE &v) { \
    NAME##_ = v; \
    return *this; \
} \
TYPE NAME##_

#define BUILDER_PROPERTY_ALT(TYPE, NAME, FIELD_NAME) \
auto &Set##NAME(const TYPE &v) { \
    FIELD_NAME = v; \
    return *this; \
} \
TYPE FIELD_NAME

#define BUILDER_MAP(KEY, VALUE, NAME) \
auto &Set##NAME(const KEY &k, const VALUE &v) { \
    NAME##s_.emplace(k, v); \
    return *this; \
} \
std::map<KEY, VALUE> NAME##s_

#define BUILDER_MAP2(KEY, V1, V2, NAME) \
auto &Set##NAME(const KEY &k, const V1 &v1, const V2 &v2) { \
    NAME##s_.emplace(k, std::forward_as_tuple(v1, v2)); \
    return *this; \
} \
std::map<KEY, std::tuple<V1, V2>> NAME##s_

#define BUILDER_MAP_ALT(KEY, VALUE, NAME, FIELD_NAME) \
auto &Set##NAME(const KEY &k, const VALUE &v) { \
    FIELD_NAME.emplace(k, v); \
    return *this; \
} \
std::map<KEY, VALUE> FIELD_NAME
