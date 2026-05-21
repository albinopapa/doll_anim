# 2D Skeletal Cutout Animation Utility — Design Document

## 1. Overview

This utility is a Windows desktop application for creating 2D skeletal animation from sprite cutouts. Users import a sprite sheet, define cutout regions, create a hierarchy of bones, bind cutouts and optional 2D deformation meshes to bones, paint vertex weights, pose bones with keyframes, preview animation through interpolation, and export animation data and images.

The application is written in modern C++ and uses only Windows platform APIs:

* **Win32 SDK** for windowing, input, menus, dialogs, and application shell.
* **Direct2D** for rendering the editor UI, sprite cutouts, mesh overlays, bones, and previews.
* **WIC** for loading and saving images.
* **DirectXMath** for vectors, matrices, quaternions, transforms, and interpolation math.

No third-party libraries are required.

## 2. Goals

The utility should allow users to:

* Create and edit 2D skeletal rigs for sprite cutouts.
* Build a hierarchical bone tree for a character or object body.
* Assign sprite cutouts to bones.
* Create flat 2D meshes over cutouts using manually placed vertices.
* Paint vertex weights so cutouts can warp when bones bend.
* Pose bones using rotation controls.
* Create keyframes on a timeline.
* Interpolate between keyframes during playback and scrubbing.
* Zoom and pan the preview camera.
* Save and load project files.
* Export the current frame as an image.
* Export animation data containing bones, rotations, cutout regions, timing, and sprite sheet references.

## 3. Non-Goals

The initial version does not need to support:

* Full inverse kinematics.
* Physics simulation.
* 3D rendering.
* GPU skinning.
* Audio timelines.
* Plugin systems.
* External UI frameworks.
* Runtime game-engine integration beyond exported data files.

These may be considered future extensions.

## 4. Target Platform and Technology

### 4.1 Language

* C++20 or C++23.
* Prefer RAII wrappers for COM resources.
* Use `std::filesystem` for paths.
* Use `std::vector`, `std::string`, `std::wstring`, `std::optional`, `std::variant`, and `std::unordered_map` where appropriate.

### 4.2 Platform APIs

| Area              | Technology                                            |
| ----------------- | ----------------------------------------------------- |
| Windowing         | Win32 SDK                                             |
| UI controls       | Custom immediate or retained UI over Win32 + Direct2D |
| Rendering         | Direct2D                                              |
| Image loading     | WIC                                                   |
| Image saving      | WIC                                                   |
| Math              | DirectXMath                                           |
| File dialogs      | Win32 common dialogs or COM file dialogs              |
| Clipboard support | Win32 clipboard APIs                                  |

### 4.3 Build System

The project shall use a Visual Studio 2022 solution exclusively.

Requirements:

- Visual Studio 2022.
- MSVC compiler toolchain.
- Windows SDK.
- x64 target platform.
- Debug and Release configurations.

The project shall not use:

- CMake.
- Ninja.
- Meson.
- Premake.
- Third-party build systems.

The repository should contain:

```text
/AnimationTool.sln
/source
/resources
/docs
```
All project configuration shall be maintained directly through Visual Studio project files (.sln, .vcxproj, .filters, and associated property sheets where appropriate).

### 4.4 Development Environment Constraints

To keep onboarding simple and reduce maintenance overhead, the project intentionally targets a single development environment.

Requirements:

- Visual Studio 2022 is the authoritative build environment.
- Project settings are managed through `.vcxproj` files.
- All generated files must be compatible with Visual Studio 2022.
- Solution and project files are considered source-controlled assets.

Restrictions:

- No CMakeLists.txt files.
- No generated Visual Studio projects from external tools.
- No cross-platform build abstractions.
- No dependency on package managers for core functionality.

Rationale:
The application is Windows-only and relies heavily on Win32, Direct2D, WIC, and DirectXMath. Maintaining a Visual Studio-native workflow simplifies project management and reduces build-system complexity.

## 5. Application Layout

The main window is divided into four major regions:

```text
+-------------------------------------------------------------+
| Menu / Toolbar                                               |
+--------------------+---------------------------+------------+
| Bone Tree Pane     | Preview Pane              | Parameters |
|                    |                           | Pane       |
| - Body             | - Sprite cutouts          |            |
|   - Torso          | - Bones                   | Selected   |
|     - Upper Arm    | - Mesh vertices           | Bone       |
|       - Forearm    | - Weight overlays         | Rotation   |
|                    | - Camera zoom/pan         | Slider     |
+--------------------+---------------------------+------------+
| Timeline Scrubber / Keyframes / Playback Controls            |
+-------------------------------------------------------------+
| Status Bar                                                   |
+-------------------------------------------------------------+
```

### 5.1 Bone Tree Pane

The bone tree pane displays all bones for the current body in a hierarchical collapsible tree.

Required behavior:

* Display parent-child bone relationships.
* Expand and collapse branches.
* Select a bone.
* Rename bones.
* Add child bone.
* Delete bone.
* Reparent bone by drag/drop or context menu.
* Show visibility and lock toggles where useful.

Recommended context menu actions:

* Add child bone.
* Rename bone.
* Delete bone.
* Duplicate bone.
* Set as root.
* Reparent to selected bone.
* Focus selected bone in preview.

### 5.2 Preview Pane

The preview pane is the main editing viewport.

Required behavior:

* Render sprite sheet cutouts at their posed positions.
* Render bones as editable visual guides.
* Render optional mesh overlays.
* Render vertices, triangles, and weight influence colors.
* Allow selection of bones, vertices, cutouts, and mesh elements.
* Support zooming with the mouse scroll wheel.
* Support panning the camera.
* Support scrubbing preview state from the timeline.

Recommended controls:

| Input             | Action                                                           |
| ----------------- | ---------------------------------------------------------------- |
| Mouse wheel       | Zoom in/out around cursor                                        |
| Middle mouse drag | Pan camera                                                       |
| Right mouse drag  | Pan camera alternative                                           |
| Left click        | Select element                                                   |
| Left drag         | Move selected vertex, bone endpoint, or cutout depending on mode |
| Ctrl + wheel      | Fine zoom                                                        |
| Space + drag      | Temporary pan mode                                               |
| Delete            | Delete selected editable element                                 |

### 5.3 Timeline Pane

The timeline pane controls animation playback and keyframes.

Required behavior:

* Set total frame count.
* Set framerate.
* Scrub current frame.
* Show keyframes for selected bone or all bones.
* Add, move, copy, paste, and delete keyframes.
* Playback from current frame.
* Stop playback.
* Jump to previous/next keyframe.

Recommended controls:

* Current frame field.
* Total frames field.
* Framerate field.
* Play/stop button.
* Previous frame / next frame buttons.
* Previous keyframe / next keyframe buttons.
* Auto-key toggle.
* Loop playback toggle.

### 5.4 Parameter Pane

The parameter pane displays editable properties for the selected item.

For selected bone:

* Bone name.
* Parent bone.
* Local position.
* Local rotation slider.
* Local scale.
* Length.
* Visibility.
* Lock state.
* Bound cutout reference.
* Keyframe controls.

Required slider:

* A rotation slider for the selected bone.
* Slider should update the selected bone rotation immediately.
* If auto-key is enabled, adjusting the slider creates or updates a keyframe on the current frame.

Recommended rotation range:

* Default: `-180°` to `180°`.
* Optional extended mode: `-360°` to `360°`.
* Store internally as radians.
* Display in degrees.

For selected mesh vertex:

* Position.
* Weight list.
* Primary bone influence.
* Weight value.

For selected cutout:

* Sprite sheet file.
* Source rectangle.
* Pivot.
* Bound bone.
* Mesh enabled/disabled.

## 6. Editing Modes

The preview pane should support explicit editing modes to keep input behavior predictable.

### 6.1 Select Mode

Used for general selection and transform editing.

Features:

* Select bones.
* Select cutouts.
* Select vertices.
* Move selected items if unlocked.

### 6.2 Bone Mode

Used to create and edit skeletons.

Features:

* Add root bone.
* Add child bone from selected bone.
* Move bone start/end points.
* Adjust bone length.
* Reparent bones.
* Rename bones.

### 6.3 Cutout Mode

Used to define and assign sprite sheet cutouts.

Features:

* Load sprite sheet.
* Create source rectangles.
* Name cutouts.
* Set cutout pivot.
* Assign cutout to selected bone.
* Preview cutout placement.

### 6.4 Mesh Mode

Used to create a flat 2D mesh over a cutout.

Features:

* Place vertices using the cutout as a visual guide.
* Create edges.
* Create triangles.
* Delete vertices or triangles.
* Move vertices.
* Snap vertices to pixel grid if desired.
* Show triangulation overlay.

Initial implementation can require manual triangle creation. A later version may add automatic triangulation.

### 6.5 Weight Paint Mode

Used to paint bone weights onto mesh vertices.

Features:

* Select active bone influence.
* Paint weights onto vertices.
* Adjust brush radius.
* Adjust brush strength.
* Normalize weights.
* Smooth weights.
* Clear weights.
* Show color overlay for selected bone influence.

Since the mesh is vertex-based, the first version may paint weights directly onto vertices rather than pixels.

### 6.6 Animate Mode

Used to create poses and keyframes.

Features:

* Select bone.
* Adjust rotation using preview gizmo or parameter slider.
* Create keyframes.
* Scrub timeline.
* Preview interpolation.
* Playback animation.

## 7. Core Data Model

### 7.1 Project

```cpp
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
```

### 7.2 Cutout

A cutout references a rectangle inside a sprite sheet.

```cpp
struct Cutout
{
    uint32_t id;
    std::string name;
    DirectX::XMFLOAT4 sourceRect; // x, y, width, height in sprite-sheet pixels
    DirectX::XMFLOAT2 pivot;      // local pivot in cutout pixels
    uint32_t boundBoneId;
    std::optional<uint32_t> meshId;
};
```

### 7.3 Bone

Bones form a hierarchy. Each bone stores a local bind transform and animated local transform.

```cpp
struct Bone
{
    uint32_t id;
    std::string name;
    std::optional<uint32_t> parentId;
    std::vector<uint32_t> children;

    DirectX::XMFLOAT2 localPosition;
    float localRotationRadians;
    DirectX::XMFLOAT2 localScale;
    float length;

    DirectX::XMFLOAT3 editorColor;
    bool visible;
    bool locked;
};
```

Although this is a 2D tool, DirectXMath quaternions may still be used for rotation interpolation by treating 2D rotation as rotation around the Z axis.

### 7.4 Mesh

A mesh is a flat 2D deformation surface associated with a cutout.

```cpp
struct Mesh
{
    uint32_t id;
    uint32_t cutoutId;
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices; // triangle indices
};
```

### 7.5 Mesh Vertex

```cpp
struct MeshVertex
{
    DirectX::XMFLOAT2 position; // local cutout-space position
    DirectX::XMFLOAT2 uv;       // normalized sprite-sheet UV or cutout-local UV
    std::vector<VertexWeight> weights;
};
```

### 7.6 Vertex Weight

```cpp
struct VertexWeight
{
    uint32_t boneId;
    float weight; // 0.0 to 1.0
};
```

Weights should usually be normalized so all weights on a vertex sum to `1.0`.

### 7.7 Animation

```cpp
struct Animation
{
    std::string name;
    uint32_t frameCount;
    float framesPerSecond;
    std::vector<BoneTrack> boneTracks;
};
```

### 7.8 Bone Track

```cpp
struct BoneTrack
{
    uint32_t boneId;
    std::vector<BoneKeyframe> keyframes;
};
```

### 7.9 Bone Keyframe

```cpp
struct BoneKeyframe
{
    uint32_t frame;
    DirectX::XMFLOAT2 localPosition;
    float localRotationRadians;
    DirectX::XMFLOAT2 localScale;
    InterpolationMode interpolation;
};
```

Initial implementation may key only rotation. The data model should still allow position and scale so the format does not need to be redesigned later.

```cpp
enum class InterpolationMode
{
    Step,
    Linear,
    SmoothStep
};
```

## 8. Transform and Animation Evaluation

### 8.1 Bone Local Transform

Each bone has a local transform relative to its parent.

Recommended transform order:

```text
Local = Scale * RotationZ * Translation
```

For DirectXMath, this may be built using:

* `XMMatrixScaling`
* `XMMatrixRotationZ`
* `XMMatrixTranslation`

### 8.2 Bone World Transform

```text
WorldBone = LocalBone * WorldParent
```

Root bones use their local transform as world transform.

### 8.3 Keyframe Sampling

For each bone at the current frame:

1. Find the previous and next keyframe.
2. If no keyframes exist, use bind/local default transform.
3. If only one keyframe exists, use that keyframe.
4. If current frame lands exactly on a keyframe, use it directly.
5. Otherwise interpolate based on the keyframe interpolation mode.

### 8.4 Rotation Interpolation

For 2D rotation, simple shortest-path angle interpolation is usually enough.

Quaternions can also be used:

1. Convert 2D Z rotation to quaternion.
2. Use `XMQuaternionSlerp`.
3. Convert result back to matrix.

This keeps the implementation aligned with the DirectXMath requirement and gives a clean path if the project later supports more advanced transform channels.

### 8.5 Mesh Skinning

For each vertex:

```text
SkinnedPosition = sum(weight[i] * TransformByBone[i](BindPosition))
```

The first implementation can perform CPU skinning because meshes are expected to be small and Direct2D is being used for rendering.

Recommended approach:

* Store vertex positions in cutout local space.
* Store each bone bind pose inverse matrix.
* During evaluation, compute final bone skinning matrix:

```text
SkinMatrix = InverseBindBoneWorld * CurrentBoneWorld
```

Then weighted-transform each vertex.

## 9. Rendering Design

### 9.1 Direct2D Resources

Core resources:

* `ID2D1Factory`
* `ID2D1HwndRenderTarget` or device-context-based Direct2D target
* `ID2D1Bitmap` for sprite sheets
* Brushes for UI, bones, mesh overlays, vertices, and selection
* Text rendering support through DirectWrite if labels are needed

DirectWrite is part of the Windows platform stack and is acceptable for text rendering.

### 9.2 Render Passes

Recommended preview render order:

1. Background grid.
2. Sprite cutouts or skinned meshes.
3. Mesh triangle overlay.
4. Mesh vertices.
5. Bone hierarchy overlay.
6. Selection outlines.
7. Gizmos.
8. Tool-specific overlays.

### 9.3 Camera

The preview camera should support:

* Zoom.
* Pan.
* Fit to character.
* Reset view.

```cpp
struct Camera2D
{
    DirectX::XMFLOAT2 pan;
    float zoom;
};
```

Mouse wheel zoom should zoom around the cursor position, not just the center of the viewport. This makes detailed mesh and weight editing much more comfortable.

### 9.4 Pixel Alignment

Because this tool works with sprite cutouts, the renderer should provide options for:

* Nearest-neighbor preview.
* Linear-filtered preview.
* Pixel grid overlay.
* Snap-to-pixel editing.

Nearest-neighbor preview is useful for pixel art. Linear filtering is useful for painted or high-resolution sprites.

## 10. File Formats

The project should have a native save format and a separate export format.

### 10.1 Native Project File

Recommended extension:

```text
.skelproj
```

The project file should contain all editor data needed to resume work:

* Version.
* Sprite sheet path.
* Cutouts.
* Bones.
* Meshes.
* Weights.
* Animation timeline.
* Keyframes.
* Editor camera state.
* UI/editor settings.

A simple custom text format or JSON-like format can be used. Since there are no third-party libraries, avoid depending on external JSON libraries unless a minimal internal serializer is written.

Recommended custom format style:

```text
SkelProjectVersion 0.1
SpriteSheet "characters/hero.png"
FrameCount 60
FPS 12

Cutout 1 "torso" 0 0 64 64 32 32 Bone 1 Mesh 1
Bone 1 "root" Parent none Position 320 180 Rotation 0 Scale 1 1 Length 48
Bone 2 "head" Parent 1 Position 0 -48 Rotation 0 Scale 1 1 Length 24

Keyframe Bone 1 Frame 0 Rotation 0
Keyframe Bone 1 Frame 15 Rotation 0.25
Keyframe Bone 1 Frame 30 Rotation 0
```

The parser should be strict enough to catch invalid files but simple enough to maintain.

### 10.2 Animation Export File

Recommended extension:

```text
.skelanim
```

The exported animation file should contain runtime-focused data:

* Format version.
* Animation name.
* Frame count.
* Framerate.
* Sprite sheet file name.
* Cutout names and source rectangles.
* Cutout pivots.
* Bone hierarchy.
* Bone bind transforms.
* Bone keyframes.
* Bone rotations.
* Optional mesh vertices.
* Optional mesh weights.

The export file should avoid editor-only data such as pane sizes, selection state, undo stack, and viewport camera.

### 10.3 Image Export

Users should be able to export:

* Current frame screenshot.
* Optional transparent-background PNG.
* Optional full animation sprite sheet.
* Optional frame sequence.

WIC should be used for PNG output.

## 11. Commands and Menus

### 11.1 File Menu

* New Project.
* Open Project.
* Save Project.
* Save Project As.
* Import Sprite Sheet.
* Export Current Frame.
* Export Animation File.
* Export Animation Sprite Sheet.
* Exit.

### 11.2 Edit Menu

* Undo.
* Redo.
* Cut.
* Copy.
* Paste.
* Delete.
* Duplicate.
* Preferences.

### 11.3 View Menu

* Zoom In.
* Zoom Out.
* Reset Zoom.
* Fit Character.
* Show Grid.
* Show Bones.
* Show Mesh.
* Show Weights.
* Show Pivots.

### 11.4 Animation Menu

* Add Keyframe.
* Delete Keyframe.
* Previous Keyframe.
* Next Keyframe.
* Play.
* Stop.
* Toggle Loop.
* Toggle Auto-Key.

## 12. Undo/Redo System

Undo and redo are strongly recommended because rigging and weight painting involve many small edits.

Use a command-based undo system:

```cpp
class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void Execute(Project& project) = 0;
    virtual void Undo(Project& project) = 0;
};
```

Recommended undoable actions:

* Add/delete/rename bone.
* Reparent bone.
* Move bone.
* Change bone rotation.
* Add/delete cutout.
* Change cutout source rectangle.
* Move pivot.
* Add/delete/move mesh vertex.
* Add/delete triangle.
* Paint weights.
* Add/delete/move keyframe.

For weight painting, group many brush updates into a single undo command per brush stroke.

## 13. Selection Model

Use a unified selection object:

```cpp
struct Selection
{
    SelectionType type;
    uint32_t id;
    std::optional<uint32_t> subId;
};
```

```cpp
enum class SelectionType
{
    None,
    Bone,
    Cutout,
    Mesh,
    MeshVertex,
    Keyframe
};
```

This keeps the parameter pane and command system simple.

## 14. Hit Testing

Preview hit testing should account for camera transform.

Recommended hit-test priority:

1. Gizmo handles.
2. Mesh vertices.
3. Bones.
4. Cutouts.
5. Mesh triangles.
6. Empty space.

Hit testing should use screen-space tolerances so clicking small bones and vertices remains easy at different zoom levels.

## 15. Validation Rules

The project should validate data before save/export.

Recommended checks:

* Bone IDs are unique.
* Cutout IDs are unique.
* Mesh IDs are unique.
* Bone hierarchy has no cycles.
* Parent bone IDs exist.
* Bound bone IDs exist.
* Cutout source rectangles are inside the sprite sheet.
* Mesh indices are valid.
* Vertex weights reference valid bones.
* Vertex weights are normalized or can be normalized on export.
* Keyframes are sorted by frame.
* Keyframes are inside animation frame range.
* Sprite sheet path is valid.

## 16. Project Architecture

Recommended modules:

```text
/source
  /app
    main.cpp
    win32_app.hpp/.cpp
    main_window.hpp/.cpp
  /platform
    com_ptr.hpp
    file_dialogs.hpp/.cpp
    wic_image_io.hpp/.cpp
  /rendering
    d2d_renderer.hpp/.cpp
    sprite_sheet_renderer.hpp/.cpp
    mesh_renderer.hpp/.cpp
    bone_renderer.hpp/.cpp
  /ui
    ui_context.hpp/.cpp
    panel.hpp/.cpp
    tree_view.hpp/.cpp
    timeline.hpp/.cpp
    slider.hpp/.cpp
    toolbar.hpp/.cpp
  /model
    project.hpp/.cpp
    bone.hpp
    cutout.hpp
    mesh.hpp
    animation.hpp
  /animation
    animation_evaluator.hpp/.cpp
    interpolation.hpp/.cpp
    skinning.hpp/.cpp
  /editor
    editor_state.hpp/.cpp
    selection.hpp/.cpp
    tools.hpp/.cpp
    undo_stack.hpp/.cpp
  /serialization
    project_serializer.hpp/.cpp
    animation_exporter.hpp/.cpp
```
Suggested solution organization:

```text
AnimationTool.sln
|
+-- AnimationTool
|   +-- app
|   +-- platform
|   +-- rendering
|   +-- ui
|   +-- model
|   +-- animation
|   +-- editor
|   +-- serialization
|
+-- Resources
|
+-- Documentation
```

## 17. UI Implementation Strategy

There are two reasonable approaches:

### 17.1 Win32 Controls + Direct2D Preview

Use native Win32 controls for tree view, sliders, buttons, and text fields, while Direct2D handles only the preview pane and custom timeline.

Pros:

* Faster to implement standard controls.
* Native accessibility and keyboard behavior.
* Less custom UI code.

Cons:

* Harder to visually unify the app.
* Timeline may still require custom drawing.
* More layout glue between child windows.

### 17.2 Custom Direct2D UI

Use Direct2D for all panes and controls.

Pros:

* Consistent visual style.
* Easier custom timeline and viewport overlays.
* Full control over layout and rendering.

Cons:

* More code.
* Must implement text boxes, sliders, tree views, focus handling, keyboard navigation, clipping, and scrolling.

Recommended first version:

* Use Win32 controls for menus, file dialogs, and possibly the bone tree.
* Use custom Direct2D for preview and timeline.
* Use a simple custom parameter pane if needed.

This balances implementation cost with custom rendering needs.

## 18. Export Runtime Contract

The exported animation should be easy for a game engine to consume.

Minimum runtime data:

```text
animation_name
frame_count
frames_per_second
sprite_sheet_file
cutouts[]
  id
  name
  source_rect
  pivot
  bound_bone_id
bones[]
  id
  name
  parent_id
  bind_position
  bind_rotation
  bind_scale
  length
tracks[]
  bone_id
  keyframes[]
    frame
    rotation
    interpolation
```

Optional runtime data for mesh deformation:

```text
meshes[]
  cutout_id
  vertices[]
    position
    uv
    weights[]
      bone_id
      weight
  indices[]
```

## 19. Useful Additions Not Explicitly Requested

### 19.1 Onion Skinning

Show previous and next frames faintly in the preview pane. This helps users judge motion arcs and spacing.

### 19.2 Pose Library

Allow users to save named poses such as:

* Idle.
* Walk contact.
* Walk passing.
* Jump start.
* Attack windup.

This speeds up animation authoring.

### 19.3 Mirroring

Support mirroring poses or bones across a vertical axis. Useful for walk cycles and symmetrical characters.

### 19.4 Auto-Key Toggle

When enabled, changing a bone transform automatically creates or updates a keyframe at the current frame.

### 19.5 Transform Gizmos

Add simple viewport gizmos:

* Rotation ring around selected bone.
* Bone endpoint handle.
* Pivot handle for cutouts.

### 19.6 Weight Normalization Tools

Add commands:

* Normalize selected vertices.
* Normalize all mesh weights.
* Prune tiny weights below threshold.
* Limit max influences per vertex.

Recommended max influences per vertex: `4`.

### 19.7 Project Diagnostics Panel

A diagnostics panel can list warnings before export:

* Missing sprite sheet.
* Unweighted vertices.
* Invalid bone references.
* Empty animation tracks.
* Cutouts with no assigned bone.

### 19.8 Autosave and Recovery

Autosave reduces risk of lost rigging work.

Recommended behavior:

* Autosave every few minutes after changes.
* Keep a small number of rotating recovery files.
* Prompt to recover after crash or abnormal exit.

### 19.9 Multiple Animations Per Project

Even if the first export focuses on one animation, the project format should allow multiple animations later:

* Idle.
* Walk.
* Run.
* Jump.
* Attack.

### 19.10 Animation Events

Future export support could include frame events:

* Footstep.
* Attack active.
* Spawn effect.
* Play sound.

These are useful for game integration but can be deferred.

## 20. Suggested Milestones

### Milestone 1 — Window, Rendering, and Image Loading

* Create Win32 application shell.
* Initialize Direct2D.
* Initialize WIC.
* Load sprite sheet.
* Render sprite sheet/cutout in preview pane.
* Implement camera pan and mouse-wheel zoom.

### Milestone 2 — Cutouts and Project Save/Load

* Define cutout rectangles.
* Set cutout pivots.
* Save/load project file.
* Add basic file menu.

### Milestone 3 — Bone Hierarchy

* Add bone data model.
* Add bone tree pane.
* Create, delete, rename, and reparent bones.
* Render bones in preview.
* Select bones.

### Milestone 4 — Bone Parameters and Posing

* Add parameter pane.
* Add rotation slider.
* Update selected bone rotation.
* Compute local/world transforms.
* Attach cutouts to bones.

### Milestone 5 — Timeline and Keyframes

* Add timeline scrubber.
* Set frame count and framerate.
* Add keyframes.
* Interpolate rotations.
* Preview playback.

### Milestone 6 — Mesh Creation

* Place mesh vertices over cutouts.
* Create triangles.
* Render mesh overlay.
* Save/load mesh data.

### Milestone 7 — Weight Painting and Warping

* Paint vertex weights.
* Normalize weights.
* CPU skin mesh vertices.
* Render warped cutouts.

### Milestone 8 — Export

* Export current frame PNG.
* Export animation file.
* Export optional animation sprite sheet or frame sequence.
* Add validation diagnostics.

## 21. Risks and Mitigations

| Risk                                      | Mitigation                                                                    |
| ----------------------------------------- | ----------------------------------------------------------------------------- |
| Custom UI grows too large                 | Use native Win32 controls where practical                                     |
| Mesh triangulation becomes complex        | Start with manual triangle creation                                           |
| Weight painting feels tedious             | Add normalize, smooth, and copy weights tools                                 |
| Export format changes often               | Version every file format from the beginning                                  |
| Direct2D mesh warping is limited          | CPU-skin vertices and render triangle pieces, or export mesh data for runtime |
| Rotation interpolation flips unexpectedly | Use shortest-path angle interpolation or quaternion slerp around Z            |
| Sprite pixel art becomes blurry           | Add nearest-neighbor preview mode                                             |

## 22. Initial Version Definition

The first usable version should support:

* Importing one sprite sheet.
* Creating named cutouts.
* Creating a bone hierarchy.
* Assigning cutouts to bones.
* Rotating selected bones with a slider.
* Creating keyframes.
* Scrubbing and playing interpolated animation.
* Saving/loading project files.
* Exporting current frame as PNG.
* Exporting a basic animation file with bones, rotations, sprite sheet file name, cutout coordinates, frame count, and framerate.

Mesh deformation and weight painting are important features, but they can be implemented after the basic rigid cutout animation pipeline is working.

## 23. Summary

This utility is a focused 2D skeletal animation editor for sprite cutouts. The core workflow is:

1. Import sprite sheet.
2. Define cutouts.
3. Create bones.
4. Bind cutouts to bones.
5. Optionally create meshes and paint weights.
6. Pose bones.
7. Set keyframes.
8. Preview interpolation.
9. Save the project.
10. Export images and runtime animation data.

By relying on Win32, Direct2D, WIC, and DirectXMath, the project remains lightweight, Windows-native, and free of third-party dependencies while still supporting a practical animation-authoring workflow.
