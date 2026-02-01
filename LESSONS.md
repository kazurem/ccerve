## Declaring static variables in header files

## Destructors aren't called on exit()

## RAII
[RAII](https://en.cppreference.com/w/cpp/language/raii.html)

## std::forward, l and r values and Move Semantics 
[StackOverflow](https://stackoverflow.com/questions/26550603/why-should-i-use-stdforward)

## SNIFAE

## Const Pointer vs Pointer to Const
```cpp
// Read from right to left
int const  *ptr; // ptr is a pointer to constant int 
int *const ptr;  // ptr is a constant pointer to int
```

## The Static Initialization Order Fiasco
Better to have a function which returns it's static member than
create a static member in global space and create a function to return it.
```cpp
Singleton& getSingleton() {
    static Singleton singleton;
    return singleton;
}
```

## Initializing members in contructor body cause them to be initialized two time
[StackOverflow](https://stackoverflow.com/questions/9903248/initializing-fields-in-constructor-initializer-list-vs-constructor-body)

## Inline