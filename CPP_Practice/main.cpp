#include <chrono>
#include <iostream>
#include "StaticRef.h"

BEGIN_CLASS(Person)
functions(
    func(&Person::processPerson),
    func(&Person::getAge)
)
END_CLASS()


int main(int argc, char* args[]){
    auto info = type_info_m<Person>();
    auto funcPointer = std::get<0>(info.functions).pointer;
    Person p;
    // (p.*funcPointer)();
    // std::cout<<std::get<0>(info.functions).name;
    static_ref::visitTuple<0>(type_info_m<Person>::functions,&p);
    
    return 0;
}
