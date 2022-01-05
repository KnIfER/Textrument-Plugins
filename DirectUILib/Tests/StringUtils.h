#pragma once

namespace Tests
{
	__declspec(selectany) QkString _buffer;


	/**
	* SMALL_STRING_SIZE should be small enough so that there is no heap allocation when a std::string is created.
	*/
	static const std::size_t SMALL_STRING_SIZE = 15;
	static const std::size_t STRING_SIZE = 50;

	static const std::int64_t SEED = 0;
	static std::mt19937_64 generator(SEED);


	/**
	* Generate a vector [0, nb_ints) and shuffle it
	*/
	std::vector<std::int64_t> get_random_shuffle_range_ints(std::size_t nb_ints);

	std::size_t get_memory_usage_bytes();

	std::string get_random_alphanum_string(std::size_t size);

	std::vector<std::string> get_random_alphanum_strings(std::size_t nb_strings, std::size_t string_size);

	class measurements {
	public:    
		measurements(): m_memory_usage_bytes_start(get_memory_usage_bytes())
			,m_chrono_start(std::chrono::high_resolution_clock::now())
			,_tasks(0)
		{
		}

		~measurements();

		void SetTaskCnt(int tasks) {
			_tasks = tasks;
		};

	private:    
		std::size_t m_memory_usage_bytes_start;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_chrono_start;
		std::size_t _tasks;
	};
}