#pragma once

#include <filesystem>
#include <optional>
#include <fstream>

#include "Windows.h"
#include "winrt/windows.web.http.h"
#include "winrt/windows.web.http.headers.h"
#include "winrt/windows.data.json.h"
#include "winrt/windows.foundation.h"

#include "../utils/utils.h"

constexpr UINT WM_AUTH_GOT_ACCESS_TOKEN = WM_APP + (1 * 10) + 1;

struct SpotifyCredentials
{
    std::string clientId;
    std::string clientSecret;
    std::optional<std::string> refreshToken;
};

std::optional<SpotifyCredentials> LoadCredentials();
void SaveCredentials(SpotifyCredentials &credentials);

winrt::fire_and_forget GetAccessTokenAsync(HWND hwnd, SpotifyCredentials &creds);
winrt::fire_and_forget GetAccessTokenAsync(HWND hwnd, SpotifyCredentials &creds, std::string *authCode);
