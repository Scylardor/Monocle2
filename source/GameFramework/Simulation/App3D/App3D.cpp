#include "App3D.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"


moe::App3D::App3D(int argc, char** argv)
{
	Add<ConfigService>(argc, argv);
}


void moe::App3D::Start()
{
}


void moe::App3D::Update()
{
}


void moe::App3D::Finish()
{
}
