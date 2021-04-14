#pragma once

namespace utilities {

    template <typename T>
    class Iterator {
    public:
        Iterator(T iterable) : iterable(iterable) {}
        ~Iterator() {}
        typename T::Type* begin() const {return iterable.data;}
        typename T::Type* end() const {return iterable.data + iterable.count;}
        typename T::Type& first() const {return *iterable.data;}
        typename T::Type& last() const {return *(iterable.data + iterable.count - 1);}
    private:
        T iterable;
    };
    template <typename T>
    class ConstIterator {
    public:
        ConstIterator(T iterable) : iterable(iterable) {}
        ~ConstIterator() {}
        const typename T::Type* begin() const {return iterable.data;}
        const typename T::Type* end() const {return iterable.data + iterable.count;}
        const typename T::Type& first() const {return *iterable.data;}
        const typename T::Type& last() const {return *(iterable.data + iterable.count - 1);}
    private:
        const T iterable;
    };

    template <typename T>
    Iterator<T> iterate (T iterable) {
        return Iterator<T>(iterable);
    }
    template <typename T>
    ConstIterator<T> const_iterate (const T iterable) {
        return ConstIterator<T>(iterable);
    }

    template <typename T>
    constexpr typename std::underlying_type<T>::type enum_value (T enumeration) {
        return static_cast<typename std::underlying_type<T>::type>(enumeration);
    }
}
