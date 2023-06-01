var arr = [];
print(arr);

// push 1 element
arr.push({});
print(arr);

// push multiple elements
arr.push(5, 4, 3, 2, 1, []);
print(arr);

// push returns the new size of the array
print(arr.length);
var newLength = arr.push("three", "new", "elements");
print(newLength);

// calling push with no arguments does not alter the array
// and simply returns the array's size
print(arr.push());
print(arr);
