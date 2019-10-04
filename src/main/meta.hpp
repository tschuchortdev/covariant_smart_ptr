#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedTemplateParameterInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma once
#include <utility>
#include <memory>
#include <type_traits>
#include <exception>

namespace meta
{
    using true_type = char;
    using false_type = long;

    template<typename S>
    class without_ptr_qualifier final {
        static_assert(
        		!std::is_reference<S>::value,
                "template argument S of class without_ptr_qualifier<S> can not be of reference type"
        );

    public:
        using type = S;
    };

    template<typename S>
    class without_ptr_qualifier<S*> final {
        static_assert(
        		!std::is_reference<S>::value,
                "template argument S of class without_ptr_qualifier<S> can not be of reference type"
		);

    public:
        using type = typename without_ptr_qualifier<typename std::remove_pointer<S>::type>::type;
    };

    template<typename base_type, typename subtype>
    class is_subtype final {
        static true_type test(typename std::remove_reference<base_type>::type*);
        static false_type test(...);

    public:
        enum {
            value = sizeof(test(
                    static_cast<typename std::remove_reference<typename without_ptr_qualifier<subtype>::type>::type*>(nullptr)
            )) == sizeof(true_type)
                    && !std::is_same<typename std::remove_reference<typename without_ptr_qualifier<base_type>::type>::type,
                    typename std::remove_reference<typename without_ptr_qualifier<subtype>::type>::type>::value
        };
    };

    template<typename origin, typename target>
    class is_implicitly_convertible final {
        static true_type test(typename std::remove_reference<target>::type);
        static const false_type test(...);

    public:
        using origin_type = origin;
        using target_type = target;

        enum { value = sizeof(test(std::declval<origin_type>())) == sizeof(true_type) };
    };

    template<bool left, bool right>
    class ignore_left_bool final {
    public:
        enum { value = right };
    };

    template<typename left, typename right>
    class ignore_left_type final {
    public:
        using type = right;
    };

    template<typename T, typename ... arg_types>
    class has_ctor final {
        template<typename... ctor_arg_types>
        static auto test(void*) ->
                ignore_left_type<decltype(T((std::declval<ctor_arg_types>())...)), true_type>;

        template<typename...>
        static const false_type test(...);

    public:
        enum { value = sizeof(test<arg_types...>(nullptr)) == sizeof(true_type) };
    };
}

#pragma clang diagnostic pop