function outer() {
  var x = "outside";
  function inner() {
    print(x);
  }
  inner();
}

outer();
