#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <ranges>
#include "StaticRef.h"
#include "TypeList.h"

BEGIN_CLASS(Person)
    functions(
    func(&Person::processPerson),
    func(&Person::getAge)
)
END_CLASS()








int main(int argc, char* args[]){
    // auto info = type_info_m<Person>();
    // auto funcPointer = std::get<0>(info.functions).pointer;
    // Person p;
    // // (p.*funcPointer)();
    // // std::cout<<std::get<0>(info.functions).name;
    // static_ref::visitTuple<0>(type_info_m<Person>::functions,&p);
    // // static_ref::visitTuple(info.functions,[](auto&& elem , auto&& instance){
    // // },std::make_index_sequence<std::tuple_size_v<decltype(info.functions)>>(),&p);

    // auto i = type_list<int , char , double>();
    // type_list_head<decltype(i)>::head_type ii = 1;
    // using test = type_list_tail<decltype(i)>::tail_type;

    // auto map = std::map<int, int>{{1, 2,}, {5, 3}, {45, 6}};
    // auto values = map | std::views::values;
    // auto vector = std::ranges::to<std::vector>(map);
    // std::ranges::for_each(vector, [](const auto& value){ std::cout << value << ' '; });

    // auto i = type_list::type_list<int , char , double>();
    
    // using typeList = type_list::type_list<int , char , double>;
    // using res =  type_list::getTypeByIndex<typeList,1>::type; // Char

    
    return 0;
}
