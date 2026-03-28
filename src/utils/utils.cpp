#include "utils.h"

std::string Base64Encode(const std::string &input)
{
    DWORD size = 0;
    CryptBinaryToStringA(
        reinterpret_cast<const BYTE *>(input.data()),
        input.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        nullptr,
        &size);

    std::string output(size - 1, '\0');

    CryptBinaryToStringA(
        reinterpret_cast<const BYTE *>(input.data()),
        input.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        output.data(),
        &size);

    return output;
}

// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string URLEncode(std::string &value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i < n; ++i)
    {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }

        if (c == '\0')
            continue;

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

std::string wtos(std::wstring wstr)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (len == 0)
        return "";
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), len, NULL, NULL);
    return str;
}

std::wstring stow(std::string mbstr)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, mbstr.c_str(), -1, NULL, 0);
    if (len == 0)
        return L"";

    std::wstring wstr(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, mbstr.c_str(), -1, wstr.data(), len);
    return wstr;
}
