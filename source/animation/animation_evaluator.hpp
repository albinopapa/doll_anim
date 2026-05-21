#pragma once

#include "../model/project.hpp"

#include <DirectXMath.h>

#include <unordered_map>

namespace animtool::animation
{

struct Pose
{
    DirectX::XMFLOAT2 position{};
    float rotationRadians{};
    DirectX::XMFLOAT2 scale{1.0f, 1.0f};
};

class AnimationEvaluator
{
public:
    [[nodiscard]] static std::unordered_map<uint32_t, Pose> EvaluateLocalPose(const model::Project& project, float frame);
    [[nodiscard]] static std::unordered_map<uint32_t, DirectX::XMMATRIX> EvaluateWorldTransforms(
        const model::Project& project,
        const std::unordered_map<uint32_t, Pose>& localPose);

private:
    [[nodiscard]] static Pose SampleBoneAtFrame(const model::Bone& bone, const model::Animation& animation, float frame);
};

} // namespace animtool::animation
