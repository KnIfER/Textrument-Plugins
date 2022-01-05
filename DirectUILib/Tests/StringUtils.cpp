/** Test Utils
* https://tessil.github.io/2016/08/29/benchmark-hopscotch-map.html
*	 https://github.com/Tessil/hash-table-shootout
* 
* 
*/

#include "StdAfx.h"


namespace Tests
{
	// TODO When generating random values to insert in the map there is no check
	// to see if duplicate random values are generated. Could improve that (but the probability is so so
	// low and the impact nearly null that it's not really worth it).

	static const std::array<char, 62> ALPHANUMERIC_CHARS = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	/**
	* Generate a vector [0, nb_ints) and shuffle it
	*/
	std::vector<std::int64_t> get_random_shuffle_range_ints(std::size_t nb_ints) {
		std::vector<std::int64_t> random_shuffle_ints(nb_ints);
		std::iota(random_shuffle_ints.begin(), random_shuffle_ints.end(), 0);
		std::shuffle(random_shuffle_ints.begin(), random_shuffle_ints.end(), generator);

		return random_shuffle_ints;
	}


	std::size_t get_memory_usage_bytes() {
		//std::ifstream file("/proc/self/statm");
		//
		//std::size_t memory;
		//file >> memory; // Ignore first
		//file >> memory;
		//
		//return memory * getpagesize();

		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

		return physMemUsedByMe+virtualMemUsedByMe;
	}

	std::string get_random_alphanum_string(std::size_t size) {
		std::uniform_int_distribution<std::size_t> rd_uniform(0, ALPHANUMERIC_CHARS.size() - 1);

		std::uniform_int_distribution<int> RD_1(-8, 8);

		size += RD_1(generator);

		std::string str(size, '\0');
		for(std::size_t i = 0; i < size; i++) {
			str[i] = ALPHANUMERIC_CHARS[rd_uniform(generator)];
		}

		return str;
	}

	std::vector<std::string> get_random_alphanum_strings(std::size_t nb_strings, std::size_t string_size) {
		std::vector<std::string> random_strings(nb_strings);
		for(std::size_t i = 0; i < random_strings.size(); i++) {
			random_strings[i] = get_random_alphanum_string(string_size);
		}

		return random_strings;
	}


	measurements::~measurements()
	{
		const auto chrono_end = std::chrono::high_resolution_clock::now();
		const std::size_t memory_usage_bytes_end = get_memory_usage_bytes();

		const double nb_seconds = std::chrono::duration<double>(chrono_end - m_chrono_start).count();
		// On reads or delete the used bytes could be less than initially.
		const std::size_t used_memory_bytes = (memory_usage_bytes_end > m_memory_usage_bytes_start)?
			memory_usage_bytes_end - m_memory_usage_bytes_start:0;

		std::cout << nb_seconds << " " << used_memory_bytes << " ";

		_buffer.Format(L"measurements::millisec=%.3f, kbmem=%.3f ", nb_seconds*1000, used_memory_bytes/1000.f);
		if (_tasks)
		{
			_buffer += "millisec 均值=";
			QkString format;
			format.Format(L"%.3f", nb_seconds*1000/_tasks);
			_buffer += format;
			_buffer += " ";
		}
		Logger::WriteMessage(_buffer); 
	}



}