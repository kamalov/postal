### Post Pascal Programming Language (aka postal)

[View syntax with proper highlighting](https://htmlpreview.github.io/?https://github.com/kamalov/postal/blob/main/syntax.html)

```
// user type
record FooType
    // simple fields
    i: integer          // signed int 64
    r: real             // double floating point
    s: string           // string (as value type)
    b: boolean          // true/false

    // complex fields
    integer_array: [integer]                    // dynamic array of integer
    foo_array: [FooType]                        // dynamic array of FooType
    string_to_integer_map: #[string, integer]   // string to integer map
    foo_to_string_map: #[FooType, FooType]      // FooType to FooType map
end

// example function
function example_function(p1: integer, p2: string) boolean
    i = 1000            // integer variable
    r = 1000.1          // floating point variable
    s = "some value"    // string variable 
    b = true            // boolean variable

    // if/else if/else
    if (i = 1000 or i > 1000 or r < 1000.1) and (b or not r)
        v = 1               
    else if s = "some"
        // all variables are in function scope (like old C or JavaScript)
        // no local scopes (yet)
        v = 2
    else 
        v = 3
    end

    // single line condition
    if i > 1000 then i = i + 1

    // endless loop
    loop 
        i = i + 1
        if i < 2000 then continue     // continue cycle
        if i > 2000 then break        // break loop
    end

    // iterate over range (both sides included)
    for 10..15
        range_item = item      // item for current element
        range_index = index    // index for current index of element
        print("iterating over range", index, item)
    end

    // array 
    integer_array = [integer]
    
    // adding into array (std functions declarations are inside 'prelude.post' file)
    array_push(integer_array, 5)
    array_push(integer_array, 6)
    array_push(integer_array, 7)
    integer_array[0] = 1

    // iterating over array
    for integer_array
        print("iterating over integer array", index, item)
    end

    // creating FooType objects
    f1 = FooType {}
    f1.i = 1
    f1.s = "one"
    
    f2 = FooType {}
    f2.r = 2.0
    f2.s = "two"

    // string to FooType map
    some_map = #[string, FooType]
    map_add(some_map, "one", f1)
    map_add(some_map, "two", f2)

    // iterating over map
    for map_keys(some_map)
        f = map_get_value(some_map, item)
        print("iterating over map", item, f.s)
    end

    // return 
    return true
end

// entry point
function run 
    b = example_function(1, "2")
    print("Hello, World!")
end

// more examples in 'examples' and 'compiler/tests' folders
```
**Goal: easy syntax (TypeScript like), fast compilation (Jai like), fast runtime (C like)**

**Priorities (by desc):**
1. Typed
2. Memsafe
3. Design time: better than TypeScript code writing speed (and TypeScript is smooth)
4. Run time: close to C runtime performance (speed and memory wise, with bump memory allocation)
5. Compile time: close to Jai compilation speed

**Implementation**

Phase zero (basic syntax and type checking, proof of concept, done):
- features:
  - [x] type checking
  - [x] type inference
  - [x] refcounted memory management
  - [x] Pascal/C/Basic hybrid syntax
  - [x] built-in strings, dyn array and hash table
  - [x] structs only
  - [x] transpilation to C++
- limitations:
  - no polymorpism
  - no global variables
  - single source file
  - ansi strings
  - no nested built-in collections (i.e. array of array, hashmap of array, etc.)

Phase one (critical):
- [ ] declarative? bump memory allocation mechanizm (and probably value types). Never ever been done before?
- [ ] polymorphism (generics, tagget unions)
- [ ] error handling mechanizm (exceptions or result or smth.)
- [ ] transpilation to C99 (or C89, or even jai... needs research)

Phase two (non-critical):
- [ ] function pointers
- [ ] enums
- [ ] tuples
- [ ] global variables
- [ ] nested collections
- [ ] language server
- [ ] ...
- [ ] multiple source files (modules?)
- [ ] utf-8 strings
- [ ] iterators (+ mapping with >>)
- [ ] function overload
- [ ] standard library

Backlog:
- better compile error messages
- semantic block comment (clojure style)
- scope variables
- ??? variant data type

**How to run**
- Current sequence is 'input.post' => compiler => 'cpp/generated.cpp' => 'your favourite C++ compiler' => executable.
- Input filename is hardcoded in 'compiler/main.rs' file.
- Output is 'compiler/cpp/generated.cpp'.
- 'compiler/cpp/prelude.cpp' contains C++ utils.
- Use 'compiler/cpp/run.bat' to compile with msvc cl.exe compiler (you should specify your C++ environment beforehand).   


 

