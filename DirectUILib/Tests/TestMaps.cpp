#include "StdAfx.h"
#include <unordered_map>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <hash_map>


namespace Tests
{
	const std::int64_t num_keys = 10000;
	const std::int64_t value = 1;

#define INSERT_INT_INTO_HASH(key, value) hash.Insert(key, value)

	TEST_CLASS(Tests)
	{
	public: 
		TEST_METHOD(MapsInsertRandSSO_QK)
		{
			QkStringPtrMap hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, SMALL_STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<QkString> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				wkeys[i] += keys[i].c_str();
			}

			Logger::WriteMessage(L"MapsInsertRandomQK(SSO) "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.Insert(wkeys[i], (LPVOID)values[i]);
			}


			Logger::WriteMessage(_buffer); 
		}

		TEST_METHOD(MapsInsertRandSSO_QK_MAP)
		{
			std::map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, SMALL_STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandomQK(SSO) "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}

		TEST_METHOD(MapsInsertRandSSO_Std)
		{
			std::map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, SMALL_STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandomStd(SSO) "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}

		TEST_METHOD(MapsInsertRandSSO_Unordered)
		{
			std::unordered_map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, SMALL_STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			_buffer.Format(L"bktsz=%d ", hash.bucket_count());
			Logger::WriteMessage(_buffer); 

			Logger::WriteMessage(L"MapsInsertRandUnord(SSO) "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}
		TEST_METHOD(MapsInsertRandSSO_Hash)
		{
			std::hash_map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, SMALL_STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandomHash(SSO) "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}

			_buffer.Format(L"bktsz=%d ", hash.bucket_count());
			Logger::WriteMessage(_buffer); 
		}


		/// <summary>
		/// 
		/// </summary>
		
		TEST_METHOD(MapsInsertRandQK)
		{
			QkStringPtrMap hash;//(127);

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<QkString> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				wkeys[i] = keys[i].c_str();
			}

			Logger::WriteMessage(L"MapsInsertRandomQK "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.Insert(wkeys[i], (LPVOID)values[i]);
				//hash.Insert((&wkeys[i])->GetData(), (LPVOID)values[i]);
			}
		}

		TEST_METHOD(MapsInsertRandomStd)
		{
			std::map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandomStd "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}

		TEST_METHOD(MapsInsertRandomUnordered)
		{
			std::unordered_map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandUnord "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}

		TEST_METHOD(MapsInsertRandomHash)
		{
			std::hash_map<wstring, LPVOID> hash;

			const std::vector<std::string> keys = get_random_alphanum_strings(num_keys, STRING_SIZE);
			const std::vector<std::int64_t> values = get_random_shuffle_range_ints(num_keys);
			std::vector<std::wstring> wkeys; wkeys.resize(keys.size());
			for (size_t i = 0; i < keys.size(); i++)
			{
				_buffer = keys[i].c_str();
				wkeys[i] = _buffer;
			}

			Logger::WriteMessage(L"MapsInsertRandUnord "); 
			measurements m;
			for(std::int64_t i = 0; i < num_keys; i++) {
				hash.insert(pair<wstring, LPVOID>(wkeys[i], (LPVOID)values[i]));
			}
		}


	};


}
