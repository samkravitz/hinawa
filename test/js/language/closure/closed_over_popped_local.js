function outer() {
  var x = "value";

  function middle() {
    function inner() {
      print(x);
    }

    print("create inner closure");
    return inner;
  }

  print("return from outer");
  return middle;
}

var mid = outer();
var inn = mid();

inn();
