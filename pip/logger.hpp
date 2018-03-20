#pragma once

class log : public cc::diagnostic
{
public:
  log(const std::string& str)
    : cc::diagnostic(cc::dk_warn, cc::location(), str)
  { }
};
