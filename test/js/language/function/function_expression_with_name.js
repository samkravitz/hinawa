var obj1 = {};
var obj2 = {};
var obj3 = {};

obj1.named = function add2(x) {
  return x + 2;
};

obj2.anon = function (x) {
  return x + 5;
};

obj3.arrow = (x) => {
  return x + 10;
};

print(obj1);
print(obj2);
print(obj3);
