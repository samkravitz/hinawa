var x = 10;
x += 2;
print(x);
x /= 3;
print(x);
x *= 20;
print(x);
x -= 3;
print(x);

function takeArr(self) {
  self[0] /= 10;
  self[1] += 24;
  self[2] -= 8;
  self[3] *= 20;
  return self;
}

var arr = [10, 20, 30, 40];
var mutated = takeArr(arr);
print(mutated);
print(arr);
