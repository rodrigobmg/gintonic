/**
 * @file vec2f.hpp
 * @brief Defines a two-dimensional vector class.
 * @author Raoul Wols
 */

#ifndef gintonic_vec2f_hpp
#define gintonic_vec2f_hpp

#include "../Foundation/utilities.hpp"
#include "../Foundation/Profiler.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Two-dimensional vector class that uses SSE.
 * 
 * @details The fact that this class carries an SSE type (namely, __m128) has
 * some consequences for classes that want to use this class as a datamember.
 * The SSE types need to be aligned to a 16-byte memory boundary. This is
 * done automatically by the compiler when you instantiate an object carrying
 * an SSE class on the program stack. It is **not done automatically** by the
 * compiler when you instantiate such a class **on the heap**. For this
 * reason, you **must** declare the macro
 * `

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();`
 * inside the definition of your class. This will take care of all memory
 * boundary problems. If you don't do this, your program will sporadically
 * crash every once in a while because just by chance objects *are* allocated
 * correctly on a 16-nyte memory boundary, while on some occassions they are
 * not. In fact, Linux and OSX allocate memory on a 16-byte boundary by
 * default, so we wouldn't need to worry about it if `gintonic` should only
 * work on those platforms. However Windows allocates on 8-byte boundaries.
 * 
 * If for some reason you need to override `operator new` and
 * `operator delete` for your class, then you cannot use the above mentioned
 * macro because the macro itself already overrides those operators. In this
 * case you will have to make sure by hand that objects are allocated on a
 * 16-byte boundary.
 */
union alignas(__m128) vec2f
{
private:
	__m128 data;
	friend union vec3f;
	friend union vec4f;
	friend union quatf;
public:

	struct
	{
		/// The X-coordinate.
		float x;
		/// The Y-coordinate.
		float y;
		//!@cond
		float dummy0;
		float dummy1;
		//!@endcond
	};

	/// Default constructor.
	inline vec2f() noexcept
	: dummy0(0.0f), dummy1(0.0f)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor that sets every coordinate to the given value.
	inline vec2f(const float s) noexcept
	: data(_mm_set1_ps(s))
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor.
	inline vec2f(const float a, const float b) noexcept
	: x(a), y(b), dummy0(0.0f), dummy1(0.0f)
	{

	}

	/// Constructor for a raw SSE value.
	inline vec2f(__m128 values) noexcept : data(values)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Initializer list constructor.
	vec2f(std::initializer_list<float> init) noexcept;

	/// Copy constructor.
	inline vec2f(const vec2f& v) noexcept : data(v.data)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Move constructor.
	inline vec2f(vec2f&& v) noexcept : data(v.data)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Copy assignment operator.
	inline vec2f& operator = (const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = v.data;
		return *this;
	}

	/// Move assignment operator.
	inline vec2f& operator = (vec2f&& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = v.data;
		return *this;
	}

	/// Initializer list assignment operator.
	vec2f& operator=(std::initializer_list<float> init) noexcept;

	/// Add-and-assign operator.
	inline vec2f& operator += (const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_add_ps(data, v.data);
		return *this;
	}

	/// Add-and-assign operator that first builds a vec2f.
	inline vec2f& operator += (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Subtract-and-assign operator.
	inline vec2f& operator -= (const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	/// Subtract-and-assign operator that first builds a vec2f.
	inline vec2f& operator -= (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec2f& operator *= (const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec2f& operator *= (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_mul_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Pointwise-divide-and-assign operator.
	inline vec2f& operator /= (float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		s = 1.0f / s;
		return operator*=(s);
	}

	/// Equality comparison operator.
	inline bool operator == (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x == v.x && y == v.y;
	}

	/// Inequality comparison operator.
	inline bool operator != (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return !operator==(v);
	}

	/// Less-than comparison operator.
	inline bool operator < (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x < v.x && y < v.y;
	}

	/// Greater-than-or-equal-to comparison operator.
	inline bool operator >= (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x >= v.x && y >= v.y;
	}

	/// Greater-than comparison operator.
	inline bool operator > (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x > v.x && y > v.y;
	}

	/// Less-than-or-equal-to comparison operator.
	inline bool operator <= (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x <= v.x && y <= v.y;
	}

	/// Addition operator.
	inline vec2f operator + (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_add_ps(data, v.data);
	}

	/// Addition operator that first builds a vec2f.
	inline vec2f operator + (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	/// Addition operator that first builds a vec2f (from the left).
	inline friend vec2f operator + (const float s, const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	/// Subtraction operator.
	inline vec2f operator - (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_sub_ps(data, v.data);
	}

	/// Subtraction operator that first builds a vec2f.
	inline vec2f operator - (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	/// Subtraction operator that first builds a vec2f (from the left).
	inline friend vec2f operator - (const float s, const vec2f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	/// "Take the negative" operator.
	inline vec2f operator - () const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(_mm_set1_ps(-1.0f), data);
	}

	/// Pointwise-multiplication operator.
	inline vec2f operator * (const vec2f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(data, v.data);
	}

	/// Pointwise-multiplication operator.
	inline vec2f operator * (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(data, _mm_load1_ps(&s));
	}

	/// Pointwise-multiplication operator (from the left).
	friend vec2f operator * (const float lhs, const vec2f& rhs) noexcept;

	/// Division by scalar operator.
	inline vec2f operator / (float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		s = 1.0f / s;
		return operator*(s);
	}

	/// The dot product (also called inner product).
	inline friend float dot(const vec2f& lhs, const vec2f& rhs) 
		noexcept
	{
		GT_PROFILE_FUNCTION;

		const auto product = _mm_mul_ps(lhs.data, rhs.data);
		return _mm_cvtss_f32(_mm_hadd_ps(product, product));
	}

	/// Get the squared length.
	inline float length2() const noexcept
	{
		GT_PROFILE_FUNCTION;

		return dot(*this, *this);
	}

	/// Get the length.
	inline float length() const noexcept
	{
		GT_PROFILE_FUNCTION;

		return std::sqrt(length2());
	}

	/// Normalize this vec2f to unit length.
	inline vec2f& normalize() noexcept
	{
		GT_PROFILE_FUNCTION;

		return operator/=(length());
	}

	/// Pointwise convert radians to degrees.
	friend vec2f deg2rad(const vec2f& degrees) noexcept;

	/// Pointwise convert degrees to radians.
	friend vec2f rad2deg(const vec2f& radians) noexcept;

	/// Pack two vec2f in a single SSE register.
	inline friend __m128 pack(const vec2f& lhs, const vec2f& rhs) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_shuffle_ps(lhs.data, rhs.data, 0x44);
	}

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	friend class boost::serialization::access;

	template <class Archive>
	inline void serialize(Archive& ar, const unsigned /*version*/)
	{
		GT_PROFILE_FUNCTION;

		ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//!@cond
// Implementation needs to be outside of the class for MSVC.
inline vec2f operator * (const float lhs, const vec2f& rhs) noexcept
{
	GT_PROFILE_FUNCTION;

	return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
}
//!@endcond

/// Convert from radians to degrees, pointwise.
inline vec2f deg2rad(const vec2f& degrees) noexcept
{
	GT_PROFILE_FUNCTION;

	return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
}

/// Convert from degrees to radians, pointwise.
inline vec2f rad2deg(const vec2f& radians) noexcept
{
	GT_PROFILE_FUNCTION;

	return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
}

/// Get the squared distance between two vectors.
inline float distance2(const vec2f& u, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return (u-v).length2();
}

/// Get the distance between two vectors.
inline float distance(const vec2f& u, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return (u-v).length();
}

/// Mix two vectors.
inline vec2f mix(const vec2f& u, const vec2f& v, const float a) noexcept
{
	GT_PROFILE_FUNCTION;

	return (1.0f - a) * u + a * v;
}

/**
 * @brief Project a vector onto another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to project.
 * @param v The vector to project onto. v needs to be of unit length.
 * 
 * @return `dot(u,v) * v`
 */
inline vec2f project(const vec2f& u, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return dot(u,v) * v;
}

/**
 * @brief Reject a vector off another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to reject.
 * @param v The vector to reject off. v needs to be of unit length.
 * 
 * @return `u - project(u, v)`
 */
inline vec2f reject(const vec2f& u, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return u - project(u, v);
}

/**
 * @brief reflect a vector around another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to reflect.
 * @param v The vector to reflect around. v needs to be of unit length.
 * 
 * @return `2.0f * project(u,v) - v`
 */
inline vec2f reflect(const vec2f& u, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return 2.0f * project(u,v) - v;
}

/// Output stream support for vectors.
inline std::ostream& operator << (std::ostream& os, const vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	return os << v.x << ' ' << v.y;
}

/// Input stream support for vectors.
inline std::istream& operator >> (std::istream& is, vec2f& v) noexcept
{
	GT_PROFILE_FUNCTION;

	is >> v.x >> v.y;
	return is;
}

/**
 * @brief Check wether two vectors are almost equal.
 * 
 * @param u Some vector.
 * @param v Another vector.
 * @param units_in_last_place The number of units in the last place. This
 * determines the precision of the measurement.
 * @return True if the two vectors are almost equal, false otherwise.
 */
inline bool almost_equal(
	const vec2f& u, 
	const vec2f& v, 
	const int units_in_last_place)
{
	GT_PROFILE_FUNCTION;

	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

//!@cond
// This macro is needed for boost::serialization because boost::serialization
// does not automatically assume unions are serializable.
BOOST_CLASS_IMPLEMENTATION(gintonic::vec2f, 
	boost::serialization::object_serializable);
//!@endcond

#endif
