#pragma once
#include <iostream>
#include <ostream>
#include <tuple>
#include <type_traits>

class Person{
public:
    Person() = default;

    Person(int age): age(age){
    }

    ~Person() = default;

    int getAge() const{
        std::cout << "age : run" << this->age << std::endl;
        return this->age;
    }

    void processPerson() const{
        std::cout << "ProcessPerson" << this->age << std::endl;
    }

public:
    int age = 1;
};


namespace static_ref{
    namespace variable_traits{
        namespace detail{
            // 普通变量 泛化
            template <typename Type>
            struct variable_type{
                using type = Type;
            };

            // 成员变量的特化
            template <typename Type, typename Class>
            struct variable_type<Type Class::*>{
                using type = Type;
            };
        }

        template <typename T>
        using variable_traits_t = typename detail::variable_type<T>::type;


        template <typename T>
        struct basic_variable_traits{
            using self_type = variable_traits_t<T>;
            static constexpr bool is_member = std::is_member_pointer_v<T>;
            static constexpr bool is_const = std::is_const_v<T>;
        };

        // 变量萃取的接口

        // 泛化
        template <typename T>
        struct variable_traits : basic_variable_traits<T>{
        };

        // 指针特化
        template <typename T>
        struct variable_traits<T*> : basic_variable_traits<T>{
            using point_type = T*;
        };

        // 成员变量指针特化
        template <typename T, typename Class>
        struct variable_traits<T Class::*> : basic_variable_traits<T Class::*>{
            using point_type = T Class::*;
            using class_type = Class;
        };
    }

    namespace function_traits{
        template <typename Ret, typename... Args>
        struct basic_function_traits{
        };

        template <typename Ret, typename... Args>
        struct basic_function_traits<Ret(Args...)>{
            using args = std::tuple<Args...>;
            using ret = Ret;
        };

        // 泛化
        template <typename Ret, typename... Args>
        struct function_traits{
        };

        // 普通函数特化
        template <typename Ret, typename... Args>
        struct function_traits<Ret(Args...)> : basic_function_traits<Ret(Args...)>{
            using func_type = Ret(Args...);
            using func_pointer_type = Ret(*)(Args...);
            using args_with_type = std::tuple<Args...>;
            static constexpr bool is_member = false;
            static constexpr bool is_const = false;
        };

        // 成员函数非const 特化
        template <typename Ret, typename Class, typename... Args>
        struct function_traits<Ret(Class::*)(Args...)> : basic_function_traits<Ret(Args...)>{
            using func_type = Ret(Class::*)(Args...);
            using func_pointer_type = Ret(Class::*)(Args...);
            using args_with_type = std::tuple<Class*, Args...>;
            static constexpr bool is_member = true;
            static constexpr bool is_const = false;
        };

        // 成员函数const 特化
        template <typename Ret, typename Class, typename... Args>
        struct function_traits<Ret(Class::*)(Args...) const> : basic_function_traits<Ret(Args...)>{
            using func_type = Ret(Class::*)(Args...) const;
            using args_with_type = std::tuple<Class*, Args...>;
            using func_pointer_type = Ret(Class::*)(Args...) const;
            static constexpr bool is_member = true;
            static constexpr bool is_const = true;
        };
    }

    namespace type_traits{
        template <typename T, bool isFunc>
        struct basic_field_traits;

        // 记录函数
        template <typename T>
        struct basic_field_traits<T, true> : function_traits::function_traits<T>{
            using traits = function_traits::function_traits<T>;

            constexpr bool is_member() const{
                return traits::is_member;
            }

            constexpr bool is_const() const{
                return traits::is_const;
            }

            constexpr bool is_function() const{
                return true;
            }

            constexpr bool is_variable() const{
                return false;
            }

            constexpr size_t param_count() const{
                return std::tuple_size_v<typename traits::args>;
            }
        };

        // 记录变量
        template <typename T>
        struct basic_field_traits<T, false> : variable_traits::variable_traits<T>{
            using traits = variable_traits::variable_traits<T>;

            constexpr bool is_member() const{
                return traits::is_member;
            }

            constexpr bool is_const() const{
                return traits::is_const;
            }

            constexpr bool is_function() const{
                return false;
            }

            constexpr bool is_variable() const{
                return true;
            }

            constexpr size_t param_count() const{
                return -1;
            }
        };

        template <typename T>
        struct is_function{
            static constexpr bool value = std::is_function_v<std::remove_pointer_t<T>> ||
                std::is_member_function_pointer_v<T>;
        };


        template <typename T>
        struct field_traits : basic_field_traits<T, is_function<T>::value>{
            constexpr field_traits(T&& pointer, std::string_view name) : pointer(pointer), name(name){
            }

            std::string_view name;
            T pointer;
        };

        template <typename T>
        struct type_info;

        // #define TRAITS_CLASS_FUNC(X) function_traits::function_traits<decltype(X)>
        //         template <>
        //         struct type_info<Person>{
        //             using functions = std::tuple<TRAITS_CLASS_FUNC(&Person::getAge)>;
        //         };

        // template<>
        // struct type_info<Person>{
        //     // 函数的反射
        //     static constexpr auto functions = std::make_tuple(field_traits{&Person::getAge} , field_traits{&Person::processPerson});
        // };
    }

    template <size_t idx, typename... Args, typename Class>
    void visitTuple(const std::tuple<Args...>& tuple, Class* instance){
        using tuple_type = std::tuple<Args...>;
        if constexpr (idx >= std::tuple_size_v<tuple_type>){
            return;
        }
        else{
            if constexpr (auto elem = std::get<idx>(tuple); elem.param_count() == 0){
                // 调用成员函数
                (instance->*elem.pointer)();
            }
            
            // 递归的进行遍历
            visitTuple<idx + 1>(tuple, instance);
        }
    }
}


#define BEGIN_CLASS(X) template <> struct static_ref::type_traits::type_info<X> {
#define functions(...) static constexpr auto functions = std::make_tuple(__VA_ARGS__);
#define func(F) static_ref::type_traits::field_traits{F , #F}
#define END_CLASS() } ;


template <typename T>
using type_info_m = static_ref::type_traits::type_info<T>;
