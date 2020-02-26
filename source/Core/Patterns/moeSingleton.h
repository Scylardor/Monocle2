#ifndef MOE_SINGLETON_H_
#define MOE_SINGLETON_H_

// A Singleton is a class we want to have an unique instance of in the whole program.
// To achieve that, this template implementation provides only an "Instance" static method
// to acquire the "singletonized" object. Deleted constructors make it impossible to acquire
// it by any other way.
namespace moe
{
    template <class T>
    class Singleton
    {
    public:
        static T&  Instance()
        {
            // Static-var-in-function initialization is thread-safe since C++11!
            static T instance;
            return instance;
        }

    protected:
        Singleton() {}
        virtual ~Singleton() {}

        Singleton(Singleton const&)             = delete;   // Copy construct
        Singleton(Singleton&&)                  = delete;   // Move construct
        Singleton& operator=(Singleton const&)  = delete;   // Copy assign
        Singleton& operator=(Singleton &&)      = delete;   // Move assign


    };
}


#endif // MOE_SINGLETON_H_
