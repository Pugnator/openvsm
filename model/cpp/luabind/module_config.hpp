#pragma once
#include <string>
#include <vector>
#include <map>

struct Resource
{
  const char *data;
  size_t size;
};

extern const std::vector<std::string> modulesToPreload;
extern const std::map<std::string, Resource> resourceMap;