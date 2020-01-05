#include "errors.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

#define BUILDER_PROPERTY2(T1, T2, NAME) \
auto &Set##NAME(T1 v1, T2 v2) { \
    NAME##_ = std::forward_as_tuple(std::move(v1), std::move(v2)); \
    return *this; \
} \
std::tuple<T1, T2> NAME##_

#define BUILDER_PROPERTY3(T1, T2, T3, NAME) \
auto &Set##NAME(T1 v1, T2 v2, T3 v3) { \
    NAME##_ = std::forward_as_tuple(std::move(v1), std::move(v2), std::move(v3)); \
    return *this; \
} \
std::tuple<T1, T2, T3> NAME##_

#define BUILDER_PROPERTY4(T1, T2, T3, T4, NAME) \
auto &Set##NAME(T1 v1, T2 v2, T3 v3, T4 v4) { \
    NAME##_ = std::forward_as_tuple(std::move(v1), std::move(v2), std::move(v3), std::move(v4)); \
    return *this; \
} \
std::tuple<T1, T2, T3, T4> NAME##_

#define BUILDER_LIST(TYPE, NAME) \
auto &Add##NAME(TYPE v) { \
    NAME##s_.emplace_back(std::move(v)); \
    return *this; \
} \
std::vector<TYPE> NAME##s_

#define BUILDER_LIST2(T1, T2, NAME) \
auto &Add##NAME(T1 v1, T2 v2) { \
    NAME##s_.emplace_back(std::forward_as_tuple(std::move(v1), std::move(v2))); \
    return *this; \
} \
std::vector<std::tuple<T1, T2>> NAME##s_

#define BUILDER_PROPERTY(TYPE, NAME) \
auto &Set##NAME(TYPE v) { \
    NAME##_ = std::move(v); \
    return *this; \
} \
TYPE NAME##_

#define BUILDER_MAP(KEY, VALUE, NAME) \
auto &Set##NAME(KEY k, VALUE v) { \
    NAME##s_.emplace(std::move(k), std::move(v)); \
    return *this; \
} \
std::map<KEY, VALUE> NAME##s_

#define BUILDER_MAP_ALT(KEY, VALUE, NAME, FIELD_NAME) \
auto &Set##NAME(KEY k, VALUE v) { \
    FIELD_NAME.emplace(std::move(k), std::move(v)); \
    return *this; \
} \
std::map<KEY, VALUE> FIELD_NAME
