#include "webview.h"

using namespace Microsoft::WRL;

WebView::WebView(std::string html, HWND &hwnd, std::function<std::wstring()> indexUrlFn)
    : html(html), hwnd(hwnd), GetIndexURL(indexUrlFn)
{
    controller = std::nullopt;
}

ComPtr<CoreWebView2EnvironmentOptions> CreateOptions()
{
    auto options = Make<CoreWebView2EnvironmentOptions>();
    // idk if these are all necessary but it saves like 120mb of RAM
    options->put_AdditionalBrowserArguments(
        L"--disable-features=AudioServiceOutOfProcess,WebRtcHWAEC,"
        L"Extensions,TranslateUI,BackForwardCache,MediaRouter,Autofill,RendererCodeIntegrity"
        L" --disable-gpu --disable-software-rasterizer --disable-gpu-compositing --disable-gpu-sandbox"
        L" --disable-extensions"
        L" --disable-component-extensions-with-background-pages"
        L" --disable-background-timer-throttling"
        L" --disable-renderer-backgrounding"
        L" --disable-client-side-phishing-detection"
        L" --disable-sync"
        L" --disable-features=msSmartScreenProtection"
        L" --no-default-browser-check"
        L" --no-first-run"
        L" --disable-logging"
        L" --disable-dev-shm-usage"
        L" --disable-breakpad"
        L" --disable-crash-reporter"
        L" --process-per-site");
    ComPtr<ICoreWebView2EnvironmentOptions4> options4;
    options.As(&options4);

    ComPtr<ICoreWebView2CustomSchemeRegistration> scheme =
        Make<CoreWebView2CustomSchemeRegistration>(L"minispot");
    scheme->put_TreatAsSecure(TRUE);
    scheme->put_HasAuthorityComponent(TRUE);

    const WCHAR *allowedOrigins[1];
    allowedOrigins[0] = L"minispot://mainapp/";
    scheme->SetAllowedOrigins(1, allowedOrigins);

    ICoreWebView2CustomSchemeRegistration *schemes[1] = {scheme.Get()};
    options4->SetCustomSchemeRegistrations(1, schemes);

    return options;
}

winrt::fire_and_forget WebView::Initialize()
{
    auto options = CreateOptions();
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> env = co_await CoCreateCoreWebView2EnvironmentWithOptions(options.Get());
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> controller = co_await CoCreateCoreWebView2Controller(hwnd, env);
    this->controller = controller;

    RECT winBounds;
    GetClientRect(hwnd, &winBounds);
    controller->put_Bounds(winBounds);

    // otherwise translucent window background is hidden
    ComPtr<ICoreWebView2Controller2> controller2;
    controller->QueryInterface(IID_PPV_ARGS(&controller2));
    controller2->put_DefaultBackgroundColor({0, 0, 0, 0});

    ComPtr<ICoreWebView2> wv;
    controller->get_CoreWebView2(&wv);
    wv->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(this, &WebView::WebMessageReceived)
            .Get(),
        nullptr);

    SetupNetworkHandling(wv, env);
    NavigateToIndex();
}

HRESULT WebView::WebMessageReceived(ICoreWebView2 *wv, ICoreWebView2WebMessageReceivedEventArgs *args)
{
    LPWSTR rawMessage;
    args->get_WebMessageAsJson(&rawMessage);

    auto json = winrt::Windows::Data::Json::JsonObject::Parse(rawMessage);
    if (static_cast<int>(json.GetNamedNumber(L"message_id")) == WEB_MESSAGE_ID_SET_CREDENTIALS)
    {
        auto clientId = json.GetNamedString(L"client_id");
        auto clientSecret = json.GetNamedString(L"client_secret");
        SetCredentialsMessage msg = SetCredentialsMessage{clientId, clientSecret};
        SendMessage(hwnd, WM_WV2_SET_CREDENTIALS, 0, (LPARAM)&msg);
    }

    CoTaskMemFree(rawMessage);
    return S_OK;
}

HRESULT WebView::SetupNetworkHandling(ComPtr<ICoreWebView2> wv, ComPtr<ICoreWebView2Environment> env)
{
    ComPtr<ICoreWebView2_22> webview22;
    wv->QueryInterface(IID_PPV_ARGS(&webview22));
    if (!webview22)
        return E_FAIL;

    webview22->AddWebResourceRequestedFilterWithRequestSourceKinds(
        L"*",
        COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL,
        COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL);

    webview22->add_WebResourceRequested(
        Callback<ICoreWebView2WebResourceRequestedEventHandler>(
            [this, env](ICoreWebView2 *wv, ICoreWebView2WebResourceRequestedEventArgs *args)
            {
                ComPtr<ICoreWebView2WebResourceRequest> request;
                args->get_Request(&request);

                LPWSTR urlBuf = nullptr;
                request->get_Uri(&urlBuf);
                std::wstring url(urlBuf);
                CoTaskMemFree(urlBuf);

                if (url.find(L"mainapp") != std::wstring::npos)
                {
                    IStream *stream = SHCreateMemStream(
                        reinterpret_cast<const BYTE *>(html.data()),
                        static_cast<UINT>(html.size()));

                    ComPtr<ICoreWebView2WebResourceResponse> resp;
                    env->CreateWebResourceResponse(stream, 200, L"OK", L"Content-Type: text/html\r\n", &resp);
                    args->put_Response(resp.Get());

                    stream->Release();
                }
                else if (url.find(L"http://127.0.0.1:20956") != std::wstring::npos)
                {
                    auto authCode = url.substr(29);
                    SendMessage(hwnd, WM_WV2_GOT_AUTH_CODE, 0, (LPARAM)&authCode);
                }

                return S_OK;
            })
            .Get(),
        nullptr);

    return S_OK;
}

void WebView::NavigateToIndex()
{
    if (!controller)
        return;
    ComPtr<ICoreWebView2> wv;
    controller.value()->get_CoreWebView2(&wv);
    auto newUrl = GetIndexURL();
    wv->Navigate(newUrl.c_str());
}

void WebView::Resize(RECT &bounds)
{
    if (!controller)
        return;
    controller.value()->put_Bounds(bounds);
}
