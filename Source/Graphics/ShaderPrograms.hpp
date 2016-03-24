/**
 * @file ShaderPrograms.hpp
 * @brief Defines all the shader classes.
 * @author Raoul Wols
 */

#pragma once

#include "OpenGL/ShaderProgram.hpp"

namespace gintonic {

namespace Uniform {

#define GT_STRINGIFY(x) #x

#define GT_PASTE_TOGETHER(x, y) x ## y

#define GT_DEFINE_UNIFORM(UNIFORM_TYPE, UNIFORM_NAME, UNIFORM_NAME_WITH_FIRST_CAPITAL) \
class UNIFORM_NAME : virtual public OpenGL::ShaderProgram                              \
{                                                                                      \
private:                                                                               \
	GLuint mLocation;                                                                  \
protected:                                                                             \
	UNIFORM_NAME()                                                                     \
	{                                                                                  \
		mLocation = getUniformLocation(GT_STRINGIFY(UNIFORM_NAME));                    \
	}                                                                                  \
	virtual ~UNIFORM_NAME() noexcept = default;                                        \
public:                                                                                \
	/**                                                                                \
	 * @brief Set the value of the UNIFORM_NAME uniform in the shader program.         \
	 * @param [in] value The value to set.                                             \
	 */                                                                                \
	void GT_PASTE_TOGETHER(set, UNIFORM_NAME_WITH_FIRST_CAPITAL)                       \
		(UNIFORM_TYPE value) const noexcept                                            \
	{                                                                                  \
		setUniform(mLocation, value);                                                  \
	}                                                                                  \
};

GT_DEFINE_UNIFORM(const mat4f&, matrixPVM,                     MatrixPVM);
GT_DEFINE_UNIFORM(const mat4f&, matrixVM,                      MatrixVM);
GT_DEFINE_UNIFORM(const mat4f&, matrixPV,                      MatrixPV);
GT_DEFINE_UNIFORM(const mat3f&, matrixN,                       MatrixN);

GT_DEFINE_UNIFORM(GLint,        instancedRendering,            InstancedRendering);
GT_DEFINE_UNIFORM(GLint,        hasTangentsAndBitangents,      HasTangentsAndBitangents);

GT_DEFINE_UNIFORM(GLint,        materialFlag,                  MaterialFlag);
GT_DEFINE_UNIFORM(GLint,        materialDiffuseTexture,        MaterialDiffuseTexture);
GT_DEFINE_UNIFORM(GLint,        materialSpecularTexture,       MaterialSpecularTexture);
GT_DEFINE_UNIFORM(GLint,        materialNormalTexture,         MaterialNormalTexture);
GT_DEFINE_UNIFORM(const vec4f&, materialDiffuseColor,          MaterialDiffuseColor);
GT_DEFINE_UNIFORM(const vec4f&, materialSpecularColor,         MaterialSpecularColor);

GT_DEFINE_UNIFORM(GLint,        depthTexture,                  DepthTexture);
GT_DEFINE_UNIFORM(GLfloat,      farPlane,                      FarPlane);

GT_DEFINE_UNIFORM(GLint,        geometryBufferPositionTexture, GeometryBufferPositionTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferDiffuseTexture,  GeometryBufferDiffuseTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferSpecularTexture, GeometryBufferSpecularTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferNormalTexture,   GeometryBufferNormalTexture);

GT_DEFINE_UNIFORM(const vec2f&, viewportSize,                  ViewportSize);

GT_DEFINE_UNIFORM(const vec4f&, lightIntensity,                LightIntensity);
GT_DEFINE_UNIFORM(const vec4f&, lightAttenuation,              LightAttenuation);
GT_DEFINE_UNIFORM(const vec3f&, lightDirection,                LightDirection);
GT_DEFINE_UNIFORM(const vec3f&, lightPosition,                 LightPosition);
GT_DEFINE_UNIFORM(GLfloat,      lightCosineHalfAngle,          LightCosineHalfAngle);
GT_DEFINE_UNIFORM(GLint,        lightCastShadow,               LightCastShadow);
GT_DEFINE_UNIFORM(const mat4f&, lightShadowMatrix,             LightShadowMatrix);
GT_DEFINE_UNIFORM(GLint,        lightShadowDepthTexture,       LightShadowDepthTexture);

GT_DEFINE_UNIFORM(GLint,        debugFlag,                     DebugFlag);

GT_DEFINE_UNIFORM(GLint,        diffuseTexture,                DiffuseTexture);
GT_DEFINE_UNIFORM(const vec3f&, color,                         Color);
GT_DEFINE_UNIFORM(GLint,        glyphTexture,                  GlyphTexture);

} // namespace Uniform

template <class DerivedShaderProgram>
class ShaderProgramBase
{
public:
	inline static void initialize()
	{
		delete sInstance;
		sInstance = new DerivedShaderProgram();
	}
	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}
	inline static const DerivedShaderProgram& get() noexcept
	{
		return *sInstance;
	}
protected:
	ShaderProgramBase() = default;
	virtual ~ShaderProgramBase() noexcept = default;
private:
	static DerivedShaderProgram* sInstance;
};

template <class D> D* ShaderProgramBase<D>::sInstance = nullptr;

class MaterialShaderProgram
: public ShaderProgramBase<MaterialShaderProgram>
, public Uniform::matrixPVM
, public Uniform::matrixVM
, public Uniform::matrixN
, public Uniform::instancedRendering
, public Uniform::hasTangentsAndBitangents
, public Uniform::materialDiffuseColor
, public Uniform::materialSpecularColor
, public Uniform::materialDiffuseTexture
, public Uniform::materialSpecularTexture
, public Uniform::materialNormalTexture
, public Uniform::materialFlag
{
public:
	MaterialShaderProgram();
	virtual ~MaterialShaderProgram() noexcept = default;
};

class DepthBufferShaderProgram
: public ShaderProgramBase<DepthBufferShaderProgram>
, public Uniform::depthTexture
, public Uniform::viewportSize
, public Uniform::farPlane
{
public:
	DepthBufferShaderProgram();
	virtual ~DepthBufferShaderProgram() noexcept = default;
};

class ShadowShaderProgram
: public ShaderProgramBase<ShadowShaderProgram>
, public Uniform::matrixPVM
, public Uniform::instancedRendering
{
public:
	ShadowShaderProgram();
	virtual ~ShadowShaderProgram() noexcept = default;
};

class AmbientLightShaderProgram
: public ShaderProgramBase<AmbientLightShaderProgram>
, public Uniform::viewportSize
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::lightIntensity
{
public:
	AmbientLightShaderProgram();
	virtual ~AmbientLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a directional light.
 */
class DirectionalLightShaderProgram
: public ShaderProgramBase<DirectionalLightShaderProgram>
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightDirection
, public Uniform::lightCastShadow
, public Uniform::lightShadowMatrix
, public Uniform::lightShadowDepthTexture
{
public:
	DirectionalLightShaderProgram();
	virtual ~DirectionalLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a point light.
 */
class PointLightShaderProgram
: public ShaderProgramBase<PointLightShaderProgram>
, public Uniform::matrixPVM
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightAttenuation
, public Uniform::lightPosition
#ifndef NDEBUG
, public Uniform::debugFlag
#endif
{
public:
	PointLightShaderProgram();
	virtual ~PointLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a spot light.
 */
class SpotLightShaderProgram
: public ShaderProgramBase<SpotLightShaderProgram>
, public Uniform::matrixPVM
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightAttenuation
, public Uniform::lightDirection
, public Uniform::lightCosineHalfAngle
, public Uniform::lightPosition
#ifndef NDEBUG
, public Uniform::debugFlag
#endif
{
public:
	SpotLightShaderProgram();
	virtual ~SpotLightShaderProgram() noexcept = default;
};

/**
 * @brief Shader for a skybox.
 */
class SkyboxShaderProgram
: public ShaderProgramBase<SkyboxShaderProgram>
, public Uniform::matrixPV
, public Uniform::diffuseTexture
{
public:
	SkyboxShaderProgram();
	virtual ~SkyboxShaderProgram() noexcept = default;
};

/**
 * @brief Simple shader for two-dimensional text.
 */
class FlatTextShaderProgram 
: public ShaderProgramBase<FlatTextShaderProgram>
, public Uniform::color
, public Uniform::glyphTexture
{
public:
	FlatTextShaderProgram();
	virtual ~FlatTextShaderProgram() noexcept = default;
};

} // namespace gintonic