# Pascal Highlighting Test

```pascal
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
    i = 1000                      // integer literal
    r = 1000.1                    // floating point literal
    s = "some" + " " + "value"    // string literals and concatenation
    b = true or false             // boolean literals
    
    // if/else
    if (i = 1000 or i > 1000 or r < 1000.1) and (b or not r)
        v = 1
    else if s = "some"
        // all variables are in function scope (like old C or JavaScript)
        // no local scopes (yet)
        v = 2
    else
        v = 3
    end
    // return 
    return true
end

// entry point
function run
    b = example_function(1, "2")
    print("Hello, World!")
end

```

# Js Highlighting Test

```js
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
    i = 1000                      // integer literal
    r = 1000.1                    // floating point literal
    s = "some" + " " + "value"    // string literals and concatenation
    b = true or false             // boolean literals
    
    // if/else
    if (i = 1000 or i > 1000 or r < 1000.1) and (b or not r)
        v = 1
    else if s = "some"
        // all variables are in function scope (like old C or JavaScript)
        // no local scopes (yet)
        v = 2
    else
        v = 3
    end
    // return 
    return true
end

// entry point
function run
    b = example_function(1, "2")
    print("Hello, World!")
end

```