#ifndef MOE_TYPE_LIST_H_
#define MOE_TYPE_LIST_H_

// An implementation of a a-la-Alexandrescu type list using variadic templates.
// This implementation isn't complete (lacks appending, erasing, replacing, sorting...)
// but it is sufficient at the moment for our usage.
namespace moe
{
    // A dummy type we use to identify the end of a typelist
    class NullType {};

    // A Type List is just an empty template containing:
    // - a type (the Head)
    // - another type list containing the next types (the Tail).
    // We use the NullType as an "end of list" marker.
    template <class T, typename... Args>
    struct TypeList
    {
        typedef T                   Head;
        typedef TypeList<Args...>   Tail;
    };

    // Specialization for the last node of the list (Tail is NullType)
    template <class T>
    struct TypeList<T>
    {
        typedef T           Head;
        typedef NullType    Tail;
    };


    // TypeListLength returns the length of a type list, or 0 for an empty list (obviously).
    template <class TList>
    struct TypeListLength;

    // Single element list...
    template <class T>
    struct TypeListLength<TypeList<T>>
    {
        enum { value = 1 };
    };

    // Empty list or end-of-list "node"...
    template <>
    struct TypeListLength<TypeList<NullType>>
    {
        enum { value = 0 };
    };

    // Tail is not NullType so length is at least 1 + value of next type list length...
    template <class T, typename... Args>
    struct TypeListLength<TypeList<T, Args...> >
    {
        enum { value = 1 + TypeListLength<typename TypeList<T, Args...>::Tail>::value };
    };


    // TypeListIndexOf returns the index of the given type T in a type list,
    // or -1 if the end of the list is reached without finding it.
    template <class TList, class T>
    struct TypeListIndexOf;

    // Single element list...
    template <class T, typename... Args>
    struct TypeListIndexOf<TypeList<T, Args...>, T>
    {
        enum { value = 0 };
    };


    // Empty list or end-of-list "node"...
    template <class U>
    struct TypeListIndexOf<NullType, U>
    {
        enum { value = -1 };
    };

    // The entry point for TypeListIndexOf recursion, stores the result of the
    // recursion in a private temp enum, then uses the ternary operator
    // to set the public value of the template.
    template <class Head, class T, typename... Args>
    struct TypeListIndexOf<TypeList<Head, Args...>, T>
    {
    private:
        enum { temp = TypeListIndexOf<typename TypeList<Head, Args...>::Tail, T>::value };
    public:
        enum { value = (temp == -1 ? -1 : 1 + temp) };
    };
}

#endif // MOE_TYPE_LIST_H_
