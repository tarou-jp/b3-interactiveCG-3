#include <cmath>

#include "light.h"



using namespace std;

double DirectionalLight::distanceAttenuation(const Vec3d& P) const
{
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation(const Vec3d& P) const
{
	Vec3d d = getDirection(P);	// 光源への方向ベクトルをとる
	d.normalize();
	ray r(P, d, ray::SHADOW);	// Pからdへ進む、影の判定用のRayを生成
	isect i;			// 交点情報を保存するためのオブジェクトを生成

	if (getScene()->intersect(r, i)) 	// Rayがシーン上の何らかのオブジェクトと衝突するか判定
		return Vec3d(0.0, 0.0, 0.0);	// 衝突するなら0を返す
	else
		return Vec3d(1.0, 1.0, 1.0);	// 衝突しなければ1を返す
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

	Vec3d dv = this->position - P;	// 点光源の位置ベクトルと交点の位置ベクトルの差をとる
	double d = dv.length();	// 上のベクトルのサイズをとって光源からの距離を計算
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
	Vec3d d = getDirection(P);	// 光源への方向ベクトルをとる
	d.normalize();
	ray r(P, d, ray::SHADOW);	// Pからdへ進む、影の判定用のRayを生成
	isect i;			// 交点情報を保存するためのオブジェクトを生成
	Vec3d dv = position - P;	// 光源の位置ベクトルと交点の位置ベクトルの差をとる
	double dist = dv.length();

	if (getScene()->intersect(r, i)) // Rayがシーン上の何らかのオブジェクトと衝突するか判定
	{
		Vec3d dv1 = r.at(i.t) - P;  //衝突オブジェクトとの距離が, distよりも長い->光源の後ろで衝突するので遮られない
		if (dv1.length() > dist)
			return Vec3d(1.0, 1.0, 1.0);
		else
			return Vec3d(0.0, 0.0, 0.0);// 衝突するなら0を返す
	}
	else {
		return Vec3d(1.0, 1.0, 1.0);// 衝突しなければ1を返す
	}
}