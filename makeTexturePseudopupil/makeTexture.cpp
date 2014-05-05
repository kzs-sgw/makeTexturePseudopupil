#include "makeTexture.h"
#include <gtc\constants.hpp>


makeTexture::makeTexture(void) : PI(glm::pi<float>()), HalfPI(glm::half_pi<float>())
{
	radius = 1.0f;
	srand(RAND_MAX);
	firstCrossPoint = radius * normalize( vec3( rand(), rand(), rand() ) );
	printVec( firstCrossPoint );
	getchar();
	centerSphere = vec3( 0.0, 0.0, 0.0 );
	nRatio = 1.491;
}


makeTexture::~makeTexture(void)
{
	
}


void makeTexture::saveImageAsBITMAP(const char* filename)
{
	dib = FreeImage_Allocate( 512, 512, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK );

	int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);

	for (unsigned y = 0; y < FreeImage_GetHeight(dib); y++)
	{
		BYTE *bits = FreeImage_GetScanLine(dib, y);

		for (unsigned x = 0; x < FreeImage_GetWidth(dib); x++)
		{
			bits[FI_RGBA_RED]   = 0;
			bits[FI_RGBA_GREEN] = 255;
			bits[FI_RGBA_BLUE]  = 0;
			bits[FI_RGBA_ALPHA] = 128;

			// jump to next pixel
			bits += bytespp;
		}
	}

	if ( FreeImage_Save(FIF_BMP, dib, filename, 0) )
	{
		std::cout<<filename<<" successfully saved!"<<std::endl;
	}

	if (dib)
	{
		FreeImage_Unload(dib);
		std::cout<<"unload"<<std::endl;
	}
}


void makeTexture::calcColor()
{
	dib = FreeImage_Allocate( 512, 512, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK );

	int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);

	float phi   = 0;
	float theta = 0;
	float step_phi   = HalfPI / (float)FreeImage_GetHeight(dib);
	float step_theta = HalfPI / (float)FreeImage_GetWidth(dib);

	
	vec3 Normal = vec3(0.0,0.0,1.0); // 平面の法線ベクトル

	float eta = 1.0 / nRatio;
	int over_count = 0;

	for (unsigned y = 0; y < FreeImage_GetHeight(dib); y++)
	{
		BYTE *bits = FreeImage_GetScanLine(dib, y);

		//getchar();
		theta = 0;
		for (unsigned x = 0; x < FreeImage_GetWidth(dib); x++)
		{ 
			// phi,thetaにおける光の道筋を計算する。
			vec3 incident = vec3( sin(theta)*cos(phi), sin(theta)*sin(phi), -cos(theta) ); // 入射ベクトル
			//printVec( incident );
			//std::cout<<"th"<<theta<<std::endl;
			//std::cout<<"ph"<<phi<<std::endl;

			//====centerSphereを更新して繰り返す=====================

			// 屈折ベクトルの計算
			vec3 lightPathVec = refract( incident, Normal, eta );
			
			vec3 cPoint = lineSphereCrossPointPlus( lightPathVec, firstCrossPoint, centerSphere, radius );
			
			lightPathVec = refract( lightPathVec, normalize(cPoint - centerSphere), 1.0f/eta ); // 全反射でなければ外にでるベクトル

			// 球との衝突判定（3つの球に対しておこなう必要）
			// 同一球の場合とそれ以外とでPlus、Minusが変わる

			if ( isCrossing ( lightPathVec, cPoint, centerSphere, radius ) )
			{
				// 同一球の場合
				cPoint = lineSphereCrossPointPlus( lightPathVec, cPoint, centerSphere, radius );
			}
			else
			{
				over_count++;
				// 近傍球の探索
				// lightPathVec.xyを2つのベクトルで表した時の係数を計算し、比較する
				vec2 cef = mat2( 1.0, 0.0, -1.0f/sqrt(3), 2.0f/sqrt(3) ) * vec2(lightPathVec);
				printVec( cef ); 

				// 近傍球の判定---------------------------------------------------
				int judge_dir;
				if		( cef.x>=0 && cef.y>=0 )							{judge_dir = 0; std::cout<<"0"<<std::endl;}
				else if ( cef.x<0  && cef.y>=0 && abs(cef.x)<abs(cef.y) )	{judge_dir = 1; std::cout<<"1"<<std::endl;}
				else if ( cef.x<0  && cef.y>=0 && abs(cef.x)>=abs(cef.y) )	{judge_dir = 2; std::cout<<"2"<<std::endl;}
				else if ( cef.x<0  && cef.y<0 )								{judge_dir = 3; std::cout<<"3"<<std::endl;}
				else if ( cef.x>=0 && cef.y<0 && abs(cef.x)<abs(cef.y) )	{judge_dir = 4; std::cout<<"4"<<std::endl;}
				else if ( cef.x>=0 && cef.y<0 && abs(cef.x)>=abs(cef.y) )	{judge_dir = 5; std::cout<<"5"<<std::endl;}
				//----------------------------------------------------------------
				mat4 m;
				m = glm::rotate( 60.0 * judge_dir, 0.0, 0.0, 1.0 ); // angle_in_degrees
				vec3 nearSphere0 = centerSphere + vec3( m * vec4( 1.0, 0.0, 0.0, 1.0 ) );
				vec3 nearSphere1 = centerSphere + vec3( m * vec4( 0.5, sqrt(3)/2.0, 0.0, 1.0 ) );
				
				//printVec( nearSphere0 );
				//printVec( nearSphere1 );
				std::cout<<std::endl;
				//if ( isCrossing() )
				{

				}

			}

			//======================================================



			bits[FI_RGBA_RED]   = 0;
			bits[FI_RGBA_GREEN] = 255;
			bits[FI_RGBA_BLUE]  = 0;
			bits[FI_RGBA_ALPHA] = 128;

			// jump to next pixel
			bits += bytespp;
			// tilt incident
			theta += step_theta;
		}
		phi += step_phi;
	}
	cout<<"over_count = "<<over_count<<endl;

}


void makeTexture::setRatio( float value )
{
	nRatio = value;
}


vec3 makeTexture::refract( vec3 I, vec3 N, float eta ) // eta = 1.0/nRatio
{
	vec3 result;
	float k = 1.0 - eta*eta*( 1.0-dot(N,I) * dot(N,I) );
	if ( k < 0.0 ) // 全反射
		result = reflect(I,N);
	else 
		result = eta * I - ( eta * dot(N,I) + sqrt(k) ) * N;

	return result;
}


vec3 makeTexture::reflect( vec3 I, vec3 N )
{
	return I - 2.0f * dot(N,I) * N;
}


vec3 makeTexture::lineSphereCrossPointPlus( vec3 v, vec3 x, vec3 c, float r )
{
	float D = pow( dot(v,x-c), 2.0) - dot(v,v) * ( dot(x-c,x-c) - r*r );
	float t = ( -dot(v,x-c) + sqrt(D) ) / dot(v,v);

	return x + t*v;	
}


vec3 makeTexture::lineSphereCrossPointMinus( vec3 v, vec3 x, vec3 c, float r )
{
	float D = pow( dot(v,x-c), 2.0) - dot(v,v) * ( dot(x-c,x-c) - r*r );
	float t = ( -dot(v,x-c) - sqrt(D) ) / dot(v,v);

	return x + t*v;	
}


vec2 makeTexture::leastSquaresMethod( vec3 _v03, vec3 _v10, vec3 _v20 )
{
	/* v3 = v0 + a*v10 + b*v20 */
	vec2 result;
	float adbc = dot(_v10,_v10)*dot(_v20,_v20) - dot(_v10,_v20)*dot(_v20,_v10);
	result.x = ( dot(_v20,_v20)*dot(_v03,_v10)-dot(_v20,_v10)*dot(_v03,_v20) ) / adbc;
	result.y = ( dot(_v10,_v10)*dot(_v03,_v20)-dot(_v10,_v20)*dot(_v03,_v10) ) / adbc;
	
	return result;
}


bool makeTexture::isCrossing(vec3 v, vec3 x, vec3 c, float r )
{
	float D = pow( dot(v,x-c), 2.0) - dot(v,v) * ( dot(x-c,x-c) - r*r );
	if ( D >= 0.0 ) 
		return true;
	else
		return false;
}