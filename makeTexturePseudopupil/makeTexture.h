#pragma once
//#include "TextureManager.h"
#include <math.h>
#include "FreeImage.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>
#include <iostream>

#pragma comment(lib, "FreeImage.lib")

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat2;
using glm::mat3;
using glm::mat4;
using glm::dot;
using glm::normalize;

using namespace std;

class makeTexture
{
private:
	float		nRatio; // ã¸ê‹ó¶
	void printVec( vec2 v ){ std::cout<<v.x<<", "<<v.y<<std::endl; }
	void printVec( vec3 v ){ std::cout<<v.x<<", "<<v.y<<", "<<v.z<<std::endl; }
public:
	makeTexture(void);
	~makeTexture(void);

	void saveImageAsBITMAP(const char* filename);
	void calcColor();
	void setRatio( float value );
	vec3 refract( vec3 I, vec3 N, float eta );
	vec3 reflect( vec3 I, vec3 N );
	vec3 lineSphereCrossPointPlus( vec3 v/*LineDir*/, vec3 x/*a point on the line*/,
								   vec3 c/*center of sphere*/, float r/*radius of sphere*/ );
	vec3 lineSphereCrossPointMinus( vec3 v, vec3 x, vec3 c, float r );
	vec2 leastSquaresMethod( vec3 _v03, vec3 _v10, vec3 _v20 );
	bool isCrossing( vec3 v, vec3 x, vec3 c, float r );

	const float PI;
	const float HalfPI;
	vec3		firstCrossPoint;
	vec3		centerSphere;
	float		radius;

	FIBITMAP	*bitmap;
	FIBITMAP	*dib;
	
};

