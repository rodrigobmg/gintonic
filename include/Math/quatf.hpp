/**
 * @file quatf.hpp
 * @brief Defines a quaternion.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/Profiler.hpp"
#include "../Foundation/utilities.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic
{

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.

/*****************************************************************************
 * gintonic::quatf                                                            *
 *****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Represents a quaternion.
 */
union alignas(__m128) quatf {
  private:
  public:
    /// The raw SSE type.
    __m128 data;

    struct
    {
        /// The i-component.
        float x;
        /// The j-component.
        float y;
        /// The k-component.
        float z;
        /// The real component.
        float w;
    };

    /// Default constructor constructs the quaternion 1.
    inline quatf() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { GT_PROFILE_FUNCTION; }

    /// Constructor that takes a raw SSE type.
    inline quatf(const __m128& data) : data(data) { GT_PROFILE_FUNCTION; }

    /// Constructor. Note that the real part is the first argument.
    inline quatf(const float w, const float x, const float y, const float z)
        : x(x), y(y), z(z), w(w)
    {
        GT_PROFILE_FUNCTION;
    }

    /// Constructor.
    quatf(const float realpart, const vec3f& imaginarypart);

    /// \brief Addition operator.
    inline quatf operator+(const quatf& other) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return _mm_add_ps(data, other.data);
    }

    /// \brief Subtraction oeprator.
    inline quatf operator-(const quatf& other) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return _mm_sub_ps(data, other.data);
    }

    /// Multiplication operator.
    quatf operator*(const quatf& other) const noexcept;

    /// Scalar multiplication operator.
    inline quatf operator*(const float s) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return _mm_mul_ps(data, _mm_set1_ps(s));
    }

    // inline friend quatf operator * (const float a, const quatf& q) noexcept
    // {
    // 	GT_PROFILE_FUNCTION;

    // 	return _mm_mul_ps(_mm_set1_ps(a), q.data);
    // }

    /// Multiply-and-assign operator.
    inline quatf& operator*=(const quatf& q) noexcept
    {
        GT_PROFILE_FUNCTION;

        return *this = *this * q;
    }

    /// Multiply-and-assign operator for scalars.
    inline quatf& operator*=(const float s) noexcept
    {
        GT_PROFILE_FUNCTION;

        data = _mm_mul_ps(data, _mm_set1_ps(s));
        return *this;
    }

    inline quatf operator/(const quatf& q) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return *this * q.inverse();
    }

    inline quatf operator/(const float s) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return _mm_mul_ps(data, _mm_set1_ps(1.0f / s));
    }

    inline friend quatf operator/(const float s, const quatf& q) noexcept
    {
        GT_PROFILE_FUNCTION;

        return _mm_mul_ps(q.data, _mm_set1_ps(1.0f / s));
    }

    inline quatf& operator/=(const quatf& q) noexcept
    {
        GT_PROFILE_FUNCTION;

        return *this = (*this * q.inverse());
    }

    inline quatf& operator/=(const float s) noexcept
    {
        GT_PROFILE_FUNCTION;

        data = _mm_mul_ps(data, _mm_set1_ps(1.0f / s));
        return *this;
    }

    /**
     * @brief Get the conjugate of this quaternion.
     * @details If the quaternion is given by \f$q = w + xi + yj + zk\f$, then
     * the conjugate of \f$q\f$ is given by
     * \f$\overline{q} = w - xi - yj - zk \f$. Observe that
     * \f$ q \cdot \overline{q} = \left| q \right|^2 \f$, so in code lingo
     * this means `q * q.conjugate() == q.length2()` modulo floating point
     * errors.
     * @return The conjugate of this quaternion.
     */
    inline quatf conjugate() const noexcept
    {
        GT_PROFILE_FUNCTION;

        return quatf(w, -x, -y, -z);
    }

    inline quatf& invert() noexcept
    {
        GT_PROFILE_FUNCTION;

        x = -x;
        y = -y;
        z = -z;
        return operator/=(length2());
    }

    inline quatf inverse() const noexcept
    {
        GT_PROFILE_FUNCTION;

        return conjugate() / length2();
    }

    /**
     * @brief Apply the quaternion to a vector. This rotates the vector.
     * @param v The vector to apply the quaternion to.
     * @return The rotated vector.
     */
    vec3f apply_to(const vec3f& v) const noexcept;

    /**
     * @brief Get the forward direction.
     * @return The forward direction.
     */
    vec3f forward_direction() const noexcept;

    /**
     * @brief Get the right direction.
     * @return The right direction.
     */
    vec3f right_direction() const noexcept;

    /**
     * @brief Get the up direction.
     * @return The up direction.
     */
    vec3f up_direction() const noexcept;

    /**
     * @brief Get the forward direction.
     * @return The forward direction.
     */
    vec3f direction() const noexcept;

    /**
     * @brief Construct a quaternion from a mouse delta.
     * @param a The angles vector.
     * @return *this.
     */
    quatf& set_mousedelta(const vec2f& a) noexcept;

    /**
     * @brief Add a mouse delta to the current quaternion.
     * @param a The angles vector.
     * @return *this.
     * @deprecated This method does not work correctly at the moment.
     */
    quatf& add_mousedelta(const vec2f& a) noexcept;

    /**
     * @brief Get the squared length (or norm) of this quaternion.
     * @return The squared length of this quaternion.
     */
    float length2() const noexcept;

    /**
     * @brief Get the length (or norm) of this quaternion.
     * @return The length of this quaternion.
     */
    inline float length() const noexcept
    {
        GT_PROFILE_FUNCTION;

        return std::sqrt(length2());
    }

    /**
     * @brief Normalize this quaternion to unit length.
     * @return *this.
     */
    inline quatf& normalize() noexcept
    {
        GT_PROFILE_FUNCTION;

        return operator/=(length());
    }

    /// Equality comparison operator.
    inline bool operator==(const quatf& other) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    /// Inequality comparison operator.
    inline bool operator!=(const quatf& other) const noexcept
    {
        GT_PROFILE_FUNCTION;

        return !operator==(other);
    }

    /**
     * @brief Add mouse delta to this quaternion.
     * @param angles The angles vector.
     */
    void add_mouse(const vec2f& angles) noexcept;

    /**
     * @brief Construct a quaternion from an axis and an angle.
     * @param rotation_axis The rotation axis.
     * @param rotation_angle The rotation angle.
     * @return A quaternion that encodes the rotation.
     */
    static quatf axis_angle(const vec3f& rotation_axis,
                            const float rotation_angle);

    /**
     * @brief Construct a quaternion from a given yaw, pitch and roll.
     * @param yaw The amount of yaw.
     * @param pitch The amount of pitch.
     * @param roll The amount of roll.
     * @return A quaternion that encodes the rotation.
     */
    static quatf yaw_pitch_roll(const float yaw, const float pitch,
                                const float roll);

    /**
     * @brief Construct a rotation quaternion that looks at a subject.
     * @param eye_position The eye position.
     * @param subject_position The subject position.
     * @param up_direction The up direction.
     * @return A quaternion that looks from the eye to the subject with the
     * given up orientation.
     * @note This method does not work correctly. It needs implementation.
     */
    static quatf look_at(const vec3f& eye_position,
                         const vec3f& subject_position,
                         const vec3f& up_direction);

    /**
     * @brief Construct a rotation quaternion that looks at a subject.
     * @param eye_position The eye position.
     * @param subject_position The subject position.
     * @return A quaternion that looks from the eye to the subject with the
     * given up orientation given by [0,1,0].
     * @note This method does not work correctly. It needs implementation.
     */
    static quatf look_at(const vec3f& eye_position,
                         const vec3f& subject_position);

    /**
     * @brief Construct a rotation quaternion from the given mouse delta.
     * @param angles The angles vector.
     * @return A rotation quaternion that is rotated in the Y and X axis
     * according to the angles vector.
     */
    static quatf mouse(const vec2f& angles);

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    friend boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned /*version*/)
    {
        GT_PROFILE_FUNCTION;

        ar& BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) &
            BOOST_SERIALIZATION_NVP(z) & BOOST_SERIALIZATION_NVP(w);
    }
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

/**
 * Linear interpolation between two quaternions. May produce
 * unwanted results.
 * @param[in] u The first quaternion.
 * @param[in] v The second quaternion.
 * @param[in] a Interpolation parameter. Must be a number in the closed interval
 * [0,1].
 * @return      The linear interpolation.
 */
inline quatf mix(const quatf& u, const quatf& v, const float a)
{
    return u * (1.0f - a) + v * a;
}

/**
 * Spherical linear interpolation for quaternions. See the paper
 * at http://web.mit.edu/2.998/www/QuaternionReport1.pdf.
 * @param[in] u The first quaternion.
 * @param[in] v The second quaternion.
 * @param[in] s Interpolation parameter. Must be a number in the closed interval
 * [0,1].
 * @return      The spherical linear interpolation.
 */
inline quatf slerp(const quatf& u, const quatf& v, const float s)
{
    const float theta = std::acos((u * v).w);
    return (u * std::sin((1.0f - s) * theta) + v * std::sin(s * theta)) /
           std::sin(theta);
}

/// Stream output support for quaternions.
inline std::ostream& operator<<(std::ostream& os, const quatf& q)
{
    GT_PROFILE_FUNCTION;

    return os << q.w << ' ' << q.x << ' ' << q.y << ' ' << q.z;
}

/// Stream input support for quaternions.
inline std::istream& operator>>(std::istream& is, quatf& q)
{
    GT_PROFILE_FUNCTION;

    is >> q.w >> q.x >> q.y >> q.z;
    return is;
}

} // namespace gintonic

// This macro is needed for boost::serialization because boost::serialization
// does not automatically assume unions are serializable.
BOOST_CLASS_IMPLEMENTATION(gintonic::quatf,
                           boost::serialization::object_serializable);
