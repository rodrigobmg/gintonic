#ifndef filesystem_hpp
#define filesystem_hpp

#include <boost/filesystem.hpp>

namespace gintonic {

	typedef boost::error_info<struct tag_filesystem_path_err,boost::filesystem::path> errinfo_path;

} // namespace gintonic

BOOST_SERIALIZATION_SPLIT_FREE(boost::filesystem::path)

namespace boost {
	namespace serialization {
		template<class Archive>
		void save(Archive& ar, const filesystem::path& p, unsigned int version) 
		{
			ar & p.relative_path().generic_wstring();
		}
		template<class Archive>
		void load(Archive& ar, filesystem::path& p, unsigned int version) 
		{
			std::wstring str;
			ar & str;
			p = str;
		}
	}
} // namespace boost::serialization

#endif