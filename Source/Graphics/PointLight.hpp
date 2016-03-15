#pragma once

#include "Light.hpp"

namespace gintonic {

/**
 * @brief A point light.
 *
 * @details A point light has an intrinsic position, but no idea of a
 * direction. A point light is a point in the scene that casts light in
 * every direction. A point light also carries an extra datamember called the
 * attenuation. Indeed, we must have some way to model the falloff of a point
 * light depending on the distance to the point light. This could be a
 * constant falloff, a linear falloff or a quadratic falloff (or higher
 * powers). It can also be a combination. The way we model it is as follows.
 * Let \f$d \geq 0\f$ be the distance from the point light to the surface to
 * light. Suppose that the attenuation value is \f$(a_x, a_y, a_z)\f$. Then
 * the light contribution is multiplied by a factor of
 * \f$ 1/ \left( a_x + a_y d + a_z d^2 \right) \f$.
 */
class PointLight : public Light
{
public:

	/// Default constructor.
	PointLight() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	PointLight(const vec4f& intensity);
	
	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 * @param attenuation The attenuation value.
	 */
	PointLight(const vec4f& intensity, const vec4f& attenuation);

	/// Destructor.
	virtual ~PointLight() noexcept = default;

	/**
	 * @brief Get the attenuation.
	 * @return The attenuation value.
	 */
	vec4f attenuation() const noexcept;

	// Set the attenuation value. Setting a new attenuation value
	// also recalculates the cutoff radius (see below).
	
	/**
	 * @brief Set the attenuation value.
	 *
	 * @details When the attenuation value is set, the cutoff point is
	 * recalculated.
	 * 
	 * @param a The attenuation value.
	 */
	void setAttenuation(const vec4f& a) noexcept;
	
	/**
	 * @brief Get the cutoff radius.
	 *
	 * @details The cutoff radius is the value where the light intensity of
	 * the point light is so diminished that pixels do not get any brighter.
	 * 
	 * @return The cutoff radius.
	 */
	float cutoffPoint() const noexcept;

	/**
	 * @brief Shine the point light given the Entity's global transform.
	 *
	 * @details In the case of a point light, only the translation part of
	 * the global transformation matrix of the Entity is used.
	 */
	virtual void shine(const Entity&) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const;

	/**
	 * @brief Set the brightness.
	 *
	 * @details When the brightness value is set, the cutoff point is
	 * recalculated.
	 * 
	 * @param brightness The brightness value.
	 */
	virtual void setBrightness(const float brightness);

	/// Stream output support for a point light.
	friend std::ostream& operator << (std::ostream&, const PointLight&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	vec4f mAttenuation;

	float mCutoffPoint;

	void calculateCutoffRadius() noexcept;

	// Reimplement this method to support output streams.
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
		ar & boost::serialization::make_nvp("attenuation", mAttenuation);
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		vec4f att;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
		ar & boost::serialization::make_nvp("attenuation", att);
		setAttenuation(att);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic