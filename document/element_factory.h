#pragma once

#include <memory>

#include "document.h"

std::shared_ptr<Element> create_element(Document &, const std::string &);
std::shared_ptr<Element> create_element(const std::string &);
