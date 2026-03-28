// coroutine wrappers for some wv2 oneshot callbacks.
#pragma once

#include <coroutine>

#include "Windows.h"
#include "wrl.h"
#include "winrt/windows.foundation.h"

#include "WebView2.h"
#include "WebView2EnvironmentOptions.h"

class CoCreateCoreWebView2EnvironmentWithOptions
{
public:
    CoCreateCoreWebView2EnvironmentWithOptions(
        ICoreWebView2EnvironmentOptions *environmentOptions);

    bool await_ready();
    bool await_suspend(std::coroutine_handle<> h);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> await_resume();

private:
    ICoreWebView2EnvironmentOptions *environmentOptions;
    std::optional<Microsoft::WRL::ComPtr<ICoreWebView2Environment>> ret;
};

class CoCreateCoreWebView2Controller
{
public:
    CoCreateCoreWebView2Controller(HWND hwnd, Microsoft::WRL::ComPtr<ICoreWebView2Environment> env);

    bool await_ready();
    bool await_suspend(std::coroutine_handle<> h);
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> await_resume();

private:
    HWND hwnd;
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> env;
    std::optional<Microsoft::WRL::ComPtr<ICoreWebView2Controller>> ret;
};
