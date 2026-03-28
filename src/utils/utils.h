#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include "Windows.h"
#include "wincrypt.h"

std::string Base64Encode(const std::string &input);
std::string URLEncode(std::string &value);

std::string wtos(std::wstring wstr);
std::wstring stow(std::string mbstr);
