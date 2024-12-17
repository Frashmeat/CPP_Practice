#pragma once
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include "util.h"

namespace dlgt{
    template <typename>
    class delegate;

    template <typename Ret, typename... Args>
    class delegate<Ret(Args...)>{
    public:
        using SelfType = delegate;
        using ReturnType = Ret;
        using FunctionType = Ret(void const*, Args...);
        using Type = Ret(Args...);

        delegate() noexcept : context(nullptr), function(nullptr){
        }

        delegate(const delegate& other) noexcept = default;
        delegate(delegate&& other) noexcept = default;
        virtual ~delegate() = default;

        template <auto Candidate>
        explicit delegate(util::Spreader<Candidate> spreader) noexcept : context(nullptr){
            this->bind<Candidate>();
        }

        template <auto Candidate, typename Class>
        explicit delegate(util::Spreader<Candidate> spreader, Class& instance) noexcept : context(nullptr){
            this->template bind<Candidate>(instance);
        }

        explicit delegate(FunctionType* function, void const* payload): context(nullptr){
            this->bind(function, payload);
        }


        // 普通函数的绑定
        template <auto Candidate>
        void bind() noexcept{
            static_assert(std::is_invocable_r_v<Ret, decltype(Candidate), Args...>,
                          "Candidate Must be a Function");

            this->context = nullptr;
            this->function = [](void const*, Args... args)-> Ret{
                return Ret(std::invoke(Candidate, std::forward<Args>(args)...));
            };
        }

        template <auto Candidate, typename Class>
        void bind(Class& instance) noexcept{
            static_assert(std::is_invocable_r_v<Ret, decltype(Candidate), Class*, Args...>,
                          "Candidate Must be a Function");

            this->context = &instance;
            this->function = [](void const* payload, Args... args)-> Ret{
                Class* typeInstance = static_cast<Class*>(const_cast<util::same_constness_t<void, Class>*>(payload));
                // Class* typeInstance = static_cast<Class*>(const_cast<void *>(payload));
                return Ret(std::invoke(Candidate, typeInstance, std::forward<Args>(args)...));
            };
        }

        void bind(FunctionType* function, void const* payload){
            assert(function);
            this->function = function;
            this->context = payload;
        }

        operator bool() const noexcept{
            return this->function == nullptr;
        }

        Ret operator()(Args... args) const{
            return this->function(this->context, std::forward<Args>(args)...);
        }

        delegate& operator=(const delegate& other){
            this->function = other.function;
            this->context = other.context;
        }

        template <typename Type>
        bool operator==(const delegate<Type>& other){
            return this->isSameDelegate(other);
        }

        template <typename Type>
        bool operator!=(const delegate<Type>& other){
            return !this->isSameDelegate(other);
        }

        FunctionType* getBindFunc() const noexcept{
            return this->function;
        }

        const void* getContext() const noexcept{
            return this->context;
        }

        bool isMemberFunc() const noexcept{
            return this->context != nullptr;
        }

    private:
        template <typename Type>
        bool isSameDelegate(const delegate<Type>& other){
            if constexpr (std::is_same_v<Ret(Args...), Type>){
                return this->function == other.function && this->context == other.context;
            }

            return false;
        }


        // 类指针 (如果是类函数则有值)
        void const* context;
        // 函数指针 
        FunctionType* function;
    };


    class BasicSink{
        friend class dispatcher;

    public:
        using SelfType = BasicSink;

        virtual ~BasicSink() = default;

        BasicSink() noexcept = default;
        BasicSink(const BasicSink& other) = default;
        BasicSink(BasicSink&& other) = default;

        BasicSink& operator=(const BasicSink& other) = default;
        BasicSink& operator=(BasicSink&& other) = default;

        virtual void trigger(void*) const = 0;
    };


    template <typename EventType>
    class Sink : public BasicSink{
        using SelfType = Sink;
        using DelegateType = delegate<EventType>;

    public:
        Sink() = default;

        // ~Sink(){
        // }

        template <auto Candidate>
        void connect(){
            util::IDType delegateId = util::non_type::getId<DelegateType, Candidate>();
            if (auto iter = this->delegatesMap.find(delegateId); iter == delegatesMap.cend()){
                this->delegatesMap.emplace(delegateId, DelegateType(util::spreadArg<Candidate>));
            }
            else{
                iter->second.template bind<Candidate>();
            }
        }

        template <auto Candidate, typename Class>
        void connect(Class& instance){
            util::IDType delegateId = util::non_type::getId<DelegateType, Candidate>();
            if (auto iter = this->delegatesMap.find(delegateId); iter == delegatesMap.cend()){
                this->delegatesMap.emplace(delegateId, DelegateType(util::spreadArg<Candidate>));
            }
            else{
                iter->second.template bind<Candidate>(instance);
            }
        }

        template <auto Candidate>
        void disconnect(){
            util::IDType id = util::non_type::getId<DelegateType, Candidate>();
            if (auto iter = this->delegatesMap.find(id); iter == this->delegatesMap.cend()){
                delegatesMap.erase(id);
            }
        }


        template <auto Candidate, typename Class>
        void disconnect(){
            util::IDType id = util::non_type::getId<DelegateType, Candidate>();
            if (auto iter = this->delegatesMap.find(id); iter == this->delegatesMap.cend()){
                delegatesMap.erase(id);
            }
        }

        void trigger(void* event) const override{
            std::ranges::for_each(this->delegatesMap, [event](const auto& pair){
                pair.second(*(static_cast<EventType*>(event)));
            });
        }

    private:
        std::unordered_map<util::IDType, DelegateType> delegatesMap;
    };


    class dispatcher{

    };
}
