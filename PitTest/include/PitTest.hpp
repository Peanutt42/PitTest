#pragma once

#include <iostream>
#include <vector>
#include <functional>

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
		SetConsoleTextAttribute(consoleColor, color);
	}
#elif __linux__     // !!! UNTESTED !!!
#define CONSOLE_RED		31
#define CONSOLE_GREEN	32
#define CONSOLE_WHITE	39

	FORCEINLINE void SetConsoleColor(int color) {
		std::cout << "\033[" << color << "m";
	}
#endif     // !!! UNTESTED !!!

	FORCEINLINE void PitTests() {
		SetConsoleColor(CONSOLE_GREEN);
		std::cout << "[PitTest]\n";
		int testsFailed = 0;

		for (auto& testEntry : _PitTestsRegister::Tests) {
			SetConsoleColor(CONSOLE_WHITE);
			std::cout << testEntry.name << "...";
			bool result = testEntry.testFunc();
			if (result) {
				SetConsoleColor(CONSOLE_GREEN);
				std::cout << " [OK]\n";
			}
			else {
				SetConsoleColor(CONSOLE_RED);
				std::cout << " [FAILED]\n";
				testsFailed++;
			}
			SetConsoleColor(CONSOLE_WHITE);
		}

		if (testsFailed == 0) {
			SetConsoleColor(CONSOLE_GREEN);
			std::cout << "ALL TESTS PASSED :>\n";
		}
		else {
			SetConsoleColor(CONSOLE_RED);
			std::cout << testsFailed << " TEST" << (testsFailed > 1 ? "S" : "") << " FAILED :[\n";
		}
		SetConsoleColor(CONSOLE_WHITE);
	}

#define PIT_TEST_BASETEST(name, testFunc)	\
	struct TEST_##name {											\
		static std::string GetTestName() { return #name; }			\
		static std::function<bool()> GetTestFunc() {				\
			return []() { testFunc };					\
		}															\
		static bool s_registered;									\
	};																\
	bool TEST_##name::s_registered = _PitTestsRegister::Register<TEST_##name>()

#define PIT_TEST_EQUALS(name, val1, val2)	\
	PIT_TEST_BASETEST(name, {return val1 == val2;})

#define PIT_TEST_DIFFRENCE(name, val1, val2)	\
	PIT_TEST_BASETEST(name, {return val1 != val2;})

#define PIT_TEST_TRUE(name, result)		\
	PIT_TEST_BASETEST(name, {return result;})

#define PIT_TEST_TRY_FUNC(name, func)	\
	PIT_TEST_BASETEST(name, {										\
		bool failed = false;										\
		try {														\
			func();													\
		}															\
		catch (const std::exception& e) {							\
			failed = true;											\
			SetConsoleColor(CONSOLE_RED);							\
			std::cout << " [EXCEPTION]: " << e.what();				\
			SetConsoleColor(CONSOLE_WHITE);							\
		}															\
		catch (...) {												\
			failed = true;											\
			SetConsoleColor(CONSOLE_RED);							\
			std::cout << " [EXCEPTION TRIGGERED]";					\
			SetConsoleColor(CONSOLE_WHITE);							\
		}															\
		return !failed;												\
	})

#define PIT_TEST_TRY_LAMDA(name, lamda)	\
	PIT_TEST_BASETEST(name, {										\
		bool failed = false;										\
		try {														\
			{ lamda	}												\
		}															\
		catch (const std::exception& e) {							\
			failed = true;											\
			SetConsoleColor(CONSOLE_RED);							\
			std::cout << " [EXCEPTION]: " << e.what();				\
			SetConsoleColor(CONSOLE_WHITE);							\
		}															\
		catch (...) {												\
			failed = true;											\
			SetConsoleColor(CONSOLE_RED);							\
			std::cout << " [EXCEPTION TRIGGERED]";					\
			SetConsoleColor(CONSOLE_WHITE);							\
		}															\
		return !failed;												\
	})

}