#pragma once

#include "object.h"

namespace js
{
class Error : public Object
{
public:
	virtual const char *name() const { return "Error"; }
};

class ReferenceError final : public Error
{
public:
	virtual const char *name() const override { return "ReferenceError"; }
};

class TypeError final : public Error
{
public:
	virtual const char *name() const override { return "TypeError"; }
};
}