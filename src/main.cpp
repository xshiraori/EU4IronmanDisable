#include "rwh/rwh.h"
#include <iostream>

struct EU4Patch {
	std::vector<byte> signature;
	std::uint32_t offset;
	std::uint8_t instruction;
	std::uint32_t size;
};

using std::cout;
using std::endl;

std::vector<EU4Patch> vecEuPatches = {
	{{	0x49,0xFF,0xC0,
		0x48,0x8B,0x55,0x90,
		0x48,0x8D,0x4D,0x90,
		0xE8, '?', '?','?', '?',
		0x48,0xC7,0x45,
		0xA8,0x0F,0x00,0x00,0x00,
		0x4C,0x89,0x65,0xA0,
		0xC6,0x45,0x90,0x00,
		0x45,0x84,0xF6
	}, 0x23, 0xEB, 1},
	{{
		0xC7,0x04,0x25,0x00,
		0x00,0x00,0x00,0x39,
		0x05,0x00,0x00,0x0F,
		0xB6,0x45,0xE0,0x88,
		0x06,0x48,0x8D,0x4E,
		0x08,0x48,0x8D,0x55,
		0xE8}, 0x00, 0x90, 11
	}
};


void GetEu4ModuleInformation(std::uintptr_t& uiModuleBase, std::uintptr_t& uiCodeSectionBase, std::uintptr_t& uiLocalModuleBase, std::uint32_t& uiCodeSectionSize, HANDLE& hProcess) {
	std::uint32_t uiSize = 0;
	hProcess = rwh::process::FindProcess("eu4.exe");
	uiModuleBase = rwh::remote::GetRemoteProcessModule(hProcess, "eu4.exe", uiSize);
	uiLocalModuleBase = (std::uintptr_t)rwh::remote::ReadRemoteBytes(hProcess, uiModuleBase, uiSize);
	uiCodeSectionBase = rwh::module::GetModuleSectionStart(uiLocalModuleBase, ".text", uiCodeSectionSize);
}

int main(int argc, char* argv[]) {
	HANDLE hProcess;
	std::uint32_t uiCodeSectionSize;
	std::uintptr_t uiRemoteModuleBase, uiCodeSectionBase, uiLocalModuleBase;

	GetEu4ModuleInformation(uiRemoteModuleBase, uiCodeSectionBase, uiLocalModuleBase, uiCodeSectionSize, hProcess);

	if (!uiLocalModuleBase) {
		cout << "Module not found, exiting..." << endl;
		return 0;
	}

	for (auto patchInfo : vecEuPatches) {
		auto uiPatchAddressCopy = rwh::pattern::SearchRangeEx(uiCodeSectionBase, uiCodeSectionSize, patchInfo.signature);

		if (!uiPatchAddressCopy) {
			cout << "Patching failed, exiting..." << endl;
			return 0;
		}

		auto uiPatchAddress = uiRemoteModuleBase + uiPatchAddressCopy - uiLocalModuleBase + patchInfo.offset;

		cout << "Found offset, patching address ->" << std::hex << uiPatchAddress << endl;
		std::vector<std::uint8_t> patch(patchInfo.size, patchInfo.instruction);
		rwh::remote::WriteRemoteBytes(hProcess, uiPatchAddress, patch.data(), patchInfo.size);
	}

	CloseHandle(hProcess);
	return 0;
}