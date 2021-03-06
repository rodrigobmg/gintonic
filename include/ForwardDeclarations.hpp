#pragma once

#include <cstddef>

namespace gintonic
{

/* Foundation classes */
template <class Derived, class NameType> class Object;
// template <class T, std::size_t Alignment> class allocator;
class exception;
class ReadLock;
class ReadWriteLock;
class Octree;
class timer;
class one_shot_timer;
class loop_timer;

/* Math classes */
union vec2f;
union vec3f;
union vec4f;
union quatf;
union mat2f;
union mat3f;
union mat4f;
struct SQT;
struct box2f;
struct box3f;
class MatrixPipeline;
class mat4fstack;
class SQTstack;

/* Graphics classes */
namespace GUI
{
class Base;
}
class AnimationClip;
class DirectionalShadowBuffer;
class Font;
class Mesh;
class PointShadowBuffer;
class ShadowBuffer;
class Texture2D;
class Light;
class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;
class Material;
class Renderer;
class Skybox;
class Skeleton;
class Mesh;
class MaterialShaderProgram;
class ShadowShaderProgram;
class AmbientLightShaderProgram;
class DirectionalLightShaderProgram;
class PointLightShaderProgram;
class SpotLightShaderProgram;
class SkyboxShaderProgram;
class FlatTextShaderProgram;

/* general classes */
class Entity;
class Camera;
class Component;

} // namespace gintonic
