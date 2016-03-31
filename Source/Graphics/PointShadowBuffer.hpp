#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"
#include "../Math/mat4f.hpp"

namespace gintonic {

class PointShadowBuffer : public ShadowBuffer
{
public:
	PointShadowBuffer();
	virtual ~PointShadowBuffer() noexcept = default;

	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept;

	virtual void bindDepthTextures() const noexcept;

	inline virtual const mat4f& projectionMatrix() const noexcept { return mProjectionMatrix; }

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:
	OpenGL::Framebuffer mFramebuffer[6];
	OpenGL::TextureObject mTexture[6];
	mat4f mProjectionMatrix;
};

} // namespace gintonic 