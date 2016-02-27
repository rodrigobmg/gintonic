#include "textures.hpp"
#ifdef BOOST_MSVC
	#include <wincodec.h>
	#pragma comment(lib, "windowscodecs.lib")
#else
	#include <png.h> // The PNG library.
	#include <jpeglib.h> // The JPEG library.
#endif
#include <fstream>
#include <sstream>
#include "utilities.hpp"
#include "../exception.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gintonic {
namespace opengl {

namespace
{
	#ifdef BOOST_MSVC
	std::wstring string2wstring(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}
	const char* error_code_to_string(HRESULT hr)
	{
		switch (hr)
		{
			case WINCODEC_ERR_ABORTED: return "aborted";
			case WINCODEC_ERR_ACCESSDENIED: return "access denied";
			case WINCODEC_ERR_ALREADYLOCKED: return "already locked";
			case WINCODEC_ERR_BADHEADER: return "bad header";
			case WINCODEC_ERR_BADIMAGE: return "bad image";
			case WINCODEC_ERR_BADMETADATAHEADER: return "bad metadata header";
			case WINCODEC_ERR_BADSTREAMDATA: return "bad stream data";
			case WINCODEC_ERR_CODECNOTHUMBNAIL: return "codec not thumbnail";
			case WINCODEC_ERR_CODECPRESENT: return "codec present";
			case WINCODEC_ERR_CODECTOOMANYSCANLINES: return "codec too many scanlines";
			case WINCODEC_ERR_COMPONENTINITIALIZEFAILURE: return "component initialize failure";
			case WINCODEC_ERR_COMPONENTNOTFOUND: return "component not found";
			case WINCODEC_ERR_DUPLICATEMETADATAPRESENT: return "duplicate data present";
			case WINCODEC_ERR_FRAMEMISSING: return "frame missing";
			case WINCODEC_ERR_GENERIC_ERROR: return "generic error";
			case WINCODEC_ERR_IMAGESIZEOUTOFRANGE: return "image size out of range";
			case WINCODEC_ERR_INSUFFICIENTBUFFER: return "insufficient buffer";
			case WINCODEC_ERR_INTERNALERROR: return "internal error";
			case WINCODEC_ERR_INVALIDPARAMETER: return "invalid parameter";
			case WINCODEC_ERR_INVALIDQUERYCHARACTER: return "invalid query character";
			case WINCODEC_ERR_INVALIDQUERYREQUEST: return "invalid query request";
			case WINCODEC_ERR_INVALIDREGISTRATION: return "invalid registration";
			case WINCODEC_ERR_NOTIMPLEMENTED: return "not implemented";
			case WINCODEC_ERR_NOTINITIALIZED: return "not initialized";
			case WINCODEC_ERR_OUTOFMEMORY: return "out of memory";
			case WINCODEC_ERR_PALETTEUNAVAILABLE: return "palette unavailable";
			case WINCODEC_ERR_PROPERTYNOTFOUND: return "property not found";
			case WINCODEC_ERR_PROPERTYNOTSUPPORTED: return "property not supported";
			case WINCODEC_ERR_PROPERTYSIZE: return "property size";
			case WINCODEC_ERR_PROPERTYUNEXPECTEDTYPE: return "property unexpected type";
			case WINCODEC_ERR_REQUESTONLYVALIDATMETADATAROOT: return "request only valid metadata root";
			case WINCODEC_ERR_SOURCERECTDOESNOTMATCHDIMENSIONS: return "source rectangle does not match dimensions";
			case WINCODEC_ERR_STREAMWRITE: return "stream write";
			case WINCODEC_ERR_STREAMREAD: return "stream read";
			case WINCODEC_ERR_STREAMNOTAVAILABLE: return "stream not available";
			case WINCODEC_ERR_TOOMUCHMETADATA: return "too much metadata";
			case WINCODEC_ERR_UNKNOWNIMAGEFORMAT: return "unknown image format";
			case WINCODEC_ERR_UNEXPECTEDMETADATATYPE: return "unexpected metadata type";
			case WINCODEC_ERR_UNEXPECTEDSIZE: return "unexpected size";
			case WINCODEC_ERR_UNSUPPORTEDOPERATION: return "unsupported operation";
			case WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT: return "unsupported pixel format";
			case WINCODEC_ERR_UNSUPPORTEDVERSION: return "unsupported version";
			case WINCODEC_ERR_VALUEOUTOFRANGE: return "value out of range";
			case WINCODEC_ERR_VALUEOVERFLOW: return "value overflow";
			case WINCODEC_ERR_WRONGSTATE: return "wrong state";
			default: return "";
		}
	}

	IWICImagingFactory* s_wic_factory = nullptr;

	#elif defined(__APPLE__)

	void png_read_data(png_structp png, png_bytep data, png_size_t length)
	{
		png_voidp a = png_get_io_ptr(png);
		((::std::istream*)a)->read((char*)data, length);
	}

	#elif defined __linux__

	void png_read_data(png_structp png, png_bytep data, png_size_t length)
	{
		png_voidp a = png_get_io_ptr(png);
		((::std::istream*)a)->read((char*)data, length);
	}

	#endif

	bool s_is_initialized = false;
}


// constructs sane and default values for the texture parameters.
texture_parameters::texture_parameters()
//: depth_stencil_mode(GL_DEPTH_COMPONENT)
: mipmaps(0)
, base_level(0)
, border_color({ { 0.0f, 0.0f, 0.0f, 0.0f } })
, compare_func(GL_LEQUAL)
, lod_bias(0.0f)
, minification_filter(GL_LINEAR_MIPMAP_LINEAR)
, magnification_filter(GL_LINEAR)
, minimum_lod(-1000)
, maximum_lod(1000)
, max_level(1000)
, swizzle({ { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } })
, wrap({ { GL_REPEAT, GL_REPEAT, GL_REPEAT } })
{
	// nothing
}

texture_parameters texture::parameter = texture_parameters();

// texture2d::texture2d(texture2d&& other) noexcept
// : m_tex(other.m_tex)
// {
// 	other.m_tex = 0;
// }

// texture2d& texture2d::operator = (texture2d&& other) noexcept
// {
// 	glDeleteTextures(1, &m_tex);
// 	m_tex = other.m_tex;
// 	other.m_tex = 0;
// 	return *this;
// }

void texture::init()
{
	if (!s_is_initialized)
	{
		#ifdef BOOST_MSVC
		// we assume COM has been initialized at this point
		const auto hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			(LPVOID*)&s_wic_factory
		);
		if (!SUCCEEDED(hr))
		{
			throw exception("Failed to load image factory.");
		}
		#endif
		s_is_initialized = true;
		std::atexit(texture::release);
	}
}

void texture::release()
{
	#ifdef BOOST_MSVC
	if (s_wic_factory)
	{
		s_wic_factory->Release();
		s_wic_factory = nullptr;
	}
	#endif
}

texture::~texture()
{
	/* Empty on purpose. */
}

texture2d::texture2d(const boost::filesystem::path& filename)
{
	// GLsizei width;
	// GLsizei height;
	
	// GLenum type;
	// std::vector<char> data;

	// init_generic_image(filename, width, height, format, type, data);

	const std::string filestr(filename.string());

	GLenum format;
	int width;
	int height;
	int comp;

	const auto data = stbi_load(filestr.c_str(), &width, &height, &comp, STBI_default);

	if (!data)
	{
		throw exception("Image data ptr is null.");
	}

	switch (comp)
	{
		case STBI_grey: format = GL_RED; break;
		case STBI_grey_alpha: format = GL_RG; break;
		case STBI_rgb: format = GL_RGB; break;
		case STBI_rgb_alpha: format = GL_RGBA; break;
		default:
			stbi_image_free(data);
			throw exception("Unknown image format.");
	}
	
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glTexImage2D(GL_TEXTURE_2D, 0, format, 
		static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, 
		format, GL_UNSIGNED_BYTE, (const GLvoid*)data);	

	glGenerateMipmap(GL_TEXTURE_2D);  // Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	stbi_image_free(data);
}

texture2d::~texture2d()
{
	/* Empty on purpose. */
}

texture2d::texture2d(texture2d&& other)
: m_tex(std::move(other.m_tex))
{
	/* Empty on purpose. */
}

texture2d& texture2d::operator = (texture2d&& other)
{
	m_tex = std::move(other.m_tex);
	return *this;
}

void texture2d::bind(const GLint texture_unit) const noexcept
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, m_tex);
}

GLint texture2d::width(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &r);
	return r;
}
GLint texture2d::height(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &r);
	return r;
}
GLint texture2d::depth(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_DEPTH, &r);
	return r;
}
GLint texture2d::internal_format(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &r);
	return r;
}
bool texture2d::is_compressed(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED, &r);
	return r == GL_TRUE ? true : false;
}
GLint texture2d::compressed_size(const GLint level) const noexcept
{
	GLint r;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &r);
	return r;
}

cube_texture::cube_texture(
	const boost::filesystem::path& positive_X_file,
	const boost::filesystem::path& negative_X_file,
	const boost::filesystem::path& positive_Y_file,
	const boost::filesystem::path& negative_Y_file,
	const boost::filesystem::path& positive_Z_file,
	const boost::filesystem::path& negative_Z_file)
{
	BOOST_CONSTEXPR GLenum types[6] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	const std::string filenames[6] =
	{
		positive_X_file.string(),
		negative_X_file.string(),
		positive_Y_file.string(),
		negative_Y_file.string(),
		positive_Z_file.string(),
		negative_Z_file.string()
	};

	// const boost::filesystem::path* filenames[6] =
	// {
	// 	&positive_X_file,
	// 	&negative_X_file,
	// 	&positive_Y_file,
	// 	&negative_Y_file,
	// 	&positive_Z_file,
	// 	&negative_Z_file
	// };

	// GLsizei width;
	// GLsizei height;
	// GLenum format;
	// GLenum type;
	// std::vector<char> data;

	int width;
	int height;
	int comp;
	GLenum format;
	stbi_uc* data;

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex);
	for (int i = 0; i < 6; ++i)
	{
		data = stbi_load(filenames[i].c_str(), &width, &height, &comp, STBI_default);

		if (!data)
		{
			throw exception("Image data ptr is null.");
		}

		switch (comp)
		{
			case STBI_grey: format = GL_RED; break;
			case STBI_grey_alpha: format = GL_RG; break;
			case STBI_rgb: format = GL_RGB; break;
			case STBI_rgb_alpha: format = GL_RGBA; break;
			default:
				stbi_image_free(data);
				throw exception("Unknown image format.");
		}

		glTexImage2D(types[i], 0, format, 
			static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, 
			format, GL_UNSIGNED_BYTE, (const GLvoid*)data);	
	}
	// glGenerateMipmap(GL_TEXTURE_CUBE_MAP);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

cube_texture::~cube_texture()
{
	/* Empty on purpose. */
}

cube_texture::cube_texture(cube_texture&& other)
: m_tex(std::move(other.m_tex))
{
	/* Empty on purpose. */
}

cube_texture& cube_texture::operator = (cube_texture&& other)
{
	m_tex = std::move(other.m_tex);
	return *this;
}

void cube_texture::bind(const GLint texture_unit) const noexcept
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex);
}

void texture::init_tga_image(
	const boost::filesystem::path& filename, 
	GLsizei& m_width, 
	GLsizei& m_height, 
	GLenum& m_format, 
	GLenum& m_type, 
	std::vector<char>& m_data)
{
	std::ifstream input(filename.c_str());
	uint8_t depth;
	uint16_t width, height;
	input.seekg(12, std::ios::beg);
	input.read((char*)&width, 2);
	input.read((char*)&height, 2);
	input.read((char*)&depth,  1);
	input.seekg(18, std::ios::beg);
	m_width = static_cast<std::remove_reference<decltype(m_width)>::type>(width);
	m_height = static_cast<std::remove_reference<decltype(m_height)>::type>(height);
	switch (depth)
	{
		case 24: m_format = GL_BGR; break;
		case 32: m_format = GL_BGRA; break;
		default: throw exception(filename.string() + std::string(": Unknown TGA format."));
	}
	const std::size_t size = (depth / 8) * m_width * m_height;
	m_data.resize(size);
	input.read(m_data.data(), size);
	m_type = GL_UNSIGNED_BYTE;
}

#ifdef BOOST_MSVC
void texture::init_wic_image(
	const boost::filesystem::path& filename, 
	GLsizei& m_width, 
	GLsizei& m_height, 
	GLenum& m_format, 
	GLenum& m_type, 
	std::vector<char>& m_data)
{
	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	WICPixelFormatGUID pixelformat;
	auto wfilename = filename.wstring();
	UINT width, height, bpp, size, stride;
	HRESULT hr;

	#define CHECK_FOR_SUCCESS(handle) if (!SUCCEEDED(handle)) goto wic_error_label

	if (!s_is_initialized)
	{
		throw exception("Textures were not yet initialized.");
		// BOOST_THROW_EXCEPTION(wic_error() << errinfo_path(key()));
	}

	hr = s_wic_factory->CreateDecoderFromFilename(
		wfilename.c_str(),                // Image to be decoded
		nullptr,                         // Do not prefer a particular vendor
		GENERIC_READ,                    // Desired read access to the file
		WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
		&decoder                         // Pointer to the decoder
	);
	CHECK_FOR_SUCCESS(hr);
	hr = decoder->GetFrame(0, &frame);
	CHECK_FOR_SUCCESS(hr);
	hr = frame->GetPixelFormat(&pixelformat);
	CHECK_FOR_SUCCESS(hr);

	if (pixelformat == GUID_WICPixelFormat24bppRGB)
	{
		bpp = 24;
		m_format = GL_RGB;
	} 
	else if (pixelformat == GUID_WICPixelFormat24bppBGR)
	{
		bpp = 24;
		m_format = GL_BGR;
	}
	else if (pixelformat == GUID_WICPixelFormat32bppBGR)
	{
		bpp = 32;
		m_format = GL_BGR;
	}
	else if (pixelformat == GUID_WICPixelFormat32bppRGBA)
	{
		bpp = 32;
		m_format = GL_RGBA;
	}
	else if (pixelformat == GUID_WICPixelFormat32bppBGRA)
	{
		bpp = 32;
		m_format = GL_BGRA;
	}
	else
	{
		goto wic_error_label;
	}
	
	hr = frame->GetSize(&width, &height);
	CHECK_FOR_SUCCESS(hr);

	m_width = static_cast<GLsizei>(width);
	m_height = static_cast<GLsizei>(height);

	stride = (width * bpp + 7)/8;
	size = width * (bpp / 8) * height;
	m_data.resize(size);

	hr = frame->CopyPixels(nullptr, stride, size, (BYTE*)m_data.data());
	CHECK_FOR_SUCCESS(hr);

	m_type = GL_UNSIGNED_BYTE;

	frame->Release();
	frame = nullptr;
	decoder->Release();
	decoder = nullptr;
	return;

wic_error_label:
	if (frame) frame->Release();
	if (decoder) decoder->Release();
	std::stringstream ss;
	if (hr == WINCODEC_ERR_COMPONENTNOTFOUND)
	{
		ss << "Unable to decode " << filename.string();
	}
	else
	{
		ss << filename.string() << ": Error (code " << std::hex << hr << ')';
	}
	throw exception(ss.str());
}

#elif defined(__APPLE__)

void texture::init_png_image(
	const boost::filesystem::path& filename,
	GLsizei& width, 
	GLsizei& height, 
	GLenum& format, 
	GLenum& type, 
	std::vector<char>& data)
{
	std::ifstream lInput(filename.c_str(), std::ios::binary);
	png_bytep* lRows = nullptr;
	png_byte lSignature[8];
	lInput.read((char*)lSignature, 8);
	if (png_sig_cmp(lSignature, 0, 8) != 0)
	{
		throw exception(filename.c_str() + std::string(": Invalid PNG signature."));
	}
	png_structp lPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!lPng)
	{
		throw std::bad_alloc();
	}
	png_infop lInfo = png_create_info_struct(lPng);
	if (!lInfo)
	{
		throw std::bad_alloc();
	}
	if (setjmp(png_jmpbuf(lPng)))
	{
		png_destroy_read_struct(&lPng, &lInfo, nullptr);
		delete [] lRows;
		throw exception(filename.c_str() + std::string(": PNG read error."));
	}
	png_set_read_fn(lPng, (png_voidp)&lInput, png_read_data);
	png_set_sig_bytes(lPng, 8);
	png_read_info(lPng, lInfo);
	width                 = png_get_image_width(lPng, lInfo);
	height                = png_get_image_height(lPng, lInfo);
	png_uint_32 lBitdepth = png_get_bit_depth(lPng, lInfo);
	png_uint_32 lChannels = png_get_channels(lPng, lInfo);
	switch (png_get_color_type(lPng, lInfo))
	{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(lPng);
			lChannels = 3;
			format = GL_RGB;
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (lBitdepth < 8) png_set_expand_gray_1_2_4_to_8(lPng);
			lBitdepth = 8;
			format = GL_RED;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: format = GL_RG; break;
		case PNG_COLOR_TYPE_RGB: format = GL_RGB; break;
		case PNG_COLOR_TYPE_RGBA: format = GL_RGBA; break;
		default:
			png_destroy_read_struct(&lPng, &lInfo, nullptr);
			throw exception(filename.c_str() + std::string(": Unknown PNG color format."));
	}
	if (png_get_valid(lPng, lInfo, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(lPng);
		++lChannels;
	}
	lRows = new png_bytep[height];
	if (lRows == nullptr)
	{
		png_destroy_read_struct(&lPng, &lInfo, nullptr);
		throw std::bad_alloc();
	}
	const std::size_t size = width * height * (lBitdepth / 8) * lChannels;
	data.resize(size);
	const unsigned int lStride = width * (lBitdepth / 8) * lChannels;
	for (int i = 0; i < height; ++i)
	{
		const png_uint_32 q = (height - i - 1) * lStride;
		lRows[i] = reinterpret_cast<png_bytep>(data.data()) + q;
	}
	png_read_image(lPng, lRows);
	delete [] lRows;
	png_destroy_read_struct(&lPng, &lInfo, nullptr);
	type = GL_UNSIGNED_BYTE;
}

void texture::init_jpeg_image(
	const boost::filesystem::path& filename,
	GLsizei& width,
	GLsizei& height, 
	GLenum& format, 
	GLenum& type, 
	std::vector<char>& data)
{
	unsigned long data_size;     // length of the file
	int channels;               //  3 =>RGB   4 =>RGBA
	unsigned char * rowptr[1];    // pointer to an array
	struct jpeg_decompress_struct info; //for our jpeg info
	struct jpeg_error_mgr err;          //the error handler

	FILE* file = fopen(filename.c_str(), "rb");  //open the file

	info.err = jpeg_std_error(& err);     
	jpeg_create_decompress(& info);   //fills info structure

	//if the jpeg file doesn't load
	if (!file)
	{
		throw exception(filename.c_str() + std::string(": JPEG read error."));
	}

	jpeg_stdio_src(&info, file);    
	jpeg_read_header(&info, TRUE); // read jpeg file header

	jpeg_start_decompress(&info);  // decompress the file

	//set width and height
	width = static_cast<GLsizei>(info.output_width);
	height = static_cast<GLsizei>(info.output_height);
	channels = info.num_components;
	assert((channels == 3) || (channels == 4));
	switch (channels)
	{
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: format = GL_RGB; break;
	}

	data_size = width * height * channels;

	//--------------------------------------------
	// read scanlines one at a time & put bytes 
	//    in jdata[] array. Assumes an RGB image
	//--------------------------------------------
	data.resize(data_size);
	while (info.output_scanline < info.output_height) // loop
	{
		// Enable jpeg_read_scanlines() to fill our jdata array
		rowptr[0] = (unsigned char *)&data[0] +  // secret to method
						channels * info.output_width * info.output_scanline; 

		jpeg_read_scanlines(&info, rowptr, 1);
	}
	//---------------------------------------------------

	jpeg_finish_decompress(&info);   //finish decompressing

	fclose(file); //close the file

	type = GL_UNSIGNED_BYTE;
}

#elif defined __linux__

void texture::init_png_image(
	const boost::filesystem::path& filename,
	GLsizei& width, 
	GLsizei& height, 
	GLenum& format, 
	GLenum& type, 
	std::vector<char>& data)
{
	std::ifstream lInput(filename.c_str(), std::ios::binary);
	png_bytep* lRows = nullptr;
	png_byte lSignature[8];
	lInput.read((char*)lSignature, 8);
	if (png_sig_cmp(lSignature, 0, 8) != 0)
	{
		throw exception(filename.c_str() + std::string(": Invalid PNG signature."));
	}
	png_structp lPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!lPng)
	{
		throw std::bad_alloc();
	}
	png_infop lInfo = png_create_info_struct(lPng);
	if (!lInfo)
	{
		throw std::bad_alloc();
	}
	if (setjmp(png_jmpbuf(lPng)))
	{
		png_destroy_read_struct(&lPng, &lInfo, nullptr);
		delete [] lRows;
		throw exception(filename.c_str() + std::string(": PNG read error."));
	}
	png_set_read_fn(lPng, (png_voidp)&lInput, png_read_data);
	png_set_sig_bytes(lPng, 8);
	png_read_info(lPng, lInfo);
	width                 = png_get_image_width(lPng, lInfo);
	height                = png_get_image_height(lPng, lInfo);
	png_uint_32 lBitdepth = png_get_bit_depth(lPng, lInfo);
	png_uint_32 lChannels = png_get_channels(lPng, lInfo);
	switch (png_get_color_type(lPng, lInfo))
	{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(lPng);
			lChannels = 3;
			format = GL_RGB;
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (lBitdepth < 8) png_set_expand_gray_1_2_4_to_8(lPng);
			lBitdepth = 8;
			format = GL_RED;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: format = GL_RG; break;
		case PNG_COLOR_TYPE_RGB: format = GL_RGB; break;
		case PNG_COLOR_TYPE_RGBA: format = GL_RGBA; break;
		default:
			png_destroy_read_struct(&lPng, &lInfo, nullptr);
			throw exception(filename.c_str() + std::string(": Unknown PNG color format."));
	}
	if (png_get_valid(lPng, lInfo, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(lPng);
		++lChannels;
	}
	lRows = new png_bytep[height];
	if (lRows == nullptr)
	{
		png_destroy_read_struct(&lPng, &lInfo, nullptr);
		throw std::bad_alloc();
	}
	const std::size_t size = width * height * (lBitdepth / 8) * lChannels;
	data.resize(size);
	const unsigned int lStride = width * (lBitdepth / 8) * lChannels;
	for (int i = 0; i < height; ++i)
	{
		const png_uint_32 q = (height - i - 1) * lStride;
		lRows[i] = reinterpret_cast<png_bytep>(data.data()) + q;
	}
	png_read_image(lPng, lRows);
	delete [] lRows;
	png_destroy_read_struct(&lPng, &lInfo, nullptr);
	type = GL_UNSIGNED_BYTE;
}

void texture::init_jpeg_image(
	const boost::filesystem::path& filename,
	GLsizei& width,
	GLsizei& height, 
	GLenum& format, 
	GLenum& type, 
	std::vector<char>& data)
{
	unsigned long data_size;     // length of the file
	int channels;               //  3 =>RGB   4 =>RGBA
	unsigned char * rowptr[1];    // pointer to an array
	struct jpeg_decompress_struct info; //for our jpeg info
	struct jpeg_error_mgr err;          //the error handler

	FILE* file = fopen(filename.c_str(), "rb");  //open the file

	info.err = jpeg_std_error(& err);     
	jpeg_create_decompress(& info);   //fills info structure

	//if the jpeg file doesn't load
	if (!file)
	{
		throw exception(filename.c_str() + std::string(": JPEG read error."));
	}

	jpeg_stdio_src(&info, file);    
	jpeg_read_header(&info, TRUE); // read jpeg file header

	jpeg_start_decompress(&info);  // decompress the file

	//set width and height
	width = static_cast<GLsizei>(info.output_width);
	height = static_cast<GLsizei>(info.output_height);
	channels = info.num_components;
	assert((channels == 3) || (channels == 4));
	switch (channels)
	{
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: format = GL_RGB; break;
	}

	data_size = width * height * channels;

	//--------------------------------------------
	// read scanlines one at a time & put bytes 
	//    in jdata[] array. Assumes an RGB image
	//--------------------------------------------
	data.resize(data_size);
	while (info.output_scanline < info.output_height) // loop
	{
		// Enable jpeg_read_scanlines() to fill our jdata array
		rowptr[0] = (unsigned char *)&data[0] +  // secret to method
						channels * info.output_width * info.output_scanline; 

		jpeg_read_scanlines(&info, rowptr, 1);
	}
	//---------------------------------------------------

	jpeg_finish_decompress(&info);   //finish decompressing

	fclose(file); //close the file

	type = GL_UNSIGNED_BYTE;
}

#else
#error Platform not supported.
#endif

void texture::init_generic_image(const boost::filesystem::path& filename, GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data)
{
	const auto ext = filename.extension();

	#ifdef BOOST_MSVC

		// if (ext == ".tga" || ext == ".TGA")
		// {
		// 	init_tga_image(filename, width, height, format, type, data);
		// }
		// else
		// {
		// 	init_wic_image(filename, width, height, format, type, data);
		// }

		init_wic_image(filename, width, height, format, type, data);

	#else

		if (ext == ".tga" || ext == ".TGA")
		{
			init_tga_image(filename, width, height, format, type, data);
		}
		else if (ext == ".png" || ext == ".PNG")
		{
			init_png_image(filename, width, height, format, type, data);
		}
		else if (ext == ".jpg" || ext == ".jpeg" || ext == ".JPG" || ext == ".JPEG")
		{
			init_jpeg_image(filename, width, height, format, type, data);
		}
		else
		{
			throw exception(filename.c_str() + std::string(": Unknown file extension."));
		}

	#endif
}

} // end of namespace opengl
} // end of namespace gintonic