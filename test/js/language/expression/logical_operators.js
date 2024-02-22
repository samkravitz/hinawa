// operator || with bools
print(true || true);
print(true || false);
print(false || true);
print(false || false);

// operator && with bools
print(true && true);
print(true && false);
print(false && true);
print(false && false);

// operator || with non-bools
print("foo" || "");
print(undefined || {});
print(null || []);

// operator && with non-bools
print(undefined && {});
print(null && []);

// operator && short circuits if lhs is false
print(false && null.ptr());

// operator || short circuits if lhs is true
print(true || null.ptr());
