
#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/allocator.hpp"

#include "../Math/SQT.hpp"
#include "../Math/mat4f.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/tracking.hpp>

#include <iosfwd>
#include <string>
#include <vector>

#define GT_JOINT_INDEX_TYPE uint8_t
#define GT_JOINT_MAX_CHILDREN 8
#define GT_SKELETON_MAX_JOINTS (1 << 7)
#define GT_JOINT_NONE ((GT_SKELETON_MAX_JOINTS)-1)

namespace gintonic
{

class Skeleton
{
  public:
    class Joint
    {
      public:
        std::string name;

        mat4f inverseBindPose;

        uint8_t parent = GT_JOINT_NONE;

        uint8_t child[GT_JOINT_MAX_CHILDREN];

        Joint() = default;

        GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

        friend std::ostream& operator<<(std::ostream& os, const Joint& joint);

      private:
        friend class Skeleton;
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive& archive, const unsigned /*version*/)
        {
            archive& name& inverseBindPose& parent& child;
        }
    };

    class Exception : public std::exception
    {
      public:
        Exception() = delete;
        virtual ~Exception() noexcept = default;

      protected:
      private:
    };

    class TooManyChildJointsException : public Exception
    {
      public:
        TooManyChildJointsException() = delete;
        virtual ~TooManyChildJointsException() noexcept = default;
        inline const char* what() const noexcept
        {
            return "TooManyChildJointsException";
        }

      private:
        friend class Skeleton;
    };

    class TooManyJointsException : public Exception
    {
      public:
        TooManyJointsException() = delete;
        virtual ~TooManyJointsException() noexcept = default;
        inline const char* what() const noexcept
        {
            return "TooManyJointsException";
        }

      private:
        friend class Skeleton;
    };

    std::string name;

    std::vector<Joint, allocator<Joint>> joints;

    using iterator = std::vector<Joint, allocator<Joint>>::iterator;
    using const_iterator = std::vector<Joint, allocator<Joint>>::const_iterator;

    inline iterator begin() noexcept { return joints.begin(); }
    inline iterator end() noexcept { return joints.end(); }
    inline const_iterator begin() const noexcept { return joints.begin(); }
    inline const_iterator end() const noexcept { return joints.end(); }
    inline const_iterator cbegin() const noexcept { return joints.cbegin(); }
    inline const_iterator cend() const noexcept { return joints.cend(); }

    template <class StringType>
    iterator findJointByName(StringType&& name) noexcept
    {
        iterator lIter = begin();
        for (; lIter != end(); ++lIter)
        {
            if (lIter->name == std::forward<StringType>(name)) return lIter;
        }
        return lIter;
    }

    template <class StringType>
    const_iterator findJointByName(StringType&& name) const noexcept
    {
        const_iterator lIter = begin();
        for (; lIter != end(); ++lIter)
        {
            if (lIter->name == std::forward<StringType>(name)) return lIter;
        }
        return lIter;
    }

    inline Joint& operator[](const uint8_t jointIndex) noexcept
    {
        return joints[jointIndex];
    }

    inline const Joint& operator[](const uint8_t jointIndex) const noexcept
    {
        return joints[jointIndex];
    }

  private:
    friend class boost::serialization::access;

    Skeleton() = default;

    friend std::ostream& operator<<(std::ostream& os, const Skeleton& skeleton);

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& name& joints;
    }
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Skeleton, boost::serialization::track_always);
