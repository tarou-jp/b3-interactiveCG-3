#include <cmath>

#include "light.h"



using namespace std;

double DirectionalLight::distanceAttenuation(const Vec3d& P) const
{
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation(const Vec3d& P) const
{
	Vec3d d = getDirection(P);	// �����ւ̕����x�N�g�����Ƃ�
	d.normalize();
	ray r(P, d, ray::SHADOW);	// P����d�֐i�ށA�e�̔���p��Ray�𐶐�
	isect i;			// ��_����ۑ����邽�߂̃I�u�W�F�N�g�𐶐�

	if (getScene()->intersect(r, i)) 	// Ray���V�[����̉��炩�̃I�u�W�F�N�g�ƏՓ˂��邩����
		return Vec3d(0.0, 0.0, 0.0);	// �Փ˂���Ȃ�0��Ԃ�
	else
		return Vec3d(1.0, 1.0, 1.0);	// �Փ˂��Ȃ����1��Ԃ�
}

Vec3d DirectionalLight::getColor(const Vec3d& P) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d DirectionalLight::getDirection(const Vec3d& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const Vec3d& P) const
{

	Vec3d dv = this->position - P;	// �_�����̈ʒu�x�N�g���ƌ�_�̈ʒu�x�N�g���̍����Ƃ�
	double d = dv.length();	// ��̃x�N�g���̃T�C�Y���Ƃ��Č�������̋������v�Z
	double f = 1.0 / (constantTerm + linearTerm * d + quadraticTerm * d * d);

	if (f > 1.0)
		f = 1.0;
	else if (f <= 0.0)
		f = 0.0;

	return f;
}

Vec3d PointLight::getColor(const Vec3d& P) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d PointLight::getDirection(const Vec3d& P) const
{
	Vec3d ret = position - P;
	ret.normalize();
	return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
	Vec3d d = getDirection(P);	// �����ւ̕����x�N�g�����Ƃ�
	d.normalize();
	ray r(P, d, ray::SHADOW);	// P����d�֐i�ށA�e�̔���p��Ray�𐶐�
	isect i;			// ��_����ۑ����邽�߂̃I�u�W�F�N�g�𐶐�
	Vec3d dv = position - P;	// �����̈ʒu�x�N�g���ƌ�_�̈ʒu�x�N�g���̍����Ƃ�
	double dist = dv.length();

	if (getScene()->intersect(r, i)) // Ray���V�[����̉��炩�̃I�u�W�F�N�g�ƏՓ˂��邩����
	{
		Vec3d dv1 = r.at(i.t) - P;  //�Փ˃I�u�W�F�N�g�Ƃ̋�����, dist��������->�����̌��ŏՓ˂���̂ŎՂ��Ȃ�
		if (dv1.length() > dist)
			return Vec3d(1.0, 1.0, 1.0);
		else
			return Vec3d(0.0, 0.0, 0.0);// �Փ˂���Ȃ�0��Ԃ�
	}
	else {
		return Vec3d(1.0, 1.0, 1.0);// �Փ˂��Ȃ����1��Ԃ�
	}
}