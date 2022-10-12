// pop.hpp - Single header utility library.

#ifndef POP_HPP
#define POP_HPP

#include <stdint.h> // *int_t
#include <stddef.h> // size_t
#include <string>   // std::string
#include <vector>   // std::vector

using std::string;
using std::vector;

#define BYTE(b) asm volatile(".byte " #b)
#define TRAP BYTE(0xCC)

namespace pop
{
	template <class T> string ToString(T value);

	void Info();
	void Success();
	void Warning();
	void Error();
	void Debug();
	void Info(const char *s);
	void Success(const char *s);
	void Warning(const char *s);
	void Error(const char *s);
	void Fatal(const char *s);
	void Debug(const char *s);
	void Info(const string s);
	void Success(const string s);
	void Warning(const string s);
	void Error(const string s);
	void Fatal(const string s);
	void Debug(const string s);

	string ByteToString(const uint8_t byte);
	string AddressToString(const uintptr_t address);
	string PointerToString(void* pointer);
	string IntToHex(uint64_t i);
	string Hex(void* p, size_t length);
	void HexDump(void* p, size_t length);
	void HexDump(vector<uint8_t> bytes);

	string BaseName(string s);
}

#ifdef POP

#include <stdlib.h> // exit() errno
#include <string.h> // strerror()
#include <ctype.h>  // isprint()

#include <iomanip> // std::hex
#include <sstream> // std::stringstream
#include <cxxabi.h>
#include <iostream>
#include <filesystem>

static const char* ANSI_RESET  = "\x1B[0m";
static const char* ANSI_ORANGE = "\x1B[38;5;202m";
static const char* ANSI_GREEN  = "\x1B[38;5;10m";
static const char* ANSI_RED    = "\x1B[38;5;9m";
static const char* ANSI_GRAY   = "\x1B[38;5;8m";

namespace pop
{
	template <class T> string ToString(T value) { return std::to_string(value); }

	void Info()    { fprintf(stdout, "[-] "); }
	void Success() { fprintf(stdout, "[%s*%s] ", ANSI_GREEN, ANSI_RESET); }
	void Warning() { fprintf(stdout, "[%s#%s] ", ANSI_ORANGE, ANSI_RESET); }
	void Error()   { fprintf(stderr, "[%s!%s] ", ANSI_RED, ANSI_RESET); }
	void Debug()   { fprintf(stderr, "[%s?%s] ", ANSI_GRAY, ANSI_RESET); }

	void Info(const char *s)    { Info();    fprintf(stdout, "%s\n", s); }
	void Success(const char *s) { Success(); fprintf(stdout, "%s\n", s); }
	void Warning(const char *s) { Warning(); fprintf(stdout, "%s\n", s); }
	void Error(const char *s)   { Error();   fprintf(stderr, "%s\n", s); }
	void Debug(const char *s)   { Debug();   fprintf(stderr, "%s\n", s); }
	void Fatal(const char *s)   { Error(s); exit(1); }

	void Info(const string s)    { Info(s.c_str()); }
	void Success(const string s) { Success(s.c_str()); }
	void Warning(const string s) { Warning(s.c_str()); }
	void Error(const string s)   { Error(s.c_str()); }
	void Fatal(const string s)   { Fatal(s.c_str()); }
	void Debug(const string s)   { Debug(s.c_str()); }

	string ByteToString(const uint8_t byte)
	{
		string s(sizeof ("FF"), 0x00);
		snprintf(&s[0], s.capacity(), "%02X", byte);
		s.pop_back();
		return s;
	}
	string AddressToString(const uintptr_t address)
	{
		string s(sizeof ("FFFFFFFFFFFFFFFF"), 0x00);
		snprintf(&s[0], s.capacity(), "%012lX", address);
		s.pop_back();
		return s;
	}
	string PointerToString(void* pointer) { return AddressToString((uintptr_t)pointer); }
	string IntToHex(uint64_t i)
	{
		std::stringstream sstream;
		sstream << std::uppercase << std::hex << i;
		return sstream.str();
	}
	string Hex(void* p, size_t length)
	{
		uint8_t* m = (uint8_t*)p;
		string s;
		s.reserve(length * 3);
		for (size_t i = 0; i < length; i++) {
			s += ByteToString(m[i]) + " ";
		}
		return s;
	}
	void HexDump(void* p, size_t length)
	{
		uint8_t* mem = (uint8_t*)p;
		vector<uint8_t> line;
		
		puts("Offset | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | Characters");
		puts("-------|-------------------------------------------------|-----------------");
		for (size_t i = 0; i < length; i++) {
			if (line.size() == 0) printf("0x%04lX | ", i);
			printf("%02X ", mem[i]);
			line.push_back(mem[i]);
			size_t size = line.size();
			if (i == length) {
				for (size_t n = 0; n <= ((16 - size) - 1); n++)
					printf("   ");
			}
			if (size == 16 || i == length) {
				printf("| ");
				for (uint8_t byte : line) {
					if (isprint(byte)) printf("%c", (char)byte);
					else printf(".");
				}
				puts("");
				line.clear();
			}
		}
		puts("");
	}
	void HexDump(vector<uint8_t> bytes)
	{
		HexDump(&bytes[0], bytes.size());
	}

	string BaseName(string s)
	{
		return std::filesystem::path(s).filename();
	}
}

#endif
#endif
