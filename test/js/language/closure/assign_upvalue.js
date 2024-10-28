function outer() {
  var x = "before";
  function inner() {
    x = "assigned";
  }

  inner();
  print(x);
}

outer();
