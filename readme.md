### Post Pascal

[readme in html (with proper syntax highlighting)](readme.html)

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

// more examples in 'examples' and 'compiler/test' folders
```



