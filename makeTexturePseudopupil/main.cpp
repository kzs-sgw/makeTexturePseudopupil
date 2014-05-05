// PxUserApp class inherits "kzsGLObject" class and "kzsPhysXFramework" class.
// 
//
//

//#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "makeTexture.h"

using namespace std;


//************************************************************
// **main**
int main( int argc, char* argv[] )
{
	makeTexture tex;
	//tex.saveImageAsBITMAP("texturetest.bmp");
	tex.calcColor();

	getchar();
	return 0;
}