#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	Vec3d p = r.getPosition();

	Vec3d d = r.getDirection();

	double det = (p * d) * (p * d) - p * p + 1.0;

	if (det < 0.0) return false;
	// ���ʎ���0�����Ȃ�������Ȃ�

	double t1 = -(p * d) - sqrt(det);

	double t2 = -(p * d) + sqrt(det);

	double t;

	if (t1 > RAY_EPSILON) t = t1;
	// �덷�΍�̂���RAY_EPSILON���g��

	else if (t2 > RAY_EPSILON) t = t2;

	else return false;

	Vec3d intersect = r.at(t);

	if (intersect * intersect - 1.0 < RAY_EPSILON) { // ���߂����W���P�ʋ��ォ�m�F

		i.obj = this;
		// Ray�ƏՓ˂����v���~�e�B�u�͎������g�Ȃ̂�this���w��

		i.t = t;

		i.N = intersect;

		i.N.normalize();
		// �傫��1�̃x�N�g�������O�̂��ߐ��K��

		return true;

	}

	return false;
	// �P�ʋ��ォ�痣��Ă��܂������͌������Ȃ��Ƃ݂Ȃ�

}

