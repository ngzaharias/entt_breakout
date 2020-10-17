#include "Application.h"

Application::Application() 
{ 
}

Application::~Application() 
{ 
}

void Application::Run(int agrc, char* argv[])
{
	if (!Initialise(agrc, argv))
		return;

	while (m_isForceQuitting == false)
	{
		if (!Update()) 
			break;

		if (!Draw()) 
			break;
	}
	Destroy();
}

void Application::Quit()
{
	m_isForceQuitting = true;
}