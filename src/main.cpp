#include <Windows.h>
#include <winrt/base.h>
#include <winrt/Windows.Data.Json.h>
#include <string>
#include <dwmapi.h>
#include "wrl.h"
#include <iostream>

#include "./utils/utils.h"
#include "./auth/auth.h"
#include "./webview/webview.h"

#define HTML_RC_ID 101
#define WEBVIEW2_STATIC_LIB

HWND g_hwnd;
std::optional<SpotifyCredentials> g_credentials;
std::optional<std::string> g_accessToken = std::nullopt;
std::unique_ptr<WebView> g_wv;

void OnSetCredentials(SetCredentialsMessage *msg)
{
    auto newCreds = g_credentials.value_or(SpotifyCredentials{});

    newCreds.clientId = wtos(msg->clientId.c_str());
    newCreds.clientSecret = wtos(msg->clientSecret.c_str());

    SaveCredentials(newCreds);
    g_credentials = newCreds;

    g_wv->NavigateToIndex();
}

void OnGotAuthCode(std::wstring *authCodePtr)
{
    auto code = wtos(*authCodePtr);
    GetAccessTokenAsync(g_hwnd, g_credentials.value(), &code);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    switch (wmsg)
    {
    case WM_WV2_SET_CREDENTIALS:
        OnSetCredentials((SetCredentialsMessage *)lParam);
        break;
    case WM_WV2_GOT_AUTH_CODE:
        if (!g_credentials)
            break;
        OnGotAuthCode((std::wstring *)lParam);
        break;
    case WM_AUTH_GOT_ACCESS_TOKEN:
        g_accessToken = *(std::string *)lParam;
        g_wv->NavigateToIndex();
    case WM_SIZE:
        RECT newBounds;
        GetClientRect(hwnd, &newBounds);
        g_wv->Resize(newBounds);
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, wmsg, wParam, lParam);
}

std::string LoadHTML()
{
    HRSRC res = FindResource(nullptr, MAKEINTRESOURCE(HTML_RC_ID), RT_RCDATA);
    if (!res)
        return "";

    HGLOBAL data = LoadResource(nullptr, res);
    if (!data)
        return "";

    DWORD size = SizeofResource(nullptr, res);
    void *resPtr = LockResource(data);

    std::string asciiHTML((char *)resPtr, size);

    return asciiHTML;
}

std::wstring GetIndexURL()
{
    std::string newUrl("minispot://mainapp/");

    if (g_credentials)
    {
        newUrl += "?client_id=";
        newUrl += g_credentials->clientId;

        if (g_accessToken)
        {
            newUrl += "&access_token=";
            newUrl += g_accessToken.value();
        }
    }

    return stow(newUrl);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    // makes it not blurry
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    const wchar_t MAIN_CLASS_NAME[] = L"minispot";
    WNDCLASSW wc = {};
    wc.hInstance = hInstance;
    wc.lpszClassName = MAIN_CLASS_NAME;
    wc.lpfnWndProc = WindowProc;

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP, MAIN_CLASS_NAME, L"minispot",
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 512, 256,
        nullptr, nullptr, hInstance, nullptr);

    g_credentials = LoadCredentials();
    if (g_credentials && g_credentials->refreshToken)
    {
        GetAccessTokenAsync(g_hwnd, g_credentials.value());
    }

    g_wv = std::make_unique<WebView>(LoadHTML(), g_hwnd, GetIndexURL);
    g_wv->Initialize();

    ShowWindow(g_hwnd, nCmdShow);
    DWORD backdrop = DWMSBT_TRANSIENTWINDOW;
    DwmSetWindowAttribute(g_hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));

    MSG wmsg;
    while (GetMessage(&wmsg, nullptr, 0, 0))
    {
        TranslateMessage(&wmsg);
        DispatchMessage(&wmsg);
    }

    CoUninitialize();
    return 0;
}
