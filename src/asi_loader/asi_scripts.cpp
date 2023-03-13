#include "asi_scripts.hpp"
#include "logger.hpp"
#include "pe_image.hpp"

namespace big::asi_loader
{
	void initialize()
	{

		LOG(INFO) << "Loading *.asi plugins.";

		std::string currentFolder = std::getenv("appdata");
		currentFolder += "\\BigBaseV2";
		const std::string asiFolder = currentFolder + "\\OpenHookV";

		const std::string asiSearchQuery = asiFolder + "\\*.asi";

		WIN32_FIND_DATAA fileData;
		HANDLE fileHandle = FindFirstFileA(asiSearchQuery.c_str(), &fileData);
		if (fileHandle != INVALID_HANDLE_VALUE) {

			do {

				const std::string pluginPath = asiFolder + "\\" + fileData.cFileName;

				LOG(INFO) << "Loading " << pluginPath.c_str();

				pe_utils::pe_image pluginImage;
				if (!pluginImage.load(pluginPath)) {

					LOG(FATAL) << "Failed to load image.";
					continue;
				}

				// Image not compatible, needs patching
				if (!pluginImage.is_openvhook_compatible()) {

					LOG(INFO) << "Detected non compatible image. Patching compatibility.";

					if (pluginImage.patch_compatibility()) {
						LOG(INFO) << "Successfully patched.";
					}
					else {
						LOG(FATAL) << "Failed to patch compatibility.";
						continue;
					}
				}

				// Image compatible (now), load it
				HMODULE module = LoadLibraryA(pluginPath.c_str());
				if (module) {
					LOG(INFO) << "Loaded " << fileData.cFileName << " -> " HEX_TO_UPPER(module);
				}
				else {
					LOG(FATAL) << "Failed to load";
				}

			} while (FindNextFileA(fileHandle, &fileData));

			FindClose(fileHandle);
		}

		LOG(INFO) << "Finished loading *.asi plugins.";
	}
}