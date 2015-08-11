/**
 * ABOUT THE NAMING SCHEME FOR MATERIALS
 *
 * The material classes follow a naming scheme. This works as follows.
 * The words "diffuse", "specular", "normal" and "height" are abbreviated
 * with their first letter. So, "diffuse" becomes just "d", specular becomes
 * "s", and so forth. The letter after the color component indicates if it is
 * a simple uniform color value or a reference to a texture (file). So for
 * example, if you want a material with a diffuse texture, a uniform specular 
 * value, and a normal map, then this would be material_dtscnt. For another 
 * example, if we want a material that has both a diffuse color value and a 
 * diffuse texture, which get multiplied in the shader, together with a normal
 * map and a height map, then this would be material_dcdtn. The corresponding 
 * shaders follow the same convention. Each shader pair that belongs to a 
 * material starts with gp_*. So for example, the corresponding shader pair 
 * for material_dtscnt is gp_dtscnt.vs and gp_dtscnt.fs. The abbreviation gp 
 * stands for geometry pass.
 */

#ifndef gintonic_materials_hpp
#define gintonic_materials_hpp

#include "math.hpp"
#include "locks.hpp"
#include "textures.hpp"
#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

// forward declarations
class material_component_diffuse_texture;
class material_component_specular_texture;
class material_component_normal_texture;	

/*****************************************************************************
 * gintonic::material (base class for inheritance)                           *
 ****************************************************************************/

class material : public std::enable_shared_from_this<material>
{
public:

	// Bind the material.
	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Destructor.
	virtual ~material() BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Default constructor.
	material();

	// Constructor that sets the diffuse color.
	material(
		const vec4f& diffuse_color);

	// Constructor that sets the diffuse color and the specular color.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color);

	// Constructor that sets the diffuse color,
	// the specular color and
	// the diffuse texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture);

	// Constructor that sets the diffuse color,
	// the specular color,
	// the diffuse texture and
	// the specular texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture);

	// Constructor that sets the diffuse color,
	// the specular color,
	// the diffuse texture,
	// the specular texture and
	// the normal texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture,
		const boost::filesystem::path& normal_texture);

	// Static method that returns the default extension for a material.
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return ".gtm";
	}

	// Methods to save a material to a stream or file.
	void save(std::ostream&) const;
	void save(const boost::filesystem::path&) const;
	void save(const std::string&) const;
	void save(const char*) const;

	// Static methods to load a material from a stream or file.
	static material load(std::istream&);
	static material load(const boost::filesystem::path&);
	static material load(const std::string&);
	static material load(const char*);

	// The diffuse color property.
	vec4f diffuse_color;

	// The specular color property.
	vec4f specular_color;

	// Methods to determine if a material carries various textures.
	bool has_diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	bool has_specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	bool has_normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Methods to obtain a const reference to various textures.
	// Be sure to first check if the material has such textures.
	const opengl::texture2d& diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	const opengl::texture2d& specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	const opengl::texture2d& normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Methods to set various textures.
	void set_diffuse_texture(const boost::filesystem::path&);
	void set_specular_texture(const boost::filesystem::path&);
	void set_normal_texture(const boost::filesystem::path&);

	// Methods to clear various textures.
	void clear_diffuse_texture();
	void clear_specular_texture();
	void clear_normal_texture();

	// Stream output operator support.
	// Don't use this for serialization, instead use the save/load methods.
	friend std::ostream& operator << (std::ostream&, const material&);

	// Needed because a material carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	typedef std::tuple
	<
		boost::filesystem::path, 
		std::size_t, 
		opengl::texture2d
	> item_type;

	typedef std::list<item_type> datastructure_type;
	
	typedef datastructure_type::iterator iter_type;

	static void safe_obtain_texture(
		const boost::filesystem::path& filename, 
		iter_type& iter);

	static void unsafe_obtain_texture(
		const boost::filesystem::path& filename,
		iter_type& iter);

	static void safe_set_null_texture(iter_type& iter);

	static void safe_release_texture(iter_type& iter);

	static void unsafe_release_texture(iter_type& iter);

private:

	static datastructure_type s_textures;
	
	static write_lock s_textures_lock;

	friend boost::serialization::access;
	
	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		// Serialize the diffuse color and the specular color.
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
		ar & BOOST_SERIALIZATION_NVP(specular_color);

		// Obtain a lock on the global texture container.
		s_textures_lock.obtain(); 

		// Determine what textures we carry.
		bool has_diffuse = m_diffuse_tex != s_textures.end();
		bool has_specular = m_specular_tex != s_textures.end();
		bool has_normal = m_normal_tex != s_textures.end();

		boost::filesystem::path filename;

		// Serialize if we carry a diffuse texture.
		ar & BOOST_SERIALIZATION_NVP(has_diffuse);
		if (has_diffuse)
		{
			// If we do, serialize the filename of the diffuse texture.
			filename = std::get<0>(*m_diffuse_tex);
			ar & boost::serialization::make_nvp("diffuse_texture", filename);
		}

		// Serialize if we carry a specular texture.
		ar & BOOST_SERIALIZATION_NVP(has_specular);
		if (has_specular)
		{
			// If we do, serialize the filename of the specular texture.
			filename = std::get<0>(*m_specular_tex);
			ar & boost::serialization::make_nvp("specular_texture", filename);
		}

		// Serialize if we carry a normal texture.
		ar & BOOST_SERIALIZATION_NVP(has_normal);
		if (has_normal)
		{
			// If we do, serialize the filename of the normal texture.
			filename = std::get<0>(*m_normal_tex);
			ar & boost::serialization::make_nvp("normal_texture", filename);
		}

		// Release the lock on global texture container.
		s_textures_lock.release(); 
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		// Load the diffuse color and the specular color.
		// These are always present.
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
		ar & BOOST_SERIALIZATION_NVP(specular_color);

		// Obtain a lock on the global texture container.
		s_textures_lock.obtain();

		bool has_diffuse;
		bool has_specular;
		bool has_normal;
		boost::filesystem::path filename;

		// Check if we need to load a diffuse texture.
		ar & BOOST_SERIALIZATION_NVP(has_diffuse);
		if (has_diffuse)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("diffuse_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_diffuse_tex);
		}
		else
		{
			// Otherwise, set our diffuse iterator to point
			// to the end of the global texture container.
			m_diffuse_tex = s_textures.end();
		}

		// Check if we need to load a specular texture.
		ar & BOOST_SERIALIZATION_NVP(has_specular);
		if (has_specular)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("specular_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_specular_tex);
		}
		else
		{
			// Otherwise, set our diffuse iterator to point
			// to the end of the global texture container.
			m_specular_tex = s_textures.end();
		}

		// Check if we need to load a normal texture.
		ar & BOOST_SERIALIZATION_NVP(has_normal);
		if (has_normal)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("normal_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_normal_tex);
		}
		else
		{
			// Otherwise, set our normal iterator to point
			// to the end of the global texture container.
			m_normal_tex = s_textures.end();
		}

		// Release the lock on global texture container.
		s_textures_lock.release();
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_diffuse_tex;
	iter_type m_specular_tex;
	iter_type m_normal_tex;

	friend class material_component_diffuse_texture;
	friend class material_component_specular_texture;
	friend class material_component_normal_texture;	
};

class material_component_diffuse_color
{
public:
	
	vec4f diffuse_color;

	material_component_diffuse_color(const vec4f&);

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

	virtual ~material_component_diffuse_color() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_component_diffuse_color() = default;

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
	}
};

class material_component_diffuse_texture
{
public:

	const opengl::texture2d& diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_texture(boost::filesystem::path);

	material_component_diffuse_texture(boost::filesystem::path);

	virtual ~material_component_diffuse_texture() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_component_diffuse_texture();

private:

	material::iter_type m_tex;

	friend boost::serialization::access;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & boost::serialization::make_nvp("diffuse_texture", 
			std::get<0>(*m_tex));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		boost::filesystem::path diffuse_filename;
		ar & boost::serialization::make_nvp("diffuse_texture", 
			diffuse_filename);
		set_diffuse_texture(diffuse_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

class material_component_specular_color
{
	
	vec4f specular_color;

	material_component_specular_color(const vec4f&);

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

	virtual ~material_component_specular_color() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_component_specular_color() = default;

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(specular_color);
	}
};

class material_component_specular_texture
{
public:

	const opengl::texture2d& specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular_texture(boost::filesystem::path);

	material_component_specular_texture(boost::filesystem::path);

	virtual ~material_component_specular_texture() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_component_specular_texture();

private:

	material::iter_type m_tex;

	friend boost::serialization::access;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & boost::serialization::make_nvp("specular_texture", 
			std::get<0>(*m_tex));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		boost::filesystem::path diffuse_filename;
		ar & boost::serialization::make_nvp("specular_texture", 
			diffuse_filename);
		set_specular_texture(diffuse_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

class material_component_normal_texture
{
public:

	const opengl::texture2d& normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal_texture(boost::filesystem::path);

	material_component_normal_texture(boost::filesystem::path);

	virtual ~material_component_normal_texture() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_component_normal_texture();

private:

	material::iter_type m_tex;

	friend boost::serialization::access;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & boost::serialization::make_nvp("normal_texture", 
			std::get<0>(*m_tex));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		boost::filesystem::path diffuse_filename;
		ar & boost::serialization::make_nvp("normal_texture", 
			diffuse_filename);
		set_normal_texture(diffuse_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

/*****************************************************************************
 * gintonic::material_dc (diffuse color)                                     *
 ****************************************************************************/

// class material_dcdtscnt
// : public material_component_diffuse_color
// , public material_component_diffuse_texture
// , public material_component_specular_color
// , public material_component_normal_texture
// {
// public:
// 	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

// 	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

// private:

// 	friend boost::serialization::access;

// 	template <class Archive>
// 	void serialize(Archive& ar, const unsigned /*version*/)
// 	{
// 		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_component_diffuse_color);
// 		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_component_diffuse_texture);
// 		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_component_specular_color);
// 		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_component_normal_texture);
// 	}
// };

class material_dc : public material
{
public:

	material_dc() = default;

	material_dc(const vec4f& diffuse_color);

	virtual ~material_dc() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec4f diffuse_color;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material);
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
	}
};

/*****************************************************************************
 * gintonic::material_dcsc (diffuse color, specular color)                   *
 ****************************************************************************/

class material_dcsc : public material_dc
{
public:

	material_dcsc() = default;

	material_dcsc(const vec4f& diffuse_color, const vec4f& specular_color);

	virtual ~material_dcsc() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec4f specular_color;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_dc);
		ar & BOOST_SERIALIZATION_NVP(specular_color);
	}
};

/*****************************************************************************
 * gintonic::material_dt (diffuse texture)                                   *
 ****************************************************************************/

class material_dt : public material
{
public:

	material_dt(boost::filesystem::path diffuse_filename);

	virtual ~material_dt() BOOST_NOEXCEPT_OR_NOTHROW;

	void set_diffuse_texture(const boost::filesystem::path& filename);
	const opengl::texture2d& diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	material_dt(); // for boost::serialization

private:

	friend boost::serialization::access;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material);
		ar & boost::serialization::make_nvp("diffuse_texture", 
			std::get<0>(*m_diffuse));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		boost::filesystem::path diffuse_filename;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material);
		ar & boost::serialization::make_nvp("diffuse_texture", 
			diffuse_filename);
		set_diffuse_texture(diffuse_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_diffuse;
};

/*****************************************************************************
 * gintonic::material_dtsc (diffuse color+texture)                           *
 ****************************************************************************/

class material_dcdt : public material_dt
{
public:

	vec4f diffuse_color;

	material_dcdt(const vec4f& diffuse_color,
		boost::filesystem::path diffuse_filename);

	virtual ~material_dcdt() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	material_dcdt() = default; // for boost serialization

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_dt);
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
	}
};

/*****************************************************************************
 * gintonic::material_dtsc (diffuse texture, specular color)                 *
 ****************************************************************************/

class material_dtsc : public material_dt
{
public:

	vec4f specular_color;

	material_dtsc(
		boost::filesystem::path diffuse_filename,
		const vec4f& specular_color);

	virtual ~material_dtsc() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	material_dtsc() = default; // for boost serialization

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_dt);
		ar & BOOST_SERIALIZATION_NVP(specular_color);
	}
};

/*****************************************************************************
 * gintonic::material_c (color)                                              *
 ****************************************************************************/

class material_c : public material
{
public:

	material_c() = default;

	material_c(const vec4f& diffuse_color);

	virtual ~material_c() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec4f diffuse_color;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material);
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
	}
};

class diffuse_material : public material
{
public:

	const opengl::texture2d& diffuse() const BOOST_NOEXCEPT_OR_NOTHROW;

	void set_diffuse(const boost::filesystem::path& filename);

	float diffuse_factor;

	diffuse_material(
		boost::filesystem::path diffuse_filename,
		const float diffuse_factor);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
private:
	iter_type m_diffuse;
};

/*****************************************************************************
 * gintonic::material_cd (color, diffuse)                                    *
 ****************************************************************************/

class material_cd : public material_c
{
public:

	const opengl::texture2d& diffuse() const BOOST_NOEXCEPT_OR_NOTHROW;

	void set_diffuse(const boost::filesystem::path& filename);

	material_cd(
		const vec4f& color,
		boost::filesystem::path diffuse_filename);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual ~material_cd() BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	material_cd(); // for boost::serialization

private:

	friend boost::serialization::access;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_c);
		ar & boost::serialization::make_nvp("diffuse", 
			std::get<0>(*m_diffuse));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_c);
		boost::filesystem::path diffuse_filename;
		ar & boost::serialization::make_nvp("diffuse", diffuse_filename);
		set_diffuse(diffuse_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_diffuse;
};

/*****************************************************************************
 * gintonic::material_cds (color, diffuse, specular)                         *
 ****************************************************************************/

class material_cds : public material_cd
{
public:
	const opengl::texture2d& specular() const BOOST_NOEXCEPT_OR_NOTHROW;

	void set_specular(const boost::filesystem::path&);

	material_cds(
		const vec4f& color,
		boost::filesystem::path diffuse_filename,
		boost::filesystem::path specular_filename);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual ~material_cds() BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	material_cds(); // for boost::serialization

private:

	friend boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_cd);
		ar & boost::serialization::make_nvp("specular", 
			std::get<0>(*m_specular));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_cd);
		boost::filesystem::path specular_filename;
		ar & boost::serialization::make_nvp("specular", specular_filename);
		set_specular(specular_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_specular;

};

/*****************************************************************************
 * gintonic::material_cdn (color, diffuse, normal map)                       *
 ****************************************************************************/

class material_cdn : public material_cd
{
public:

	const opengl::texture2d& normal() const BOOST_NOEXCEPT_OR_NOTHROW;

	void set_normal(const boost::filesystem::path&);

	material_cdn(
		const vec4f& color,
		boost::filesystem::path diffuse_filename,
		boost::filesystem::path normal_filename);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material_cdn() BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	material_cdn(); // for boost::serialization

private:

	friend boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_cd);
		ar & boost::serialization::make_nvp("normal", std::get<0>(*m_normal));
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(material_cd);
		boost::filesystem::path normal_filename;
		ar & boost::serialization::make_nvp("normal", normal_filename);
		set_normal(normal_filename);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_normal;
};

class specular_diffuse_material : public diffuse_material
{
public:
	const opengl::texture2d& specular() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular(const boost::filesystem::path& filename);
	float shininess;

	specular_diffuse_material(
		boost::filesystem::path diffuse_filename,
		boost::filesystem::path specular_filename,
		const float diffuse_factor,
		const float shininess);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~specular_diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
private:
	iter_type m_specular;
};

} // namespace gintonic

#endif