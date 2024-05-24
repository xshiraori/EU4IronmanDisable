#include "rwh/rwh.h"
#include <iostream>

struct Patch {
	std::vector<byte> signature;
	std::uint32_t offset;
	std::uint8_t instruction;
	std::uint32_t size;
};

struct ModuleInformation {
	std::uintptr_t moduleBase;
	std::uintptr_t codeSectionBase;
	std::uint32_t codeSectionSize;
	std::uintptr_t localModuleBase;
	HANDLE processHandle;
};

bool retrieveModuleInformation(ModuleInformation& moduleInformation, std::string const& moduleName) {
	std::uint32_t uiSize = 0;
	moduleInformation.processHandle = rwh::process::FindProcess(moduleName);
	moduleInformation.moduleBase = rwh::remote::GetRemoteProcessModule(moduleInformation.processHandle, moduleName, uiSize);
	moduleInformation.localModuleBase = (std::uintptr_t)rwh::remote::ReadRemoteBytes(moduleInformation.processHandle, moduleInformation.moduleBase, uiSize);
	moduleInformation.codeSectionBase = rwh::module::GetModuleSectionStart(moduleInformation.localModuleBase, ".text", moduleInformation.codeSectionSize);

	return moduleInformation.processHandle == INVALID_HANDLE_VALUE || moduleInformation.codeSectionBase == 0 || moduleInformation.moduleBase == 0 || moduleInformation.localModuleBase == 0;
}

bool applyGameSpecificPatches(ModuleInformation targetModuleInformation, auto gameSpecificPatches)
{
	for (auto& patchInformation : gameSpecificPatches) {
		auto const patchAddressCopy = rwh::pattern::SearchRangeEx(targetModuleInformation.codeSectionBase, targetModuleInformation.codeSectionSize, patchInformation.signature);

		if (!patchAddressCopy) {
			std::cout << "Patching failed, exiting..." << std::endl;
			return false;
		}

		auto const addressToPatch = targetModuleInformation.moduleBase + patchAddressCopy - targetModuleInformation.localModuleBase + patchInformation.offset;
		std::cout << "Found offset, patching address ->" << std::hex << addressToPatch << std::endl;

		std::vector<std::uint8_t> patch(patchInformation.size, patchInformation.instruction);
		rwh::remote::WriteRemoteBytes(targetModuleInformation.processHandle, addressToPatch, patch.data(), patchInformation.size);
	}

	return true;
}

int main(int argc, char* argv[]) {
	ModuleInformation moduleInformation;
	if (!retrieveModuleInformation(moduleInformation, "eu4.exe"))
	{
		std::cout << "Unable to retrieve module information" << std::endl;
		return -1;
	}

	Patch firstPatch = { {0x49, 0xFF, 0xC0, 0x48, 0x8B, 0x55, 0x90, 0x48, 0x8D, 0x4D, 0x90, 0xE8, '?', '?','?', '?', 0x48, 0xC7, 0x45, 0xA8, 0x0F, 0x00, 0x00, 0x00, 0x4C, 0x89, 0x65, 0xA0, 0xC6, 0x45, 0x90, 0x00, 0x45, 0x84, 0xF6 }, 0x23, 0xEB, 0x1 };
	Patch secondPatch = { {0xC7, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x39, 0x05, 0x00, 0x00, 0x0F, 0xB6, 0x45, 0xE0, 0x88, 0x06, 0x48, 0x8D, 0x4E, 0x08, 0x48, 0x8D, 0x55, 0xE8}, 0x00, 0x90, 0xB };

	std::vector<Patch> gameSpecificPatches{ firstPatch, secondPatch };

	if (!applyGameSpecificPatches(moduleInformation, gameSpecificPatches))
	{
		return -1;
	}

	CloseHandle(moduleInformation.processHandle);
	return 0;
}