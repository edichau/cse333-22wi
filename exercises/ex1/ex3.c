// Edison Chau
// edichau@uw.edu

// Copyright 2022 Edison Chau

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "Point3d.h"

int main() {
    // create origin point
    Point3d* origin = Point3d_GetOrigin();
    if (origin == NULL) {
        perror("Failed to create origin point");
        return EXIT_FAILURE;
    }
    printf("(x: %d, y: %d, z: %d)\n", origin->x, origin->y, origin->z);
    free(origin);

    // create a random point
    Point3d* point = Point3d_Allocate(14, -5, 999);
    if (point == NULL) {
        perror("Failed to create point");
        return EXIT_FAILURE;
    }
    printf("(x: %d, y: %d, z: %d)\n", point->x, point->y, point->z);

    // test scaling
    Point3d_Scale(point, 5);
    printf("(x: %d, y: %d, z: %d)\n", point->x, point->y, point->z);

    Point3d_Scale(point, -1);
    printf("(x: %d, y: %d, z: %d)\n", point->x, point->y, point->z);

    Point3d_Scale(point, 0);
    printf("(x: %d, y: %d, z: %d)\n", point->x, point->y, point->z);

    free(point);
    // testing nulls, nothing should happen
    Point3d_Scale(NULL, 2);
}
