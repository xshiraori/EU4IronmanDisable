#pragma once

#ifndef _RWH_H_
#define _RWH_H_

#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef _WIN64
	#define C_NONE    0x00
	#define C_MODRM   0x01
	#define C_IMM8    0x02
	#define C_IMM16   0x04
	#define C_IMM_P66 0x10
	#define C_REL8    0x20
	#define C_REL32   0x40
	#define C_GROUP   0x80
	#define C_ERROR   0xff

	#define PRE_ANY  0x00
	#define PRE_NONE 0x01
	#define PRE_F2   0x02
	#define PRE_F3   0x04
	#define PRE_66   0x08
	#define PRE_67   0x10
	#define PRE_LOCK 0x20
	#define PRE_SEG  0x40
	#define PRE_ALL  0xff

	#define DELTA_OPCODES      0x4a
	#define DELTA_FPU_REG      0xfd
	#define DELTA_FPU_MODRM    0x104
	#define DELTA_PREFIXES     0x13c
	#define DELTA_OP_LOCK_OK   0x1ae
	#define DELTA_OP2_LOCK_OK  0x1c6
	#define DELTA_OP_ONLY_MEM  0x1d8
	#define DELTA_OP2_ONLY_MEM 0x1e7

	#define F_MODRM         0x00000001
	#define F_SIB           0x00000002
	#define F_IMM8          0x00000004
	#define F_IMM16         0x00000008
	#define F_IMM32         0x00000010
	#define F_IMM64         0x00000020
	#define F_DISP8         0x00000040
	#define F_DISP16        0x00000080
	#define F_DISP32        0x00000100
	#define F_RELATIVE      0x00000200
	#define F_ERROR         0x00001000
	#define F_ERROR_OPCODE  0x00002000
	#define F_ERROR_LENGTH  0x00004000
	#define F_ERROR_LOCK    0x00008000
	#define F_ERROR_OPERAND 0x00010000
	#define F_PREFIX_REPNZ  0x01000000
	#define F_PREFIX_REPX   0x02000000
	#define F_PREFIX_REP    0x03000000
	#define F_PREFIX_66     0x04000000
	#define F_PREFIX_67     0x08000000
	#define F_PREFIX_LOCK   0x10000000
	#define F_PREFIX_SEG    0x20000000
	#define F_PREFIX_REX    0x40000000
	#define F_PREFIX_ANY    0x7f000000

	#define PREFIX_SEGMENT_CS   0x2e
	#define PREFIX_SEGMENT_SS   0x36
	#define PREFIX_SEGMENT_DS   0x3e
	#define PREFIX_SEGMENT_ES   0x26
	#define PREFIX_SEGMENT_FS   0x64
	#define PREFIX_SEGMENT_GS   0x65
	#define PREFIX_LOCK         0xf0
	#define PREFIX_REPNZ        0xf2
	#define PREFIX_REPX         0xf3
	#define PREFIX_OPERAND_SIZE 0x66
	#define PREFIX_ADDRESS_SIZE 0x67

	#pragma pack(push,1)

	typedef struct {
		uint8_t len;
		uint8_t p_rep;
		uint8_t p_lock;
		uint8_t p_seg;
		uint8_t p_66;
		uint8_t p_67;
		uint8_t rex;
		uint8_t rex_w;
		uint8_t rex_r;
		uint8_t rex_x;
		uint8_t rex_b;
		uint8_t opcode;
		uint8_t opcode2;
		uint8_t modrm;
		uint8_t modrm_mod;
		uint8_t modrm_reg;
		uint8_t modrm_rm;
		uint8_t sib;
		uint8_t sib_scale;
		uint8_t sib_index;
		uint8_t sib_base;
		union {
			uint8_t imm8;
			uint16_t imm16;
			uint32_t imm32;
			uint64_t imm64;
		} imm;
		union {
			uint8_t disp8;
			uint16_t disp16;
			uint32_t disp32;
		} disp;
		uint32_t flags;
	} hde64s;

	#pragma pack(pop)

#elif _WIN32
	#define C_NONE    0x00
	#define C_MODRM   0x01
	#define C_IMM8    0x02
	#define C_IMM16   0x04
	#define C_IMM_P66 0x10
	#define C_REL8    0x20
	#define C_REL32   0x40
	#define C_GROUP   0x80
	#define C_ERROR   0xff

	#define PRE_ANY  0x00
	#define PRE_NONE 0x01
	#define PRE_F2   0x02
	#define PRE_F3   0x04
	#define PRE_66   0x08
	#define PRE_67   0x10
	#define PRE_LOCK 0x20
	#define PRE_SEG  0x40
	#define PRE_ALL  0xff

	#define DELTA_OPCODES      0x4a
	#define DELTA_FPU_REG      0xf1
	#define DELTA_FPU_MODRM    0xf8
	#define DELTA_PREFIXES     0x130
	#define DELTA_OP_LOCK_OK   0x1a1
	#define DELTA_OP2_LOCK_OK  0x1b9
	#define DELTA_OP_ONLY_MEM  0x1cb
	#define DELTA_OP2_ONLY_MEM 0x1da	

	#define F_MODRM         0x00000001
	#define F_SIB           0x00000002
	#define F_IMM8          0x00000004
	#define F_IMM16         0x00000008
	#define F_IMM32         0x00000010
	#define F_DISP8         0x00000020
	#define F_DISP16        0x00000040
	#define F_DISP32        0x00000080
	#define F_RELATIVE      0x00000100
	#define F_2IMM16        0x00000800
	#define F_ERROR         0x00001000
	#define F_ERROR_OPCODE  0x00002000
	#define F_ERROR_LENGTH  0x00004000
	#define F_ERROR_LOCK    0x00008000
	#define F_ERROR_OPERAND 0x00010000
	#define F_PREFIX_REPNZ  0x01000000
	#define F_PREFIX_REPX   0x02000000
	#define F_PREFIX_REP    0x03000000
	#define F_PREFIX_66     0x04000000
	#define F_PREFIX_67     0x08000000
	#define F_PREFIX_LOCK   0x10000000
	#define F_PREFIX_SEG    0x20000000
	#define F_PREFIX_ANY    0x3f000000

	#define PREFIX_SEGMENT_CS   0x2e
	#define PREFIX_SEGMENT_SS   0x36
	#define PREFIX_SEGMENT_DS   0x3e
	#define PREFIX_SEGMENT_ES   0x26
	#define PREFIX_SEGMENT_FS   0x64
	#define PREFIX_SEGMENT_GS   0x65
	#define PREFIX_LOCK         0xf0
	#define PREFIX_REPNZ        0xf2
	#define PREFIX_REPX         0xf3
	#define PREFIX_OPERAND_SIZE 0x66
	#define PREFIX_ADDRESS_SIZE 0x67

	#pragma pack(push,1)

		typedef struct {
			uint8_t len;
			uint8_t p_rep;
			uint8_t p_lock;
			uint8_t p_seg;
			uint8_t p_66;
			uint8_t p_67;
			uint8_t opcode;
			uint8_t opcode2;
			uint8_t modrm;
			uint8_t modrm_mod;
			uint8_t modrm_reg;
			uint8_t modrm_rm;
			uint8_t sib;
			uint8_t sib_scale;
			uint8_t sib_index;
			uint8_t sib_base;
			union {
				uint8_t imm8;
				uint16_t imm16;
				uint32_t imm32;
			} imm;
			union {
				uint8_t disp8;
				uint16_t disp16;
				uint32_t disp32;
			} disp;
			uint32_t flags;
		} hde32s;

	#pragma pack(pop)
#endif

enum class LoggingLevel {
	Debug,
	Danger,
	Error
};

enum class LoggingType {
	FILE,
	CONSOLE,
	SOCKET
};

namespace rwh {
	namespace logger{
		static LoggingLevel currentLevel = LoggingLevel::Debug;
		static LoggingType currentType;
		static HANDLE hStdout;

		template<typename ... Args>
		std::string Format(const std::string& format, Args ... args) {
			int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
			auto size = static_cast<size_t>(size_s);
			std::unique_ptr<char[]> buf(new char[size]);
			std::snprintf(buf.get(), size, format.c_str(), args ...);

			return std::string(buf.get(), buf.get() + size - 1);
		}
		VOID WriteLog(char* buffer);
		VOID SetLevel(LoggingLevel loggingLevel);
		VOID Log(LoggingLevel loggingLevel, const char* format, ...);
		VOID LogW(LoggingLevel loggingLevel, const wchar_t* format, ...);
		VOID ChangeStdout(LoggingType loggingType, HANDLE hStdout);

		template<typename ... Args>
		VOID Danger(const std::string& format, Args ... args) {
			std::string result = Format(format, args ...);
			Log(LoggingLevel::Danger, ("[WARNING] " + result).c_str());
		}

		template<typename ... Args>
		VOID Error(const std::string& format, Args ... args) {
			std::string result = Format(format, args ...);
			Log(LoggingLevel::Error, ("[ERROR] " + result).c_str());
		}

		template<typename ... Args>
		VOID Debug(const std::string& format, Args ... args) {
			std::string result = Format(format, args ...);
			Log(LoggingLevel::Debug, result.c_str());
		}
	}
	namespace process {
		bool ListProcessModules(std::unordered_map<std::string, std::uintptr_t>& moduleMap);
		HANDLE FindProcess(std::string exeName);
	}
	namespace module {
		std::uintptr_t GetModuleBaseByName(std::string moduleName);
		std::uintptr_t GetModuleSectionStart(std::string moduleName, std::string sectionName, std::uint32_t& sectionSize);
		std::uintptr_t GetModuleSectionStart(std::uintptr_t moduleBase, std::string sectionName, std::uint32_t& sectionSize);
	}
	namespace pattern{
		std::uintptr_t SearchRange(std::string pattern, std::uintptr_t startAddress, std::uint32_t range);
		std::uintptr_t SearchRangeEx(uintptr_t adr, uint32_t scanLen, std::vector<byte> pattern);
		std::uintptr_t SearchSection(std::vector<byte> pattern, std::string moduleName, std::string sectionName);
	}
	namespace dep {
#ifdef _WIN64
		unsigned int hde64_disasm(const void* code, hde64s* hs);
#elif _WIN32
		unsigned int hde32_disasm(const void* code, hde32s* hs);
#endif
	}

	namespace remote {
		std::uintptr_t GetBaseAddress();
		std::uintptr_t GetModuleSection(std::string sectionName, ULONG& sectionSize);
		std::uint8_t* ReadRemoteBytes(HANDLE hProcess, std::uintptr_t address, std::uint32_t size);
		void WriteRemoteBytes(HANDLE hProcess, std::uintptr_t address, std::uint8_t* value, std::uint32_t size);
		std::uintptr_t GetRemoteProcessModule(HANDLE hProcess, std::string moduleName, std::uint32_t& moduleSize);

		template<typename T>
		T ReadRemoteMemory(HANDLE hProcess, std::uintptr_t address) {
			T value;

			ReadProcessMemory(hProcess, address, &value, sizeof(T), NULL);			
			return value;
		}

		template<typename T>
		void WriteRemoteMemory(HANDLE hProcess, std::uintptr_t address, T value) {
			WriteProcessMemory(hProcess, address, &value, sizeof(T), NULL);
		}
	}

	namespace disk {
		std::uintptr_t ReadFile(std::string filePath);
		std::uintptr_t FindSection(std::string sectionName);
		std::uintptr_t Rva2Va(std::uint32_t rva);
	}


}

#endif