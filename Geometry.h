#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>        //sqrt, acos..

namespace Geometry{
    // 二つの座標を配列で受け取って、その座標の距離を返す
    template <typename T>
    T distanceBetweenPoints(T* x1, T* x2){
        T dx=x1[0]-x2[0];
        T dy=x1[1]-x2[1];
        T dz=x1[2]-x2[2];
        return static_cast<T> (std::sqrt(dx*dx+dy*dy+dz*dz));
    }

    // 二つのベクトルを配列で受け取り、そのなす角を返す
    template <typename T>
    T angleBetweenSegments(T* a, T* b){
        // 内積
        T dotProduct= a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
        // ベクトルの大きさ
        T magnitude_a=static_cast<T>(std::sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]));
        T magnitude_b=static_cast<T>(std::sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]));
        // なす角を返す
        return static_cast<T>(std::acos(dotProduct/(magnitude_a*magnitude_b)));
    }

}

#endif