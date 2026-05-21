#include "../animation/animation_evaluator.hpp"
#include "../model/project.hpp"
#include "../serialization/project_serializer.hpp"

#include <windows.h>

using namespace animtool;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    model::Project project{};
    project.animation.name = "demo";
    project.spriteSheetPath = L"demo.png";

    model::Bone root{};
    root.id = 1;
    root.name = "root";
    root.localPosition = {320.0f, 240.0f};
    project.bones.push_back(root);

    model::BoneTrack track{};
    track.boneId = 1;
    track.keyframes.push_back(model::BoneKeyframe{0, {320.0f, 240.0f}, -0.25f, {1.0f, 1.0f}, model::InterpolationMode::Linear});
    track.keyframes.push_back(model::BoneKeyframe{15, {320.0f, 240.0f}, 0.25f, {1.0f, 1.0f}, model::InterpolationMode::SmoothStep});
    project.animation.boneTracks.push_back(track);

    const auto pose = animation::AnimationEvaluator::EvaluateLocalPose(project, 8.0f);
    (void)pose;

    serialization::ProjectSerializer::Save(project, "sample.skelproj");
    serialization::AnimationExporter::Export(project, "sample.skelanim");

    MessageBoxW(nullptr,
                L"AnimationTool core data model and serialization are initialized.\n"
                L"Use this as the foundation for the full Win32 + Direct2D editor UI.",
                L"AnimationTool",
                MB_OK | MB_ICONINFORMATION);
    return 0;
}
