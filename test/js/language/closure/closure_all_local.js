function outer() {
  var a = 1;
  var b = 2;
  function middle() {
    var c = 3;
    var d = 4;
    function inner() {
      print(a + b + c + d);
    }

    return inner;
  }

  return middle;
}

var middle = outer();
var inner = middle();
inner();
