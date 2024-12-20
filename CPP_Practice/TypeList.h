#pragma once

namespace type_list{
    template <typename... Args>
    struct type_list{
        static constexpr size_t size = sizeof...(Args);
    };

    template <typename>
    struct type_list_head{
    };

    template <typename Head, typename... Remains>
    struct type_list_head<type_list<Head, Remains...>>{
        using head_type = Head;
    };

    template <typename>
    struct type_list_tail{
    };

    template <typename Head, typename... Remains>
    struct type_list_tail<type_list<Head, Remains...>>{
        using tail_type = type_list<Remains...>;
    };


    template <typename, size_t>
    struct get_type_by_index{
    };

    template <typename Cur, typename... Remains>
    struct get_type_by_index<type_list<Cur, Remains...>, 0>{
        using type = Cur;
    };

    template <typename Cur, typename... Remains, size_t idx>
    struct get_type_by_index<type_list<Cur, Remains...>, idx>{
        using type = typename get_type_by_index<type_list<Remains...>, idx - 1>::type;
    };


    template <typename, template<typename> typename, size_t>
    struct count{
    };

    template <typename Cur, typename... Remains, template<typename> typename F>
    struct count<type_list<Cur, Remains...>, F, 0>{
        static constexpr size_t value = F<Cur>::value ? 1 : 0;
    };

    template <typename Cur, typename... Remains, template<typename> typename F, size_t idx>
    struct count<type_list<Cur, Remains...>, F, idx>{
        static constexpr size_t value = (F<Cur>::value ? 1 : 0) + count<type_list<Remains...>, F, idx - 1>::value;
    };

    template<typename T , template<typename> typename F , size_t idx>
    using count_v = typename count<T , F , idx>::value;
    
    
    template <typename, template<typename> typename>
    struct map{
    };

    template <typename... Args, template<typename> typename F>
    struct map<type_list<Args...>, F>{
        using type = type_list<typename F<Args>::value...>;
    };

    template <typename T, template<typename> typename F>
    using map_v = typename map<T, F>::type;
    
    // // 
    // template <typename, size_t idx>
    // struct get_integer_count{
    // };
    //
    // template <typename Cur, typename... Remains>
    // struct get_integer_count<type_list<Cur, Remains...>, 0>{
    //     static constexpr size_t value = std::is_integral_v<Cur> ? 1 : 0;
    // };
    //
    // template<typename Cur , typename ... Remains ,size_t idx>
    // struct get_integer_count<type_list<Cur , Remains...>,idx>{
    //     static constexpr size_t value = (std::is_integral_v<Cur> ? 1 : 0) + get_integer_count<type_list<Remains...>,idx-1>::value;
    // };
}
