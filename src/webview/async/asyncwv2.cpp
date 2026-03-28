#include "asyncwv2.h"

CoCreateCoreWebView2EnvironmentWithOptions::CoCreateCoreWebView2EnvironmentWithOptions(
    ICoreWebView2EnvironmentOptions *environmentOptions)
    : environmentOptions(environmentOptions) {};

bool CoCreateCoreWebView2EnvironmentWithOptions::await_ready() { return false; }
bool CoCreateCoreWebView2EnvironmentWithOptions::await_suspend(std::coroutine_handle<> h)
{
    CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, environmentOptions,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this, h](HRESULT hr, ICoreWebView2Environment *env)
            {
                ret = env;
                h.resume();
                return S_OK;
            })
            .Get());
    return true;
}

Microsoft::WRL::ComPtr<ICoreWebView2Environment> CoCreateCoreWebView2EnvironmentWithOptions::await_resume()
{
    return ret.value();
}

CoCreateCoreWebView2Controller::CoCreateCoreWebView2Controller(
    HWND hwnd, Microsoft::WRL::ComPtr<ICoreWebView2Environment> env) : hwnd(hwnd), env(env) {};

bool CoCreateCoreWebView2Controller::await_ready() { return false; }
bool CoCreateCoreWebView2Controller::await_suspend(std::coroutine_handle<> h)
{
    env->CreateCoreWebView2Controller(
        hwnd,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this, h](HRESULT result, ICoreWebView2Controller *controller)
            {
                ret = controller;
                h.resume();
                return S_OK;
            })
            .Get());
    return true;
}

Microsoft::WRL::ComPtr<ICoreWebView2Controller> CoCreateCoreWebView2Controller::await_resume()
{
    return ret.value();
}
