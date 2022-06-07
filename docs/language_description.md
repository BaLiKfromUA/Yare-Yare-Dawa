# Language Description

### Hello, World!

```cpp
    // one-line comment
    print "Hello, World!";
```

### Expressions

#### Arithmetic

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

#### Comparison and equality

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

#### Logical operations

```cpp
!true;  // false.
!false; // true.

true and false; // false.
true and true;  // true.

false or false; // false.
true or false;  // true.
```

#### Precedence and grouping

```javascript
var average = (min + max) / 2;

var a = 2 + 2 * 3; // 8
var b = (2 + 2) * 3; // 12
```

### Data Types
YYD-lang supports next data types:

- **Booleans**
```cpp
true;  // Not false.
false; // Not *not* false.
```
- **Numbers**.  YYD-lang has only one kind of number: double-precision floating point. 
All basic arithmetic operations are supported
```cpp
1234;  // An integer.
12.34; // A decimal number.

2 + 2 * 3; // 8
(2 + 2) * 3; // 12
(3 - 2) / 2; // 0.5

-(42); // -42
```
- **Strings**. 
String concatenation and multiplication are supported. 
In order to get string length use built-in function `len(str)`
```javascript
"I am a string";
"";    // The empty string.
"123"; // This is a string, not a number.

"balik" + "_" + "mem"// "balik_mem"
                
"a" * 5;// "aaaaa"
3 * "b";// "bbb"

len("balik"); // 5
len("mem"); // 3
```

### Control Flow

#### If statement

```javascript
if (condition) {
  print "else branch is optional";
} 

if (condition) {
  print "yes";
} else {
  print "no";
}
```

#### While loop

```javascript
var a = 1;
while (a < 10) {
  print a;
  a = a + 1;
}
```

#### For loop

```javascript
for (var a = 1; a < 10; a = a + 1) {
  print a;
}
```
