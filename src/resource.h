#pragma once
namespace NResource {
    struct TResource {
        unsigned char *Ptr;
        unsigned int Length;
        typedef unsigned char value_type;
        constexpr TResource(unsigned char *ptr, unsigned int length) : Ptr(ptr), Length(length) {}
        [[nodiscard]] const unsigned char *begin() const { return Ptr; }
        [[nodiscard]] const unsigned char *cbegin() const { return Ptr; }
        [[nodiscard]] const unsigned char *end() const { return Ptr + Length; }
        [[nodiscard]] const unsigned char *cend() const { return Ptr + Length; }
        [[nodiscard]] const unsigned char *data() const { return Ptr; }
        [[nodiscard]] size_t size() const { return Length; }
    };
}
#include "resource_def.h"