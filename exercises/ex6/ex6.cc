// Copyright 2022 <Edison Chau>
#include <iostream>
#include <string>
#include "Vector.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

// Verifys if p and v are the same
static bool VerifyAddress(const Vector& v, void *p);

// Adds 1 to each variable in v
static void Increment(Vector v);

int main() {
    Vector v1;
    cout << "(" << v1.get_x() << ", " << v1.get_y() << ", "
        << v1.get_z() << ")" << endl;

    Vector v2(1, -3, 2);
    cout << "(" << v2.get_x() << ", " << v2.get_y() << ", "
        << v2.get_z() << ")" << endl;

    Vector v3(v2);
    cout << "(" << v3.get_x() << ", " << v3.get_y() << ", "
        << v3.get_z() << ")" << endl;

    v1 = v2;
    cout << "(" << v1.get_x() << ", " << v1.get_y() << ", "
        << v1.get_z() << ")" << endl;

    v1 += v2;
    cout << "(" << v1.get_x() << ", " << v1.get_y() << ", "
        << v1.get_z() << ")" << endl;

    Vector v4(1, 2, 3);
    Increment(v4);
    if (v4.get_x() != 1 || v4.get_y() != 2 || v4.get_z() != 3) {
        cout << "Vector: pass-by-reference" << endl;
    } else {
        cout << "Vector: pass-by-value" << endl;
  }

    if (VerifyAddress(v4, &v4)) {
        cout << "Ref: same address" << endl;
    } else {
        cout << "Ref: different address" << endl;
    }

  return EXIT_SUCCESS;
}

void Increment(Vector v) {
  Vector v1(1, 1, 1);
  v += v1;
}

bool VerifyAddress(const Vector& v, void *p) {
  return &v == reinterpret_cast<Vector*>(p);
}
