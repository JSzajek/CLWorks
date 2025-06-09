#include "BlitTextureShaders.h"

IMPLEMENT_GLOBAL_SHADER(FBlitTextureShadersCS, "/CLShaders/BlitTexture_CS.usf", "MainCS", SF_Compute);