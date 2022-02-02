// Copyright 2022 <Edison Chau>
#include "Vector.h"

#include <iostream>

using std::ostream;

Vector& Vector::operator=(const Vector& v) {
  // Replace state of 'this' with values from v
  if (this != &v) {
    x_ = v.x_;
    y_ = v.y_;
    z_ = v.z_;
  }
  // Return reference to lhs of assignment
  return *this;
}

Vector& Vector::operator+=(const Vector& v) {
  x_ += v.x_;
  y_ += v.y_;
  z_ += v.z_;
  return *this;
}

Vector& Vector::operator-=(const Vector& v) {
  x_ -= v.x_;
  y_ -= v.y_;
  z_ -= v.z_;
  return *this;
}

// operator*
float Vector::operator*(const Vector& v) const {
  return x_ * v.x_ + y_ * v.y_ + z_ * v.z_;
}
