#include "Graphics/PointShadowBuffer.hpp"

#include "Foundation/exception.hpp"

#include "Math/mat4f.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderPrograms.hpp"
#include "Graphics/Mesh.hpp"

#include "Entity.hpp"
#include "Camera.hpp"

namespace gintonic {

PointShadowBuffer::PointShadowBuffer()
{
	/* Empty on purpose. */
}

void PointShadowBuffer::collect(
	const Entity& /*lightEntity*/, 
	const std::vector<std::shared_ptr<Entity>>& /*shadowCastingGeometryEntities*/) noexcept
{
	/* Empty on purpose. */
}

void PointShadowBuffer::bindDepthTextures() const noexcept
{
	/* Empty on purpose. */
}

} // namespace gintonic