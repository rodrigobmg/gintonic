/**
 * @file camera.hpp
 * @brief Defines a camera component.
 * @author Raoul Wols
 */

#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "component.hpp"
#include "vec2f.hpp"
#include "mat4f.hpp"

namespace gintonic {

/**
 * @brief Camera component class.
 */
class camera : public std::enable_shared_from_this<camera>
{
public:

	/// Projection type. Either orthographic or perspective.
	enum ProjectionType
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};


	void setWidth(const float width);

	void setHeight(const float height);

	void setNearplane(const float nearplane);

	void setFarplane(const float farplane);

	void setFieldOfView(const float fieldOfView);

	inline float width() const noexcept
	{
		return mWidth;
	}

	inline float height() const noexcept
	{
		return mHeight;
	}

	inline float nearPlane() const noexcept
	{
		return mNearPlane;
	}

	inline float farPlane() const noexcept
	{
		return mFarPlane;
	}

	inline float fieldOfView() const noexcept
	{
		return mFieldOfView;
	}

	void setProjectionType(const ProjectionType type)
	{
		mProjection = type;
		mProjectionMatrixIsDirty = true;
	}

	/**
	 * @brief Add mouse angles.
	 * @param mouseAngles New mouse angles.
	 */
	void addMouse(const vec2f& mouseAngles) noexcept;

	const mat4f& projectionMatrix() const noexcept;

	inline const vec2f& angles() const noexcept
	{
		return mAngles;
	}

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	mutable bool mProjectionMatrixIsDirty = true;

	ProjectionType mProjection = kPerspectiveProjection;

	float mFieldOfView = 1.22173048f;

	float mWidth;

	float mHeight;

	float mNearPlane;

	float mFarPlane;

	mutable mat4f mProjectionMatrix;

	vec2f mAngles;

};

} // namespace gintonic

#endif