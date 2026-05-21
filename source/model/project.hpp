#pragma once

#include <DirectXMath.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace animtool::model
{
enum class InterpolationMode
{
    Step,
    Linear,
    SmoothStep,
};

struct VertexWeight
{
    uint32_t boneId{};
    float weight{};
};

struct MeshVertex
{
    DirectX::XMFLOAT2 position{};
    DirectX::XMFLOAT2 uv{};
    std::vector<VertexWeight> weights;
};

struct Mesh
{
    uint32_t id{};
    uint32_t cutoutId{};
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
};

struct Cutout
{
    uint32_t id{};
    std::string name;
    DirectX::XMFLOAT4 sourceRect{};
    DirectX::XMFLOAT2 pivot{};
    uint32_t boundBoneId{};
    std::optional<uint32_t> meshId;
};

struct Bone
{
    uint32_t id{};
    std::string name;
    std::optional<uint32_t> parentId;
    std::vector<uint32_t> children;

    DirectX::XMFLOAT2 localPosition{};
    float localRotationRadians{};
    DirectX::XMFLOAT2 localScale{1.0f, 1.0f};
    float length{32.0f};

    DirectX::XMFLOAT3 editorColor{1.0f, 1.0f, 1.0f};
    bool visible{true};
    bool locked{false};
};

struct BoneKeyframe
{
    uint32_t frame{};
    DirectX::XMFLOAT2 localPosition{};
    float localRotationRadians{};
    DirectX::XMFLOAT2 localScale{1.0f, 1.0f};
    InterpolationMode interpolation{InterpolationMode::Linear};
};

struct BoneTrack
{
    uint32_t boneId{};
    std::vector<BoneKeyframe> keyframes;
};

struct Animation
{
    std::string name{"default"};
    uint32_t frameCount{60};
    float framesPerSecond{12.0f};
    std::vector<BoneTrack> boneTracks;
};

struct ProjectSettings
{
    DirectX::XMFLOAT2 cameraPan{};
    float cameraZoom{1.0f};
    bool showGrid{true};
    bool showBones{true};
};

struct Project
{
    std::wstring projectFilePath;
    std::wstring spriteSheetPath;
    std::vector<Cutout> cutouts;
    std::vector<Bone> bones;
    std::vector<Mesh> meshes;
    Animation animation;
    ProjectSettings settings;
};

} // namespace animtool::model
