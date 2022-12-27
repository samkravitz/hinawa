#pragma once

#include <functional>
#include <vector>

#include "url.h"
#include <util/hinawa.h>

void load(const Url &, std::function<void(const std::vector<u8> &)>);
