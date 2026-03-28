#include "auth.h"

// file relative to the executable path
std::filesystem::path CredentialsPath()
{
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath(buffer);
    return exePath.parent_path() / "credentials.ini";
}

std::optional<SpotifyCredentials> LoadCredentials()
{
    char buf[64]{};
    std::string filepath = wtos(CredentialsPath());
    DWORD len;
    SpotifyCredentials creds;

    len = GetPrivateProfileStringA("Provided", "client_id", "", buf, sizeof(buf), filepath.c_str());
    if (len > 0)
    {
        creds.clientId = buf;
        len = GetPrivateProfileStringA("Provided", "client_secret", "", buf, sizeof(buf), filepath.c_str());
        if (len > 0)
            creds.clientSecret = buf;
        else
            return std::nullopt;
    }
    else
    {
        return std::nullopt;
    }

    len = GetPrivateProfileStringA("Extra", "refresh_token", "", buf, sizeof(buf), filepath.c_str());
    if (len > 0)
        creds.refreshToken = buf;
    else
        creds.refreshToken = std::nullopt;

    return creds;
}

void SaveCredentials(SpotifyCredentials &credentials)
{
    std::string filepath = wtos(CredentialsPath());

    WritePrivateProfileStringA("Provided", "client_id", credentials.clientId.c_str(), filepath.c_str());
    WritePrivateProfileStringA("Provided", "client_secret", credentials.clientSecret.c_str(), filepath.c_str());
    WritePrivateProfileStringA("Extra", "refresh_token", credentials.refreshToken.value_or("").c_str(), filepath.c_str());
}

winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>
GetAccessToken(SpotifyCredentials &creds, std::optional<std::string *> authCode)
{
    winrt::Windows::Web::Http::HttpClient client;

    std::string authHeader = creds.clientId + ":" + creds.clientSecret;
    std::string authHeader64 = Base64Encode(authHeader);
    auto authHeader64H = winrt::to_hstring(authHeader64);

    client.DefaultRequestHeaders().Append(L"Authorization", L"Basic " + authHeader64H);

    std::wstring body;

    body += L"grant_type=";
    if (authCode)
        body += L"authorization_code&code=" + winrt::to_hstring(*authCode.value());
    else
        body += L"refresh_token&refresh_token=" + winrt::to_hstring(creds.refreshToken.value());

    body += L"&redirect_uri=";
    body += winrt::Windows::Foundation::Uri::EscapeComponent(L"http://127.0.0.1:20956/");

    winrt::Windows::Web::Http::HttpStringContent content(body);
    content.Headers().ContentType(
        winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue(L"application/x-www-form-urlencoded"));

    winrt::Windows::Web::Http::HttpResponseMessage response;
    try
    {
        winrt::Windows::Foundation::Uri uri{L"https://accounts.spotify.com/api/token"};
        response = co_await client.PostAsync(uri, content);
        response.EnsureSuccessStatusCode();
    }
    catch (winrt::hresult_error const &ex)
    {
        winrt::hstring err = ex.message();
    }

    auto responseText = co_await response.Content().ReadAsStringAsync();

    auto json = winrt::Windows::Data::Json::JsonObject::Parse(responseText);
    if (!creds.refreshToken)
    {
        auto refreshToken = json.GetNamedString(L"refresh_token");
        creds.refreshToken = wtos(refreshToken.c_str());
    }

    winrt::hstring accessToken = json.GetNamedString(L"access_token");
    co_return accessToken;
}

winrt::fire_and_forget GetAccessTokenAsync(HWND hwnd, SpotifyCredentials &creds)
{
    winrt::hstring val = co_await GetAccessToken(creds, std::nullopt);
    std::string str = wtos(val.c_str());
    SendMessage(hwnd, WM_AUTH_GOT_ACCESS_TOKEN, 0, (LPARAM)&str);
}

winrt::fire_and_forget GetAccessTokenAsync(HWND hwnd, SpotifyCredentials &creds, std::string *authCode)
{
    winrt::hstring val = co_await GetAccessToken(creds, authCode);
    std::string str = wtos(val.c_str());
    SendMessage(hwnd, WM_AUTH_GOT_ACCESS_TOKEN, 0, (LPARAM)&str);
}
