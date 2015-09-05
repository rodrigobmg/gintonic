/*!
\file exception.hpp
\author Raoul Wols
*/

#ifndef exception_hpp
#define exception_hpp

#include <stdexcept>

namespace gintonic {

/**
 * @brief Simple exception class.
 * @details Derives from std::exception. It has
 * simple append and prepend functionality for
 * strings.
 */
class exception : public std::exception
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param message The exception message.
	 */
	exception(const std::string& message);

	/// Move constructor.
	exception(std::string&& message);

	/**
	 * @brief Constructor.
	 * 
	 * @param message The exception message.
	 */
	exception(const char* message);

	/// Copy constructor.
	exception(const exception& other) = default;

	/// Move constructor.
	exception(exception&&) BOOST_NOEXCEPT_OR_NOTHROW;

	/// Copy assignment operator.
	exception& operator=(const exception&) = default;

	/// Move assignment operator.
	exception& operator=(exception&&);

	/// Destructor.
	virtual ~exception() BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Get the exception message.
	 * @return The exception message.
	 */
	virtual const char* what() const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Append a string to the exception message.
	 * 
	 * @param message The string to append.
	 * @return *this.
	 */
	exception& append(std::string message);

	/**
	 * @brief Prepend a string to the exception message.
	 * 
	 * @param message The string to prepend.
	 * @return *this.
	 */
	exception& prepend(std::string message);
	
private:
	std::string m_message;
};

} // end of namespace gintonic

#endif