#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <thread>

#if _WIN32 || _WIN64
#include <windows.h> // SetConsoleTextAttribute
#else
#error "Platform isn't supported!"
#endif

namespace Pit::Test {
	struct _PitTestEntry {
		std::string name;
		std::function<bool()> testFunc;
	};

	struct _PitTestsRegister {
		static std::vector<_PitTestEntry> Tests;
		template<typename T>
		static bool Register() {
			Tests.emplace_back(T::GetTestName(), T::GetTestFunc());

			return true;
		}
	};
	std::vector<_PitTestEntry> _PitTestsRegister::Tests;

#if _WIN32 || _WIN64
#define CONSOLE_RED		4
#define CONSOLE_GREEN	10
#define CONSOLE_WHITE	15
	
	static HANDLE consoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
	FORCEINLINE void SetConsoleColor(int color) {
		SetConsoleTextAttribute(consoleColor, static_cast<WORD>(color));
	}
#elif __linux__     // !!! UNTESTED !!!
#define CONSOLE_RED		31
#define CONSOLE_GREEN	32
#define CONSOLE_WHITE	39

	FORCEINLINE void SetConsoleColor(int color) {
		std::cout << "\033[" << color << "m";
	}
#endif     // !!! UNTESTED !!!



	void RunAllPitTests() {
		SetConsoleColor(CONSOLE_GREEN);
		std::cout << "[PitTest]\n";
		int testsFailed = 0;

		for (auto& testEntry : _PitTestsRegister::Tests) {
			SetConsoleColor(CONSOLE_GREEN);
			std::cout << "\n[" << testEntry.name << "]\n";
			bool result = testEntry.testFunc();
			if (!result)
				testsFailed++;
		}

		if (testsFailed == 0) {
			SetConsoleColor(CONSOLE_GREEN);
			std::cout << "\nALL TESTS PASSED :>\n";
		}
		else {
			SetConsoleColor(CONSOLE_RED);
			std::cout << "\n" << testsFailed << " TEST" << (testsFailed > 1 ? "S" : "") << " FAILED >:[\n";
		}
		SetConsoleColor(CONSOLE_WHITE);
	}
}

#define PIT_TEST_BASETEST(name, testFunc)						\
	struct TEST_##name {										\
		static std::string GetTestName() { return #name; }		\
		static std::function<bool()> GetTestFunc() {			\
			return []() { testFunc };							\
		}														\
		static bool s_registered;								\
	};															\
	bool TEST_##name::s_registered = Pit::Test::_PitTestsRegister::Register<TEST_##name>()

#define PIT_TEST(name, func)									\
	PIT_TEST_BASETEST(name, { func return true; })

#define PIT_TEST_EQUALS(name, val1, val2)						\
	Pit::Test::SetConsoleColor(CONSOLE_WHITE);					\
	std::cout << #name;											\
	if (val1 == val2) {											\
		Pit::Test::SetConsoleColor(CONSOLE_GREEN);				\
		std::cout << " [OK]\n";									\
	}															\
	else {														\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [FAILED]\n";								\
	}

#define PIT_TEST_DIFFRENCE(name, val1, val2)					\
	Pit::Test::SetConsoleColor(CONSOLE_WHITE);					\
	std::cout << #name;											\
	if (val1 != val2) {											\
		Pit::Test::SetConsoleColor(CONSOLE_GREEN);				\
		std::cout << " [OK]\n";									\
	}															\
	else {														\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [FAILED]\n";								\
	}

#define PIT_TEST_TRUE(name, result)								\
	Pit::Test::SetConsoleColor(CONSOLE_WHITE);					\
	std::cout << #name;											\
	if (result) {												\
		Pit::Test::SetConsoleColor(CONSOLE_GREEN);				\
		std::cout << " [OK]\n";									\
	}															\
	else {														\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [FAILED]\n";								\
	}

#define PIT_TEST_TRY_FUNC(name, func)							\
	{															\
	Pit::Test::SetConsoleColor(CONSOLE_WHITE);					\
	std::cout << #name;											\
	try {														\
		func();													\
	}															\
	catch (const std::exception& e) {							\
		failed = true;											\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [EXCEPTION]: " << e.what();				\
		Pit::Test::SetConsoleColor(CONSOLE_WHITE);				\
	}															\
	catch (...) {												\
		failed = true;											\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [EXCEPTION TRIGGERED]";					\
		Pit::Test::SetConsoleColor(CONSOLE_WHITE);				\
	}															\
	if (failed) return false;									\
	else {														\
		Pit::Test::SetConsoleColor(CONSOLE_GREEN);				\
		std::cout << " [OK]\n";									\
	}															\
	}

#define PIT_TEST_TRY_LAMDA(name, lamda)							\
	{															\
	bool failed = false;										\
	Pit::Test::SetConsoleColor(CONSOLE_WHITE);					\
	std::cout << #name;											\
	try {														\
		{ lamda	}												\
	}															\
	catch (const std::exception& e) {							\
		failed = true;											\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [EXCEPTION]: " << e.what();				\
		Pit::Test::SetConsoleColor(CONSOLE_WHITE);				\
	}															\
	catch (...) {												\
		failed = true;											\
		Pit::Test::SetConsoleColor(CONSOLE_RED);				\
		std::cout << " [EXCEPTION TRIGGERED]";					\
		Pit::Test::SetConsoleColor(CONSOLE_WHITE);				\
	}															\
	if (failed) return false;									\
	else {														\
		Pit::Test::SetConsoleColor(CONSOLE_GREEN);				\
		std::cout << " [OK]\n";									\
	}															\
	}

int main() {
	Pit::Test::RunAllPitTests();
	std::cin.get();
}