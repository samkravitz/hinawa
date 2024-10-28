function getFee(isMember) {
  return isMember ? "$2.00" : "$10.00";
}

print(getFee(true));
print(getFee(false));
print(getFee(null));

// ternary operator short circuits
var x = true ? "right!" : null.pointer();
var y = false ? null.pointer() : "right!";

print(x);
print(y);
