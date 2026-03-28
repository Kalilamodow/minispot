#pragma once

#include <string>
#include <Shlwapi.h>
#include <optional>
#include <functional>

#include "Windows.h"
#include "wrl.h"
#include "winrt/base.h"
#include "winrt/windows.data.json.h"

#include "./async/asyncwv2.h"

#include "WebView2.h"
#include "WebView2EnvironmentOptions.h"

constexpr UINT WM_WV2_SET_CREDENTIALS = WM_APP + (0 * 10) + 1;
constexpr UINT WM_WV2_GOT_AUTH_CODE = WM_APP + (0 * 10) + 2;
using namespace Microsoft::WRL;

enum WEB_MESSAGE_IDS
{
    WEB_MESSAGE_ID_SET_CREDENTIALS = 0,
};

class WebView
{
private:
    std::string html;
    std::optional<ComPtr<ICoreWebView2Controller>> controller;
    HWND &hwnd;
    std::function<std::wstring()> GetIndexURL;
    HRESULT SetupNetworkHandling(ComPtr<ICoreWebView2> wv, ComPtr<ICoreWebView2Environment> env);
    HRESULT WebMessageReceived(ICoreWebView2 *wv, ICoreWebView2WebMessageReceivedEventArgs *args);

public:
    WebView(std::string html, HWND &hwnd, std::function<std::wstring()> indexUrlFn);
    winrt::fire_and_forget Initialize();
    void Resize(RECT &bounds);
    void NavigateToIndex();
};

struct SetCredentialsMessage
{
    winrt::hstring clientId;
    winrt::hstring clientSecret;
};
