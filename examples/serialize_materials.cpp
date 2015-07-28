#include "gintonic.hpp"
#include <boost/archive/text_oarchive.hpp>
#include "portable_iarchive.hpp"
#include "portable_oarchive.hpp"
#include <fstream>

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	try
	{
		const boost::filesystem::path filename("delete_me");
		gt::init_all("serialize_materials");
		{
			gt::material* ptr = nullptr;
			
			ptr = new gt::material_cd(gt::vec4f(0.25f, 1.0f, 1.0f, 1.0f), "../examples/bricks.jpg");

			ptr->save(filename);

			delete ptr;
			ptr = nullptr;
		}
		// End of scope; archives flush
		{
			gt::material* ptr = nullptr;

			ptr = gt::material::load(filename);

			assert(ptr);

			delete ptr;
			ptr = nullptr;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}