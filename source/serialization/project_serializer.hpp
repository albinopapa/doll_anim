#pragma once

#include "../model/project.hpp"

#include <filesystem>

namespace animtool::serialization
{

class ProjectSerializer
{
public:
    static bool Save(const model::Project& project, const std::filesystem::path& path);
    static bool Load(model::Project& project, const std::filesystem::path& path);
};

class AnimationExporter
{
public:
    static bool Export(const model::Project& project, const std::filesystem::path& path);
};

} // namespace animtool::serialization
