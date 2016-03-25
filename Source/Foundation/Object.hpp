/**
 * @file Object.hpp
 * @author Raoul Wols
 * @brief Defines the Object class.
 */

#pragma once

#include "filesystem.hpp"
#include "ReadWriteLock.hpp"

#include <string>
#include <memory>
#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/scene/shading/fbxsurfacematerial.h>
#include <fbxsdk/scene/geometry/fbxnode.h>
#include <fbxsdk/scene/geometry/fbxnodeattribute.h>

namespace gintonic {

/**
 * @brief The Object class. Use it as a base class for
 * inheritance.
 * 
 * @tparam Derived The class that's deriving from Object.
 * @tparam NameType The type for the name member variable.
 */
template
<
	class Derived,
	class NameType
>
class Object : public std::enable_shared_from_this<Derived>
{
public:

	/// The name of this Object.
	NameType name;

	/// The ReadWriteLock for this Object.
	ReadWriteLock readWriteLock;

	/// Default constructor.
	Object() = default;

	/**
	 * @brief Constructor that takes a name.
	 * @param [in] name The name of the new Object.
	 */
	Object(const NameType& name)
	: name(name)
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Constructor that takes a name.
	 * @param [in] name The name of the new Object.
	 */
	Object(NameType&& name)
	: name(std::move(name))
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Copy constructor. The ReadWriteLock is not copied along.
	 * Only the name is copied.
	 * @param [in] other Another object.
	 */
	Object(const Object& other)
	: name(other.name)
	{
		/* Don't copy the ReadWriteLock */
	}

	/**
	 * @brief Move constructor. The ReadWriteLock is not moved.
	 * Only the name is moved.
	 * @param [in] other Another object.
	 */
	Object(Object&& other)
	: name(std::move(other.name))
	{
		/* Don't move the ReadWriteLock */
	}
	
	/**
	 * @brief Copy assignment operator. The ReadWriteLock is not copied.
	 * Only the name is copied.
	 * @param [in] other Another object.
	 * @return `*this`
	 */
	Object& operator = (const Object& other)
	{
		name = other.name;
		/* Don't copy the ReadWriteLock */
		return *this;
	}
	
	/**
	 * @brief Move assignment operator. The ReadWriteLock is not moved.
	 * Only the name is moved.
	 * @param [in] other Another object.
	 * @return `*this`
	 */
	Object& operator = (Object&& other)
	{
		name = std::move(other.name);
		/* Don't move the ReadWriteLock */
		return *this;
	}

	/**
	 * @brief Deleter function. `delete` is called on the argument.
	 * @param [in] object Pointer to an object.
	 * @deprecated Don't use this.
	 */
	static void deleter(Object* object)
	{
		delete object;
	}

	/**
	 * @brief Syntax sugar to create a derived class.
	 * @param args The constructor arguments for the derived class.
	 * @return A shared pointer to a new derived class.
	 */
	template <class ...ConstructorArguments>
	static std::shared_ptr<Derived> create(ConstructorArguments&&... args)
	{
		return std::shared_ptr<Derived>(
			new Derived(std::forward<ConstructorArguments>(args)...),
			Object::deleter);
	}

	/**
	 * @brief Output stream operator support for an Object. It simply
	 * prints the name of the object.
	 * @param [in,out] os The output stream.
	 * @param object An object.
	 * @sa Object::name
	 */
	inline friend std::ostream& operator << (std::ostream& os, const Object& object)
	{
		return os << object.name;
	}

protected:

	/// Defaulted destructor.
	virtual ~Object() noexcept = default;

	/**
	 * @brief Constructs an Object from an FbxNode. The name will
	 * be set to the name of the FbxNode.
	 * @param [in] pFbxNode Pointer to an FbxNode.
	 */
	Object(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	: name(pFbxNode->GetName())
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Constructs an Object from an FbxNodeAttribute.
	 * @details If the
	 * FbxNodeAttribute has a name, then the name of the newly constructed
	 * object will be set to this name. Otherwise, the name will be set
	 * to the name of the FbxObject that holds the FbxAttribute.
	 * @param pFbxAttribute Pointer to an FbxNodeAttribute.
	 */
	Object(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			name = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			name = pFbxAttribute->GetName();
		}
	}

	/**
	 * @brief Constructs an Object from an FbxSurfaceMaterial. The name
	 * will be set to the name of the FbxSurfaceMaterial.
	 * @param pFbxMaterial Pointer to an FbxSurfaceMaterial.
	 */
	Object(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	: name(pFbxMaterial->GetName())
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Set the name of this object using the name of
	 * the FbxNode.
	 * @param pFbxNode Pointer to an FbxNode.
	 */
	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	{
		name = pFbxNode->GetName();
	}

	/**
	 * @brief Set the name of this object using the name of
	 * the FbxNodeAttribute.
	 * @param pFbxAttribute Pointer to an FbxNodeAttribute.
	 */
	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			name = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			name = pFbxAttribute->GetName();
		}
	}

	/**
	 * @brief Set the name of this object using the name of
	 * the FbxSurfaceMaterial.
	 * @param pFbxMaterial Pointer to an FbxSurfaceMaterial.
	 */
	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	{
		name = pFbxMaterial->GetName();
	}

private:

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & name;
	}
};



} // namespace gintonic