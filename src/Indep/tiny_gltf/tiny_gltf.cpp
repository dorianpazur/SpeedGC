#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace tinygltf
{
	bool LoadImageData(tinygltf::Image * /* image */, const int /* image_idx */, std::string * /* err */,
					std::string * /* warn */, int /* req_width */, int /* req_height */,
					const unsigned char * /* bytes */, int /* size */, void * /*user_data */) {
		return true;
	}
	
	bool WriteImageData(const std::string * /* basepath */, const std::string * /* filename */,
						const tinygltf::Image *image, bool /* embedImages */,
						const tinygltf::FsCallbacks * /* fs_cb */, const tinygltf::URICallbacks * /* uri_cb */,
						std::string * /* out_uri */, void * user_pointer) {
		return true;
	}
}
