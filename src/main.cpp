#include "rwh/rwh.h"
#include <iostream>

namespace eu4{
struct Patch {
	std::vector<rwh::u8> signature;
	rwh::u32 offset;
	rwh::u8 instruction;
	rwh::u32 size;
};

struct ModuleInformation {
	rwh::ptr moduleBase;
	rwh::ptr codeSectionBase;
	rwh::u32 codeSectionSize;
	rwh::ptr localModuleBase;
	rwh::handle processHandle;
};

static const Patch MODULE_FIRST_PATCH = 
{ 
	{0x49, 0xFF, 0xC0, 0x48, 0x8B, 0x55, 0x90, 0x48, 0x8D, 0x4D, 0x90, 0xE8, '?', '?','?', '?', 0x48, 0xC7, 0x45, 0xA8, 0x0F, 0x00, 0x00, 0x00, 0x4C, 0x89, 0x65, 0xA0, 0xC6, 0x45, 0x90, 0x00, 0x45, 0x84, 0xF6 }, // signature
	0x23, // offset
	0xEB, // insn
	0x1 // size
};

static const Patch MODULE_SECOND_PATCH = 
{ 
	{0xC7, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x39, 0x05, 0x00, 0x00, 0x0F, 0xB6, 0x45, 0xE0, 0x88, 0x06, 0x48, 0x8D, 0x4E, 0x08, 0x48, 0x8D, 0x55, 0xE8}, // signature
	0x00, // offset
	0x90, // insn
	0xB // size
};

static const Patch MODULE_PATCH_LIST[] = 
{ 
	MODULE_FIRST_PATCH, 
	MODULE_SECOND_PATCH 
};

bool retrieveModuleInformation(ModuleInformation& moduleInformation, std::string const& moduleName) {
	rwh::u32 size = 0;
	moduleInformation.processHandle = rwh::process::FindProcess(moduleName);
	moduleInformation.moduleBase = rwh::remote::GetRemoteProcessModule(moduleInformation.processHandle, moduleName, size);
	moduleInformation.localModuleBase = reinterpret_cast<rwh::ptr>(rwh::remote::ReadRemoteBytes(moduleInformation.processHandle, moduleInformation.moduleBase, size));
	moduleInformation.codeSectionBase = rwh::module::GetModuleSectionStart(moduleInformation.localModuleBase, ".text", moduleInformation.codeSectionSize);

	return moduleInformation.processHandle != INVALID_HANDLE_VALUE && moduleInformation.codeSectionBase != 0 && moduleInformation.moduleBase != 0 && moduleInformation.localModuleBase != 0;
}

bool applyGameSpecificPatches(ModuleInformation const& targetModuleInformation)
{
	for (auto& patchInformation : MODULE_PATCH_LIST) {
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
}

int main(int argc, char* argv[]) {
	eu4::ModuleInformation moduleInformation;
	if (!eu4::retrieveModuleInformation(moduleInformation, "eu4.exe"))
	{
		std::cout << "Unable to retrieve module information" << std::endl;
		return -1;
	}

	if (!eu4::applyGameSpecificPatches(moduleInformation))
	{
		std::cout << "Error while trying to apply game patches" << std::endl;
		return -1;
	}

	CloseHandle(moduleInformation.processHandle);
	return 0;
}