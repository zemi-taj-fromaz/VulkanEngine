#pragma once

#include "AppImpl.h"
#include <memory>

class Application
{
public:
    Application();
    ~Application();
    void run();

private:
    void initialize_window();
    void initialize_app();
    void main_loop();
    void cleanup();

private:
    std::unique_ptr<AppImpl> m_AppImpl;
};

