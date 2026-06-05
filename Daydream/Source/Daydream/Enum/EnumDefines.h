#pragma once

#define DEFINE_ENUMTYPE_FLAG_OPERATORS(EnumType) \
inline EnumType operator|(EnumType _lhs, EnumType _rhs) \
{ \
    using underlyingType = std::underlying_type_t<EnumType>; \
    underlyingType result = static_cast<underlyingType>(_lhs) | static_cast<underlyingType>(_rhs); \
    return static_cast<EnumType>(result); \
} \
inline EnumType operator&(EnumType _lhs, EnumType _rhs) \
{ \
    using underlyingType = std::underlying_type_t<EnumType>; \
    underlyingType result = static_cast<underlyingType>(_lhs) & static_cast<underlyingType>(_rhs); \
    return static_cast<EnumType>(result); \
} \
inline EnumType operator~(EnumType _target) \
{ \
    using underlyingType = std::underlying_type_t<EnumType>; \
    underlyingType result = ~static_cast<underlyingType>(_target); \
    return static_cast<EnumType>(result); \
} \
inline EnumType& operator|=(EnumType& _lhs, EnumType _rhs) \
{ \
    _lhs = _lhs | _rhs; \
    return _lhs; \
} \
inline EnumType& operator&=(EnumType& _lhs, EnumType _rhs) \
{ \
    _lhs = _lhs & _rhs; \
    return _lhs; \
}