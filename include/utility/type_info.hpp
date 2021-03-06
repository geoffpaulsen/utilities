#ifndef UTILITY_TYPE_INFO_HPP
#define UTILITY_TYPE_INFO_HPP
#include <ostream>
#include <string>
#include <type_traits>
#include <typeindex>

#include <boost/core/demangle.hpp>

namespace utility {

/// Holds type information including cv-qualifiers and reference type.
struct Type_info {
    bool is_const;
    bool is_volatile;
    std::type_index index;
    bool is_lvalue_reference;
    bool is_rvalue_reference;

    /// cv-qualified, unmangled type name with reference.
    inline std::string full_type_name() const;

    /// Core type name, unqualified with reference removed.
    inline std::string type_name() const;

    /// Human readable representation of the type.
    inline operator std::string() const;
};

/// Create a Type_info object from type T.
template <typename T>
Type_info get_type_info();

/// Memberwise equality.
inline bool operator==(const Type_info& lhs, const Type_info& rhs);
inline bool operator!=(const Type_info& lhs, const Type_info& rhs);

/// Provides unique ordering.
inline bool operator<(const Type_info& lhs, const Type_info& rhs);

/// Uses string conversion operator to output to stream.
inline std::ostream& operator<<(std::ostream& os, const Type_info& rhs);

// IMPLEMENTATIONS - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
Type_info get_type_info() {
    return Type_info{
        std::is_const<typename std::remove_reference<T>::type>::value,
        std::is_volatile<typename std::remove_reference<T>::type>::value,
        typeid(T), std::is_lvalue_reference<T>::value,
        std::is_rvalue_reference<T>::value};
}

namespace detail {

inline std::string generate_object_type_string(const Type_info& info) {
    auto result = std::string{""};
    if (info.is_const) {
        result.append("const ");
    }
    if (info.is_volatile) {
        result.append("volatile ");
    }
    result.append(boost::core::demangle(info.index.name()));
    if (info.is_lvalue_reference) {
        result.append("&");
    } else if (info.is_rvalue_reference) {
        result.append("&&");
    }
    return result;
}

inline std::string generate_pointer_type_string(const Type_info& info) {
    auto result = std::string{boost::core::demangle(info.index.name())};
    if (info.is_const) {
        result.append(" const");
    }
    if (info.is_volatile) {
        result.append(" volatile");
    }
    if (info.is_lvalue_reference) {
        result.append("&");
    } else if (info.is_rvalue_reference) {
        result.append("&&");
    }
    return result;
}

inline std::string generate_function_pointer_type_string(
    const Type_info& info) {
    auto result = std::string{boost::core::demangle(info.index.name())};
    auto qualifiers = std::string{""};
    if (info.is_const) {
        qualifiers.append(" const");
    }
    if (info.is_volatile) {
        qualifiers.append(" volatile");
    }
    if (info.is_lvalue_reference) {
        qualifiers.append("&");
    } else if (info.is_rvalue_reference) {
        qualifiers.append("&&");
    }
    auto pos = result.find(')');
    result.insert(pos, qualifiers);
    return result;
}

inline bool is_pointer_type(const std::string& name) {
    return name.back() == '*';
}

inline bool is_function_pointer_type(const std::string& name) {
    return name.find(')') != std::string::npos;
}

using Generator_fn_t = std::string (*)(const Type_info&);
inline Generator_fn_t find_string_generator(const Type_info& info) {
    const auto type_name =
        std::string{boost::core::demangle(info.index.name())};
    auto return_func = Generator_fn_t{generate_object_type_string};
    if (is_pointer_type(type_name)) {
        return_func = generate_pointer_type_string;
    } else if (is_function_pointer_type(type_name)) {
        return_func = generate_function_pointer_type_string;
    }
    return return_func;
}

}  // namespace detail

std::string Type_info::full_type_name() const {
    // Generate string based on function/pointer/object type.
    auto generate_string_fn = detail::find_string_generator(*this);
    return generate_string_fn(*this);
}

std::string Type_info::type_name() const {
    return boost::core::demangle(this->index.name());
}

Type_info::operator std::string() const {
    return this->full_type_name();
}

bool operator==(const Type_info& lhs, const Type_info& rhs) {
    if (lhs.index != rhs.index) {
        return false;
    }
    return (lhs.is_const == rhs.is_const) &&
           (lhs.is_volatile == rhs.is_volatile) &&
           (lhs.is_lvalue_reference == rhs.is_lvalue_reference) &&
           (lhs.is_rvalue_reference == rhs.is_rvalue_reference);
}

bool operator!=(const Type_info& lhs, const Type_info& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Type_info& lhs, const Type_info& rhs) {
    if (lhs.index == rhs.index) {
        if (!lhs.is_const && rhs.is_const) {
            return true;
        } else if (lhs.is_const && !rhs.is_const) {
            return false;
        }
        if (!lhs.is_volatile && rhs.is_volatile) {
            return true;
        } else if (lhs.is_volatile && !rhs.is_volatile) {
            return false;
        }
        if (!lhs.is_lvalue_reference && rhs.is_lvalue_reference) {
            return true;
        } else if (lhs.is_lvalue_reference && !rhs.is_lvalue_reference) {
            return false;
        }
        if (!lhs.is_rvalue_reference && rhs.is_rvalue_reference) {
            return true;
        } else if (lhs.is_rvalue_reference && !rhs.is_rvalue_reference) {
            return false;
        }
        return false;
    }
    return lhs.index < rhs.index;
}

std::ostream& operator<<(std::ostream& os, const Type_info& rhs) {
    os << static_cast<std::string>(rhs);
    return os;
}

}  // namespace utility
#endif  // UTILITY_TYPE_INFO_HPP
