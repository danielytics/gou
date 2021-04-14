#pragma once

#include <string>
#include <functional>
#include <memory>

namespace helpers {

namespace impl {

    template <typename... T>
    class Defer {
    public:
        Defer(std::tuple<T...> fns) : deferred(fns){}
        ~Defer(){
            std::apply([](T... args){call_deferred(args...);}, deferred);
        }
    private:
        std::tuple<T...> deferred;
        template <typename Fn, typename... Rest>
        static void call_deferred(Fn fn, Rest... rest){
            fn();
            call_deferred(rest...);
        }
        static void call_deferred() {}
    };

} // namespace impl

template <typename... T>
auto defer (T... fns) {
    return impl::Defer<T...>(std::make_tuple(fns...));
}
struct exit_scope_obj {
    template <typename Lambda>
    exit_scope_obj(Lambda& f) : func(f) {}
    template <typename Lambda>
    exit_scope_obj(Lambda&& f) : func(std::move(f)) {}
    ~exit_scope_obj() {func();}
private:
    std::function<void()> func;
};
#define CONCAT_IDENTIFIERS_(a,b) a ## b
#define ON_SCOPE_EXIT_(name,num) helpers::exit_scope_obj CONCAT_IDENTIFIERS_(name, num)
#define on_scope_exit ON_SCOPE_EXIT_(exit_scope_obj_, __LINE__)
#define defer_calls auto CONCAT_IDENTIFIERS_(deferred_calls, __LINE__) = helpers::defer

template <typename ContainerType>
void remove(ContainerType& container, std::size_t index)
{
    auto it = container.begin() + index;
    auto last = container.end() - 1;
    if (it != last) {
        // If not the last item, move the last into this element
        *it = std::move(*last);
    }
    // Remove the last item in the container
    container.pop_back();
}

template <typename ContainerType>
void move_back_and_replace(ContainerType& container, std::size_t index, typename ContainerType::reference&& data)
{
    container.push_back(std::move(container[index]));
    container[index] = std::move(data);
}

// If elements in 'in' have a deleted copy ctor, then _inserter may not work when compiling with EASTL, this is a workaround
template <typename InputContainer, typename OutputContainer>
void move_back (InputContainer& in, OutputContainer& out)
{
    for (auto& item : in) {
        out.push_back(std::move(item));
    }
}

template <typename ContainerType>
void pad_with (ContainerType& container, std::size_t size, typename ContainerType::value_type value)
{
    while (container.size() < size) {
        container.push_back(value);
    }
}

template <typename ContainerType>
std::size_t binary_search (ContainerType& container, typename ContainerType::value_type* item)
{
    std::size_t front = 0;
    std::size_t back = container.size() - 1;
    std::size_t prev = front;
    do {
        std::size_t midpoint = front + ((back - front) >> 1);
        if (midpoint == prev) {
            ++midpoint;
        }
        typename ContainerType::value_type* midpoint_item = &container[midpoint];
        if (item > midpoint_item) {
            front = midpoint;
        } else if (item < midpoint_item) {
            back = midpoint;
        } else {
            return midpoint;
        }
        prev = midpoint;
    } while (true);
}

std::string readToString(const std::string& filename);

template <typename T, typename Ctor, typename Dtor>
struct unique_maker_obj {
    explicit unique_maker_obj (Ctor ctor, Dtor dtor) : ctor(ctor), dtor(dtor) {}

    template <typename... Args>
    std::unique_ptr<T, Dtor> construct (Args... args) {
        return std::unique_ptr<T, Dtor>(ctor(args...), dtor);
    }
private:
    Ctor ctor;
    Dtor dtor;
};

template <typename T, typename Ctor>
unique_maker_obj<T, Ctor, void(*)(T*)> ptr (Ctor ctor) {
    return unique_maker_obj<T, Ctor, void(*)(T*)>(ctor, [](T* p){ delete p; });
}
template <typename T, typename Ctor, typename Dtor>
unique_maker_obj<T, Ctor, void(*)(T*)> ptr (Ctor ctor, Dtor dtor) {
    return unique_maker_obj<T, Ctor, void(*)(T*)>(ctor, dtor);
}
template <typename T>
std::unique_ptr<T> ptr (T* raw) {
    return std::unique_ptr<T>(raw);
}

template <typename T = char>
inline T* align(void* pointer, const uintptr_t bytes_alignment) {
    intptr_t value = reinterpret_cast<intptr_t>(pointer);
    value += (-value) & (bytes_alignment - 1);
    return reinterpret_cast<T*>(value);
}

inline intptr_t align(intptr_t pointer, const uintptr_t bytes_alignment) {
    return pointer + ((-pointer) & (bytes_alignment - 1));
}

template <typename T>
inline T roundDown(T n, T m) {
    return n >= 0 ? (n / m) * m : ((n - m + 1) / m) * m;
}

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin (reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end (reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }

} // namespace helpers
