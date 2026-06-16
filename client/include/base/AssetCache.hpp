#pragma once
#include <string>
#include <unordered_map>

struct Model;

class AssetCache
{
public:
    const Model* getModel(const std::string& modelName);

private:
    std::unordered_map<std::string, Model> m_models;

};
