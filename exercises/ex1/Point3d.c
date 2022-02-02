// Edison Chau
// edichau@uw.edu

// Copyright 2022 Edison Chau

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "Point3d.h"

Point3d* Point3d_Allocate(int32_t x, int32_t y, int32_t z) {
    Point3d* ptr = (Point3d*) malloc(sizeof(Point3d));

    if (ptr == NULL) {
        return NULL;
    }
    ptr->x = x;
    ptr->y = y;
    ptr->z = z;
    return ptr;

    free(ptr);
}

void Point3d_Scale(Point3d* point, int32_t scale) {
    if (point != NULL) {
        point->x *= scale;
        point->y *= scale;
        point->z *= scale;
    }
}

Point3d* Point3d_GetOrigin() {
    return Point3d_Allocate(0, 0, 0);
}
