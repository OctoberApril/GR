#pragma once

class Noncopy
{
public:
	Noncopy() = default;
	~Noncopy() = default;

private:
	Noncopy(const Noncopy&) = delete;
	Noncopy(Noncopy&&) = delete;
	Noncopy& operator=(const Noncopy&) = delete;
	Noncopy& operator=(Noncopy&&) = delete;
};