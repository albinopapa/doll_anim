#include "project_serializer.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace animtool::serialization
{
namespace
{
const char* ToInterp(const model::InterpolationMode mode)
{
    switch (mode)
    {
    case model::InterpolationMode::Step: return "Step";
    case model::InterpolationMode::SmoothStep: return "SmoothStep";
    default: return "Linear";
    }
}

model::InterpolationMode ParseInterp(const std::string& s)
{
    if (s == "Step") return model::InterpolationMode::Step;
    if (s == "SmoothStep") return model::InterpolationMode::SmoothStep;
    return model::InterpolationMode::Linear;
}
} // namespace

bool ProjectSerializer::Save(const model::Project& project, const std::filesystem::path& path)
{
    std::ofstream out(path);
    if (!out) return false;

    out << "SkelProjectVersion 0.1\n";
    out << "SpriteSheet \"" << std::string(project.spriteSheetPath.begin(), project.spriteSheetPath.end()) << "\"\n";
    out << "FrameCount " << project.animation.frameCount << "\n";
    out << "FPS " << project.animation.framesPerSecond << "\n";

    for (const auto& c : project.cutouts)
    {
        out << "Cutout " << c.id << ' ' << c.name << ' ' << c.sourceRect.x << ' ' << c.sourceRect.y << ' ' << c.sourceRect.z
            << ' ' << c.sourceRect.w << ' ' << c.pivot.x << ' ' << c.pivot.y << ' ' << c.boundBoneId << '\n';
    }

    for (const auto& b : project.bones)
    {
        out << "Bone " << b.id << ' ' << b.name << ' ';
        if (b.parentId) out << *b.parentId;
        else out << "none";
        out << ' ' << b.localPosition.x << ' ' << b.localPosition.y << ' ' << b.localRotationRadians << ' ' << b.localScale.x << ' '
            << b.localScale.y << ' ' << b.length << '\n';
    }

    for (const auto& t : project.animation.boneTracks)
        for (const auto& k : t.keyframes)
            out << "Keyframe " << t.boneId << ' ' << k.frame << ' ' << k.localPosition.x << ' ' << k.localPosition.y << ' '
                << k.localRotationRadians << ' ' << k.localScale.x << ' ' << k.localScale.y << ' ' << ToInterp(k.interpolation) << '\n';

    return true;
}

bool ProjectSerializer::Load(model::Project& project, const std::filesystem::path& path)
{
    std::ifstream in(path);
    if (!in) return false;

    project = {};
    std::string token;
    while (in >> token)
    {
        if (token == "SkelProjectVersion")
        {
            std::string v; in >> v;
        }
        else if (token == "SpriteSheet")
        {
            std::string p;
            in >> std::quoted(p);
            project.spriteSheetPath = std::wstring(p.begin(), p.end());
        }
        else if (token == "FrameCount") in >> project.animation.frameCount;
        else if (token == "FPS") in >> project.animation.framesPerSecond;
        else if (token == "Cutout")
        {
            model::Cutout c{};
            in >> c.id >> c.name >> c.sourceRect.x >> c.sourceRect.y >> c.sourceRect.z >> c.sourceRect.w >> c.pivot.x >> c.pivot.y >> c.boundBoneId;
            project.cutouts.push_back(c);
        }
        else if (token == "Bone")
        {
            model::Bone b{};
            std::string parent;
            in >> b.id >> b.name >> parent >> b.localPosition.x >> b.localPosition.y >> b.localRotationRadians >> b.localScale.x >> b.localScale.y >> b.length;
            if (parent != "none") b.parentId = static_cast<uint32_t>(std::stoul(parent));
            project.bones.push_back(b);
        }
        else if (token == "Keyframe")
        {
            uint32_t boneId;
            model::BoneKeyframe k{};
            std::string interp;
            in >> boneId >> k.frame >> k.localPosition.x >> k.localPosition.y >> k.localRotationRadians >> k.localScale.x >> k.localScale.y >> interp;
            k.interpolation = ParseInterp(interp);
            auto it = std::find_if(project.animation.boneTracks.begin(), project.animation.boneTracks.end(), [&](const model::BoneTrack& t) { return t.boneId == boneId; });
            if (it == project.animation.boneTracks.end())
            {
                project.animation.boneTracks.push_back(model::BoneTrack{boneId, {k}});
            }
            else
            {
                it->keyframes.push_back(k);
            }
        }
    }

    return true;
}

bool AnimationExporter::Export(const model::Project& project, const std::filesystem::path& path)
{
    std::ofstream out(path);
    if (!out) return false;

    out << "skelanim_version 0.1\n";
    out << "animation_name " << project.animation.name << "\n";
    out << "frame_count " << project.animation.frameCount << "\n";
    out << "frames_per_second " << project.animation.framesPerSecond << "\n";
    out << "sprite_sheet " << std::string(project.spriteSheetPath.begin(), project.spriteSheetPath.end()) << "\n";
    out << "bones " << project.bones.size() << "\n";
    for (const auto& b : project.bones)
    {
        out << b.id << ' ' << b.name << ' ';
        if (b.parentId) out << *b.parentId;
        else out << -1;
        out << ' ' << b.localPosition.x << ' ' << b.localPosition.y << ' ' << b.localRotationRadians << ' ' << b.localScale.x << ' ' << b.localScale.y << '\n';
    }

    return true;
}

} // namespace animtool::serialization
