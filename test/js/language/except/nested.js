function first() {
  function second(a) {
    function third(b) {
      function fourth(c) {
        throw c;
      }
      fourth(b + 1);
    }
    third(a + 1);
  }
  second(1);
}

try {
  first();
} catch (e) {
  print(e);
}
