# Language Description

### Hello, World!

```cpp
    // one-line comment
    print "Hello, World!";
```

### Data Types

Yare-Yare-Dawa is dynamically typed. 
Variables can store values of any type, and a single variable can even store values of different types at different times. 
If you try to perform an operation on values of the wrong type—say, dividing a number by a string—then the error is detected and reported at runtime.

YYD-lang supports next data types:

- **Booleans**
```cpp
true;  // Not false.
false; // Not *not* false.
```
- **Numbers**.  YYD-lang has only one kind of number: double-precision floating point.
```cpp
1234;  // An integer.
12.34; // A decimal number.
```
- **Strings**.
```cpp
"I am a string";
"";    // The empty string.
"123"; // This is a string, not a number.
```

### Expressions

### Arithmetic

Binary:
```cpp
add + me;
subtract - me;
multiply * me;
divide / me;
```

Unary:
```cpp
-negateMe;
```

### Comparison and equality

```cpp
less < than;
lessThan <= orEqual;
greater > than;
greaterThan >= orEqual;
```

```cpp
1 == 2;         // false.
"cat" != "dog"; // true.

314 == "pi"; // false.
123 == "123"; // false.
```

### Logical operations

```cpp
!true;  // false.
!false; // true.

true and false; // false.
true and true;  // true.

false or false; // false.
true or false;  // true.
```

### Precedence and grouping

```javascript
var average = (min + max) / 2;

var a = 2 + 2 * 3; // 8
var b = (2 + 2) * 3; // 12
```

### Control Flow

```cpp
if (condition) {
  print "yes";
} else {
  print "no";
}
```

```cpp
var a = 1;
while (a < 10) {
  print a;
  a = a + 1;
}
```

```cpp
for (var a = 1; a < 10; a = a + 1) {
  print a;
}
```