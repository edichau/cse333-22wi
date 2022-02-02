// Edison Chau
// edichau@uw.edu

// Copyright 2022 Edison Chau

#ifndef POINT3D_H_
#define POINT3D_H_

#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} Point3d;

// accepts three int32_t arguments, mallocs space for a Point3d,
// assigns the three arguments to the x, y, and z fields and and
// returns (a pointer to) the malloc'ed Point3d
Point3d* Point3d_Allocate(int32_t x, int32_t y, int32_t z);

// accepts one Point3d* and one int32_t value as arguments,
// scales the x, y, and z fields of the pointed-to struct by the given value,
// and returns nothing.
void Point3d_Scale(Point3d* point, int32_t scale);

// constructs a Point3d with x, y, and z equal to zero, and returns a copy of
// the struct.
Point3d* Point3d_GetOrigin();

#endif  // POINT3D_H_
