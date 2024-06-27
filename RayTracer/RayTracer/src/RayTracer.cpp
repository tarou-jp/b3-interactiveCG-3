// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
Vec3d RayTracer::trace( double x, double y )
{
	// Clear out the ray cache in the scene for debugging purposes,
	scene->intersectCache.clear();

    ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY );

    scene->getCamera().rayThrough( x,y,r );
	Vec3d ret = traceRay( r, Vec3d(1.0,1.0,1.0), 0 );
	ret.clamp();
	return ret;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
Vec3d RayTracer::traceRay(const ray& r, const Vec3d& thresh, int depth) {

	isect i;
	// 入射Rayと物体との交点情報

	if (scene->intersect(r, i)) {
		// 入射Rayの交差判定

		const Material& m = i.getMaterial();
		// 交差した物体のパラメータ取得用

		Vec3d ret = m.shade(scene, r, i);
		// 返り値の初期値にシェーディングの値を入れる

		if (depth < traceUI->getDepth()) {
			// 再帰深度とスライダーの値を比較

			Vec3d kr = m.kr(i);
			// 反射の係数

			Vec3d kt = m.kt(i);
			// 透過の係数

			Vec3d d = r.getDirection();

			Vec3d norm = i.N;

			if (d * i.N > 0.0) {

				norm = -norm; // 物体内からのRayの場合

			}
			// reflection

			Vec3d reflect(0.0, 0.0, 0.0); // 光源から反射する鏡面光の値

			Vec3d dd = -(norm * d) * norm + d;
			Vec3d rd = -d + 2 * dd;

			rd.normalize();

			if (!(kr.iszero())) { // 反射係数が0かチェック

				ray rr(r.at(i.t - RAY_EPSILON), rd, ray::REFLECTION);

				reflect = traceRay(rr, thresh, depth + 1);
				// 深度を深めて再帰計算

				ret += prod(kr, reflect);

			}

			// transmission

			Vec3d transmit(0.0, 0.0, 0.0);
			// 屈折するRayの返り値

			double n1 = 1.0;
			double n2 = m.index(i);
			Vec3d refractDir;
			double cosI = -d * norm;
			double sinT2 = (n1 / n2) * (n1 / n2) * (1.0 - cosI * cosI);

			if (sinT2 <= 1.0)
			{
				double cosT = sqrt(1.0 - sinT2);
				refractDir = (n1 / n2) * d + (cosT - (n1 / n2) * cosI) * norm;
				refractDir.normalize();
				ray rt(r.at(i.t + RAY_EPSILON), refractDir, ray::REFRACTION);
				transmit = traceRay(rt, thresh, depth + 1);
			}

			ret += prod(kt, transmit);


		}

		return ret;

	}

	return Vec3d(0.0, 0.0, 0.0);
	// 入射Rayが物体と交差しなかった場合は0

}

RayTracer::RayTracer()
	: scene( 0 ), buffer( 0 ), buffer_width( 256 ), buffer_height( 256 ), m_bBufferReady( false )
{

}


RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn )
{
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos )
		path = ".";
	else
		path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}


	if( ! sceneLoaded() )
		return false;



	// Initialize the scene's BSP tree
	scene->initBSPTree();


	
	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];


	}
	memset( buffer, 0, w*h*3 );
	m_bBufferReady = true;
}

void RayTracer::tracePixel( int i, int j )
{
	Vec3d col;

	if( ! sceneLoaded() )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);


	col = trace( x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}


