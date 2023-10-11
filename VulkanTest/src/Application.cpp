#include "Application.h"

Application::Application()
{
    m_AppImpl = std::unique_ptr<AppImpl>(AppImpl::create());
}

Application::~Application()
{

}

void Application::run() 
{
    {
        this->initialize_window();
        this->initialize_app();
        this->main_loop();
        this->cleanup();
    }
}



void Application::initialize_window()
{
    m_AppImpl->initialize_window();
}

void Application::initialize_app()
{
    m_AppImpl->initialize_app();
}

void Application::main_loop()
{
    m_AppImpl->main_loop();
}
void Application::cleanup()
{
    m_AppImpl->cleanup();
}

