#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	Vec3d p = r.getPosition();

	Vec3d d = r.getDirection();

	double det = (p * d) * (p * d) - p * p + 1.0;

	if (det < 0.0) return false;
	// 判別式が0未満なら交差しない

	double t1 = -(p * d) - sqrt(det);

	double t2 = -(p * d) + sqrt(det);

	double t;

	if (t1 > RAY_EPSILON) t = t1;
	// 誤差対策のためRAY_EPSILONを使う

	else if (t2 > RAY_EPSILON) t = t2;

	else return false;

	Vec3d intersect = r.at(t);

	if (intersect * intersect - 1.0 < RAY_EPSILON) { // 求めた座標が単位球上か確認

		i.obj = this;
		// Rayと衝突したプリミティブは自分自身なのでthisを指定

		i.t = t;

		i.N = intersect;

		i.N.normalize();
		// 大きさ1のベクトルだが念のため正規化

		return true;

	}

	return false;
	// 単位球上から離れてしまった時は交差しないとみなす

}

