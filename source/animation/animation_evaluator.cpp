#include "animation_evaluator.hpp"

#include <algorithm>
#include <cmath>

namespace animtool::animation
{
namespace
{
float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float SmoothStep(float t)
{
    return t * t * (3.0f - (2.0f * t));
}

float InterpAngleShortest(float a, float b, float t)
{
    constexpr float kPi = 3.14159265359f;
    constexpr float kTwoPi = 6.28318530718f;
    float d = std::fmod((b - a), kTwoPi);
    if (d > kPi) d -= kTwoPi;
    if (d < -kPi) d += kTwoPi;
    return a + (d * t);
}
} // namespace

Pose AnimationEvaluator::SampleBoneAtFrame(const model::Bone& bone, const model::Animation& animation, float frame)
{
    Pose base{bone.localPosition, bone.localRotationRadians, bone.localScale};

    auto itTrack = std::find_if(animation.boneTracks.begin(), animation.boneTracks.end(), [&](const model::BoneTrack& t) {
        return t.boneId == bone.id;
    });
    if (itTrack == animation.boneTracks.end() || itTrack->keyframes.empty()) return base;

    const auto& keys = itTrack->keyframes;
    if (keys.size() == 1) return Pose{keys.front().localPosition, keys.front().localRotationRadians, keys.front().localScale};

    const model::BoneKeyframe* prev = &keys.front();
    const model::BoneKeyframe* next = &keys.back();
    for (const auto& key : keys)
    {
        if (key.frame <= frame) prev = &key;
        if (key.frame >= frame)
        {
            next = &key;
            break;
        }
    }

    if (prev->frame == next->frame)
        return Pose{prev->localPosition, prev->localRotationRadians, prev->localScale};

    float t = (frame - static_cast<float>(prev->frame)) / static_cast<float>(next->frame - prev->frame);
    if (next->interpolation == model::InterpolationMode::Step) t = 0.0f;
    if (next->interpolation == model::InterpolationMode::SmoothStep) t = SmoothStep(t);

    Pose out{};
    out.position.x = Lerp(prev->localPosition.x, next->localPosition.x, t);
    out.position.y = Lerp(prev->localPosition.y, next->localPosition.y, t);
    out.scale.x = Lerp(prev->localScale.x, next->localScale.x, t);
    out.scale.y = Lerp(prev->localScale.y, next->localScale.y, t);
    out.rotationRadians = InterpAngleShortest(prev->localRotationRadians, next->localRotationRadians, t);
    return out;
}

std::unordered_map<uint32_t, Pose> AnimationEvaluator::EvaluateLocalPose(const model::Project& project, float frame)
{
    std::unordered_map<uint32_t, Pose> result;
    for (const auto& bone : project.bones) result.emplace(bone.id, SampleBoneAtFrame(bone, project.animation, frame));
    return result;
}

std::unordered_map<uint32_t, DirectX::XMMATRIX> AnimationEvaluator::EvaluateWorldTransforms(
    const model::Project& project,
    const std::unordered_map<uint32_t, Pose>& localPose)
{
    using namespace DirectX;
    std::unordered_map<uint32_t, XMMATRIX> world;

    for (const auto& bone : project.bones)
    {
        const Pose p = localPose.at(bone.id);
        XMMATRIX local = XMMatrixScaling(p.scale.x, p.scale.y, 1.0f) *
                         XMMatrixRotationZ(p.rotationRadians) *
                         XMMatrixTranslation(p.position.x, p.position.y, 0.0f);

        if (bone.parentId.has_value() && world.contains(*bone.parentId)) world[bone.id] = local * world.at(*bone.parentId);
        else world[bone.id] = local;
    }
    return world;
}

} // namespace animtool::animation
