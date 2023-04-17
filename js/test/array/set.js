function f() {}

var arr = [f, {}];
print(arr);

arr[0] = 0;
var idx = 1;
arr[idx] = 2;
print(arr);
