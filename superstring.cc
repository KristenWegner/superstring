// superstring.cc - An extended string class built on std::string.
// @kwegner

/*
MIT License

Copyright (c) 2018 Kristen Wegner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <cassert>
#include <cstring>
#include <cstdint>
#include <cfloat>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <cmath>
#include <cstdio>

#include <stdarg.h>
#include <wchar.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <locale>
#include <limits>
#include <regex>
#include <mutex>
#include <map>

#include "string.hh"


namespace super
{
	string::string() : base_type(), _null(false) { }

	string::string(size_t size, char initial) : _null(false), base_type(size, initial) { }

	string::string(const std::string& value) : base_type(value), _null(false) { }

	string::string(const string& value) : base_type(), _null(value._null)
	{
		if (!_null) 
			base_type::operator=(value);
	}

	string::string(string&& value) : base_type(), _null(value._null)
	{
		if (!_null)
			base_type::operator=(value);
	}

	string::string(const char* value) : base_type(), _null(value == nullptr)
	{
		if (!_null)
			base_type::operator=(value);
	}

	string& string::assign(const string& value)
	{
		if (value._null)
		{
			_null = true;
			base_type::clear();
		}
		else
		{
			_null = false;
			base_type::operator=(value);
		}

		return *this;
	}

	string& string::assign(const std::string& value)
	{
		_null = false;
		base_type::operator=(value);

		return *this;
	}

	string& string::assign(const char* value)
	{
		if (value == nullptr)
		{
			_null = true;
			base_type::clear();
		}
		else
		{
			_null = false;
			base_type::operator=(value);
		}

		return *this;
	}

	string& string::operator=(const char* value)
	{
		_null = (value == nullptr);

		if (!_null) 
			base_type::operator=(value);
		else base_type::clear();

		return *this;
	}

	string& string::operator=(const string& value)
	{
		_null = value._null;

		if (!_null) 
			base_type::operator=(value);
		else base_type::clear();

		return *this;
	}

	string& string::operator=(const std::string& value)
	{
		_null = false;

		base_type::operator=(value);

		return *this;
	}

	string& string::operator=(string&& value)
	{
		_null = value._null;

		if (!_null) 
			base_type::operator=(value);
		else base_type::clear();

		value.clear();

		return *this;
	}

	string& string::operator=(std::string&& value)
	{
		_null = false;

		base_type::operator=(value);

		value.clear();

		return *this;
	}

	string::operator const char*() const
	{
		if (_null) 
			return nullptr;
		else return base_type::c_str();
	}

	string::operator const std::string() const { return *this; }

	// Compares this instance with value, from left to right, returning the relative difference.
	// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
	int string::compare_to(const std::string& value) const
	{
		return compare_to(value.c_str());
	}

	// Compares this instance with value, from left to right, returning the relative difference.
	// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
	int string::compare_to(const char* value) const
	{
		if (value == nullptr)
		{
			if (_null) 
				return 0;
			else return 1;
		}

		if (_null) 
			return -1;

		const char* p = base_type::c_str();
		const char* q = value;

		for (; *p && *q; ++p, ++q)
		{
			int d = *p - *q;

			if (d != 0)
				return d;
		}

		if (*p && !*q) 
			return 1;
		else if (!*p && *q) 
			return -1;
		else return 0;
	}

	// Compares this instance with value, from right to left, returning the relative difference.
	// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
	int string::reverse_compare_to(const std::string& value) const
	{
		return reverse_compare_to(value.c_str());
	}

	// Compares this instance with value, from right to left, returning the relative difference.
	// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
	int string::reverse_compare_to(const char* value) const
	{
		if (value == nullptr)
		{
			if (_null) 
				return 0;
			else return 1;
		}

		if (_null)
			return -1;

		size_t m = _null ? 0 : base_type::length();
		size_t n = std::strlen(value);

		const char *p_end = c_str(), *p = p_end + (m - 1);
		const char* q_end = value, *q = q_end + (n - 1);

		for (; p >= p_end && q >= q_end; --p, --q)
		{
			int d = *p - *q;

			if (d != 0)
				return d;
		}

		if (p > p_end && q <= q_end) 
			return 1;
		else if (p <= p_end && q > q_end) 
			return -1;
		else return 0;
	}

	// Computes the Levenshtein distance between this and value.
	unsigned long long string::distance(const std::string& value) const
	{
		size_t m = value.length();
		size_t n = length();

		if (_null || n == 0) return m;
		else if (m == 0) return n;

		m++, n++;

		std::vector<std::vector<unsigned long long>> delta(n);
		
		for (size_t i = 0; i < n; ++i)
		{
			delta[i].resize(m);
			delta[i][0] = i;
		}

		for (size_t i = 0; i < m; ++i)
			delta[0][i] = i;

		const char* const pp = base_type::c_str();
		const char* const qq = value.c_str();

		for (size_t i = 1; i < n; ++i)
		{
			const char p = pp[i - 1];

			for (size_t j = 1; j < m; ++j)
			{
				const char q = qq[j - 1];

				unsigned long long cost = std::min(delta[i - 1][j] + 1, 
					std::min(delta[i][j - 1] + 1, delta[i - 1][j - 1] + ((p == q) ? 0 : 1)));

				if (i > 2 && j > 2)
				{
					unsigned long long t = delta[i - 2][j - 2] + 1;

					if (pp[i - 2] != q) t++;
					if (p != qq[j - 2]) t++;

					if (cost > t) 
						cost = t;
				}

				delta[i][j] = cost;
			}
		}
		
		return delta[n - 1][m - 1];
	}

	// Nullifies this instance.
	string& string::nullify()
	{
		_null = true;
		base_type::clear();

		return *this;
	}

	// Clears this instance.
	string& string::clear()
	{
		_null = false;
		base_type::clear();

		return *this;
	}

	// Clones this instance.
	string string::clone() const
	{
		if (_null) 
			return string(nullptr);
		else return string(*this);
	}

	// True if this has a length of one.
	bool string::is_char() const
	{
		return (!_null || base_type::length() == 1);
	}

	// Gets the first character, or '\0'.
	char string::head() const
	{
		if (is_empty()) return '\0';
		return base_type::operator[](0);
	}

	// Gets the last character, or '\0'.
	char string::tail() const
	{
		if (is_empty()) return '\0';
		return base_type::operator[](base_type::length() - 1);
	}

	// Returns true if value is one of the characters in chars, excluding the null terminator.
	bool string::is_one_of(char value, const char* chars)
	{
		if (chars == nullptr || !*chars) return false;

		for (const char* p = chars; *p; ++p)
			if (value == *p)
				return true;

		return false;
	}

	// Gets the length of this instance in characters, or zero if null.
	size_t string::length() const
	{
		if (_null) 
			return 0;
		else return base_type::length();
	}

	// True if the string is marked as null.
	bool string::is_null() const 
	{ 
		return _null || base_type::c_str() == nullptr; 
	}

	// True if the string is null or zero-length.
	bool string::is_empty() const 
	{ 
		return _null || base_type::length() == 0; 
	}

	// True if the string is not null and not zero-length.
	bool string::non_empty() const 
	{ 
		return !_null && base_type::length() > 0; 
	}

	string string::substring(size_t start, size_t count /*= std::string::npos*/) const
	{
		if (is_empty())
			return clone();

		size_t n = base_type::length();

		if (start >= n)
			return string(nullptr);

		if (count != base_type::npos)
			if ((start + count) > n)
				count = base_type::npos;

		return string(base_type::substr(start, count));
	}

	// Trims leading whitespace, in place.
	string& string::trim_left()
	{
		if (is_empty())
			return *this;

		size_t n = base_type::length();
		size_t i = 0;

		while (i < n && std::isspace(base_type::operator[](i)))
			++i;

		base_type::erase(0, i);

		return *this;
	}

	// Copies this and trims leading whitespace.
	string string::trim_left() const 
	{
		if (is_empty())
			return clone();
		else return clone().trim_left();
	}

	// Trims leading occurences of the specified characters, in place.
	string& string::trim_left(const char* chars)
	{
		if (is_empty() || chars == nullptr)
			return *this;

		size_t n = base_type::length();
		size_t i = 0;

		while (i < n && is_one_of(base_type::operator[](i), chars))
			++i;

		base_type::erase(0, i);

		return *this;
	}

	// Copies this and trims leading whitespace.
	string string::trim_left(const char* chars) const 
	{
		if (is_empty())
			return clone();

		return clone().trim_left(chars);
	}

	// Trims following whitespace, in place.
	string& string::trim_right()
	{
		if (is_empty())
			return *this;

		size_t n = base_type::length();
		size_t i = n;

		while (i > 0 && std::isspace(base_type::operator[](i - 1)))
			--i;

		base_type::erase(i, n);

		return *this;
	}

	// Copies this and trims following whitespace.
	string string::trim_right() const 
	{
		if (is_empty())
			return clone();

		return clone().trim_right();
	}

	// Trims following occurences of the specified characters, in place.
	string& string::trim_right(const char* chars)
	{
		if (is_empty() || chars == nullptr)
			return *this;

		size_t n = base_type::length();
		size_t i = n;

		while (i > 0 && is_one_of(base_type::operator[](i - 1), chars)) 
			--i;

		base_type::erase(i, n);

		return *this;
	}

	// Copies this and trims following occurences of the specified characters.
	string string::trim_right(const char* chars) const 
	{
		if (is_empty()) 
			return clone();
		else return clone().trim_right(chars);
	}

	// Trims all leading and following whitespace, in place.
	string& string::trim() 
	{
		if (is_empty())
			return *this;
		else return trim_left().trim_right(); 
	}

	// Copies this and trims all leading and following whitespace.
	string string::trim() const 
	{
		if (is_empty()) 
			return clone();
		else return clone().trim();
	}

	// Trims all leading and following occurences of the specified characters, in place.
	string& string::trim(const char* chars)
	{
		if (is_empty() || chars == nullptr) 
			return *this;
		else return trim_left(chars).trim_right(chars);
	}

	// Copies this and trims all leading and following occurences of the specified characters.
	string string::trim(const char* chars) const
	{
		if (is_empty())
			return clone();
		else return clone().trim(chars);
	}

	// Transforms this to upper-case, in place.
	string& string::to_upper()
	{
		if (is_empty()) 
			return *this;

		std::transform(base_type::begin(), base_type::end(), base_type::begin(), 
			[](char c) -> char { return char(std::toupper(int(c))); });

		return *this;
	}

	// Copies this and transforms it to upper-case.
	string string::to_upper() const
	{
		if (is_empty())
			return clone();
		else return clone().to_upper();
	}

	// Transforms this to lower-case, in place.
	string& string::to_lower()
	{
		if (is_empty())
			return *this;

		std::transform(base_type::begin(), base_type::end(), base_type::begin(), 
			[](char c) -> char { return char(std::tolower(int(c))); });

		return *this;
	}

	// Copies this and transforms it to lower-case.
	string string::to_lower() const
	{
		if (is_empty())
			return clone();
		else return clone().to_lower();
	}

	// Transforms this using the specified method, in place.
	string& string::transform(char (*method)(char))
	{
		if (is_empty() || method == nullptr) 
			return *this;

		std::transform(base_type::begin(), base_type::end(), base_type::begin(), method);

		return *this;
	}

	// Copies this and transforms it using the specified method.
	string string::transform(char (*method)(char)) const
	{
		if (is_empty() || method == nullptr)
			return clone();
		else return clone().transform(method);
	}

	// Reverses this, in place.
	string& string::reverse()
	{
		if (is_empty())
			return *this;

		std::reverse(base_type::begin(), base_type::end());

		return *this;
	}

	// Copies this and reverses it.
	string string::reverse() const
	{
		if (is_empty())
			return clone();
		else return clone().reverse(); 
	}

	// True if this is composed only of the given characters (excluding the terminating null).
	bool string::consists_of(const char* chars) const
	{
		if (chars == nullptr || is_empty())
			return false;

		for (const char* p = base_type::c_str(); *p; ++p)
			if (!is_one_of(*p, chars))
				return false;

		return true;
	}

	// True if this starts with value. If insensitive is true, then compare caseless.
	bool string::starts_with(const char* value, bool insensitive /*= false*/) const
	{
		if (_null && value == nullptr)
			return true;
		else if (value == nullptr)
			return false;

		string prefix(value);

		if (base_type::length() < prefix.length())
			return false;

		string substring(base_type::substr(0, prefix.length()));

		if (insensitive)
		{
			substring.to_lower();
			prefix.to_lower();
		}

		return substring.compare_to(prefix.c_str()) == 0;
	}

	// True if this ends with value. If insensitive is true, then compare caseless.
	bool string::ends_with(const char* value, bool insensitive /*= false*/) const
	{
		if (_null && value == nullptr)
			return true;
		else if (value == nullptr) 
			return false;

		string suffix(value);

		size_t m = base_type::length();
		size_t n = suffix.length();

		if (m < n)
			return false;

		string substring(base_type::substr(m - n, m));

		if (insensitive)
		{
			substring.to_lower();
			suffix.to_lower();
		}

		return substring.compare_to(suffix.c_str()) == 0;
	}

	// Finds the first index of value, starting at the optional position.
	// Returns the index, or -1 if not found.
	int string::index_of(char value, size_t start /*= std::string::npos*/) const
	{
		if (is_empty()) 
			return -1;

		size_t n = base_type::length();

		if (n == 0 || start >= n) 
			return -1;

		const char* p = base_type::c_str();

		if (start != base_type::npos)
			p += start;

		for (int i = 0; *p; ++p, ++i)
			if (*p == value) 
				return i;

		return -1;
	}

	// Finds the first index of value, from right to left, starting at the optional offset from end.
	// Returns the index, or -1 if not found.
	int string::last_index_of(char value, size_t start /*= std::string::npos*/) const
	{
		if (is_empty())
			return -1;

		size_t n = base_type::length();

		if (start != base_type::npos && start >= n)
			return -1;

		const char* p = base_type::c_str();
		const char* e = p + n;

		e--;

		if (start != base_type::npos)
			e -= start;

		for (int i = n - 1; e > p; --e, --i)
			if (*e == value)
				return i;

		return -1;
	}

	// Replaces the first instance of value, in-place, with substitute. 
	// Returns true if a replacement was made.
	string& string::replace(const std::string& value, const std::string& substitute)
	{
		if (is_empty())
			return *this;

		size_t location = base_type::find(value);

		if (location == base_type::npos)
			return *this;

		base_type::replace(location, value.length(), substitute);

		return *this;
	}

	// Copies and replaces the first instance of value with substitute. 
	// Returns the modified copy.
	string string::replace(const std::string& value, const std::string& substitute) const
	{
		if (is_empty())
			return clone();
		else return clone().replace(value, substitute);
	}

	// Replaces the first instance of value in this with substitute. 
	// Returns true if a replacement was made.
	string& string::replace(const char* value, const char* substitute)
	{
		if (is_empty() || value == nullptr)
			return *this;

		if (substitute == nullptr)
			return replace(std::string(value), std::string());
		else return replace(std::string(value), std::string(substitute));
	}

	// Copies and replaces the first instance of value with substitute. 
	// Returns the modified copy.
	string string::replace(const char* value, const char* substitute) const
	{
		if (is_empty() || value == nullptr)
			return clone();
		else return clone().replace(value, substitute);
	}

	// Replaces all instances of value in this with replacement. 
	// Returns the count of replacements made.
	string& string::replace_all(const std::string& value, const std::string& replacement)
	{
		if (is_empty())
			return *this;

		size_t p = 0;
		size_t m = replacement.length();
		size_t n = value.length();

		while ((p = base_type::find(value, p)) != base_type::npos)
		{
			base_type::replace(p, n, replacement);
			p += m;
		}

		return *this;
	}

	// Copies and replaces all instances of value in the copy with replacement. 
	// Returns the modified copy.
	string string::replace_all(const std::string& value, const std::string& replacement) const
	{
		if (is_empty())
			return clone();
		else return clone().replace_all(value, replacement);
	}

	// Replaces all instances of value in this with replacement. 
	// Returns the count of replacements made.
	string& string::replace_all(const char* value, const char* replacement)
	{
		if (is_empty() || value == nullptr)
			return *this;

		if (replacement == nullptr)
			return replace_all(std::string(value), std::string());
		else return replace_all(std::string(value), std::string(replacement));
	}

	// Copies and replaces all instances of value in the copy with replacement. 
	// Returns the modified copy.
	string string::replace_all(const char* value, const char* replacement) const
	{
		if (is_empty() || value == nullptr)
			return clone();
		else return clone().replace_all(value, replacement);
	}

	// Like strdup. Free with free().
	inline static char* duplicate(const char* value)
	{
		if (value == nullptr)
			return nullptr;

		size_t length = std::strlen(value) + 1;
		char* result = (char*)std::malloc(length);

		if (result == nullptr)
			return nullptr;

		return (char*)std::memcpy(result, value, length);
	}

	// Thread-safe tokenize, like strtok.
	static char* tokenize(char* value, const char* delimiters, char** state)
	{
		char* temp; // Our working pointer.

		// Skip leading delimiters if new string.

		if (value == nullptr)
		{
			value = *state;

			if (value == nullptr) // End?
				return nullptr; // Return end of string indicator.
		}
		else value += std::strspn(value, delimiters); // Get the span of delimiters in value.

		// Find end of segment.

		temp = std::strpbrk(value, delimiters); // Locate any of delimiters in value.

		if (temp != nullptr) // If located.
		{
			// Found another delimiter, split string and save state.

			*temp = '\0'; // Terminate this segment.
			*state = temp + 1;
		}
		else *state = nullptr; // Last segment, remember that.

		return value;
	}

	// Tokenizes this using the single delimiter, and appends the tokens to result vector.
	// Returns the count of tokens appended.
	size_t string::split(char delimiter, std::vector<string>& result) const
	{
		if (delimiter == '\0') return 0;

		char delimiters[2] = { delimiter, '\0' };

		if (_null || base_type::length() == 0)
			return 0;

		char* temp = duplicate(base_type::c_str()); // Make a local writable copy.

		if (temp == nullptr) return 0;

		char* state = nullptr;
		char* token = tokenize(temp, delimiters, &state);
		size_t count = 0;

		while (token != nullptr) // There are more tokens.
		{
			result.push_back(string(token));
			count++;
			token = tokenize(nullptr, delimiters, &state);
		}

		std::free(temp);

		return count;
	}

	// Tokenizes this using the single delimiter, and returns a vector of the tokens.
	std::vector<string> string::split(char delimiter) const
	{
		std::vector<string> result;

		if (delimiter != '\0')
			split(delimiter, result);

		return result;
	}

	// Tokenizes this using the delimiters, and appends the tokens to result vector.
	// Returns the count of tokens appended.
	size_t string::split(const char* delimiters, std::vector<string>& result) const
	{
		if (_null || base_type::length() == 0 || delimiters == nullptr || *delimiters == '\0')
			return 0;

		char* temp = duplicate(base_type::c_str()); // Make a local writable copy.

		if (temp == nullptr) return 0;

		char* state = nullptr;
		char* token = tokenize(temp, delimiters, &state);
		size_t count = 0;

		while (token != nullptr) // There are more tokens.
		{
			result.push_back(string(token));
			count++;
			token = tokenize(nullptr, delimiters, &state);
		}

		std::free(temp);

		return count;
	}

	// Tokenizes this using the delimiters, and appends the tokens to result vector.
	// Returns the count of tokens appended.
	size_t string::split(const std::string& delimiters, std::vector<string>& result) const
	{
		if (delimiters.length() == 0)
			return 0;
		else return split(delimiters.c_str(), result);
	}

	// Tokenizes this using the delimiters, and returns a vector of the tokens.
	std::vector<string> string::split(const char* delimiters) const
	{
		std::vector<string> result;

		if (delimiters != nullptr && *delimiters)
			split(delimiters, result);

		return result;
	}

	// Tokenizes this using the delimiters, and returns a vector of the tokens.
	std::vector<string> string::split(const std::string& delimiters) const
	{
		std::vector<string> result;

		if (delimiters.length() > 0)
			split(delimiters.c_str(), result);

		return result;
	}

	// Tokenizes this using the delimiters, and appends the tokens, and each encountered 
	// delimiter as a separate string, to the result vector. Delimiters in omit, if non-null 
	// will not be retained.
	// Returns the count of tokens (including delimiters) captured.
	size_t string::split_with(const char* delimiters, std::vector<string>& result, const char* omit /*= nullptr*/) const
	{
		if (delimiters == nullptr || *delimiters == '\0' || is_empty())
			return 0;
		
		char* copy;
		char* temp = copy = duplicate(base_type::c_str()); // Make a local writable copy.

		if (temp == nullptr) return 0;

		char token[2] = { '\0', '\0' };
		size_t count = 0;

		// Process any leading delimiters.
		while (*temp && is_one_of(*temp, delimiters))
		{
			if (!(omit && is_one_of(*temp, omit))) // Add it if not omitted.
			{
				token[0] = *temp;
				result.push_back(string(token));
				++count;
			}

			++temp;
		}

		while (*temp)
		{
			char* start = temp; // Mark token start.

			++temp;
			
			while (*temp && !is_one_of(*temp, delimiters)) // Accumulate token characters.
				++temp;

			bool ending = false; // Is there an ending token?

			if (*temp && is_one_of(*temp, delimiters)) 
			{
				token[0] = *temp; // Store any terminating delimiter.
				ending = true;
			}

			bool stop = (!*temp); // If we are at end.

			*temp = 0; // Terminate and store the current non-delimiter token.
			result.push_back(string(start));
			++count;

			if (ending) // Keep any previously stored ending delimiter token.
			{
				if (!(omit && is_one_of(token[0], omit))) // Add it if not omitted.
				{
					result.push_back(string(token));
					++count;
				}
			}

			if (stop) break; // At end so bail out.

			++temp; // Step over the null we made.

			while (*temp && is_one_of(*temp, delimiters)) // Process any trailing delimiters.
			{
				if (!(omit && is_one_of(*temp, omit))) // Add it if not omitted.
				{
					token[0] = *temp;
					result.push_back(string(token));
					++count;
				}

				++temp;
			}
		}

		std::free(copy);

		return count;
	}

	// Tokenizes this using the delimiters, and appends the tokens, and each encountered 
	// delimiter as a separate string, to the result vector.
	// Returns the count of tokens (including delimiters) captured.
	size_t string::split_with(const std::string& delimiters, std::vector<string>& result) const
	{
		size_t n = delimiters.length();
		if (n == 0) return 0;
		else if (n == 1)
			return split_with(delimiters[0], result);
		else return split_with(delimiters.c_str(), result);
	}

	// Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered 
	// delimiter as a separate string, to the result vector.
	std::vector<string> string::split_with(const char* delimiters, const char* omit /*= nullptr*/) const
	{
		std::vector<string> result;

		if (delimiters == nullptr || !*delimiters)
			return result;

		size_t n = std::strlen(delimiters);

		if (n == 0)
			return result;
		else if (n == 1)
			split_with(delimiters[0], result);
		else split_with(delimiters, result, omit);

		return result;
	}

	// Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered 
	// delimiter as a separate string, to the result vector.
	std::vector<string> string::split_with(const std::string& delimiters) const
	{
		std::vector<string> result;

		size_t n = delimiters.length();

		if (n == 0) 
			return result;
		else if (n == 1) 
			split_with(delimiters[0], result);
		else split_with(delimiters.c_str(), result);

		return result;
	}

	// Tokenizes this using the single delimiter, and appends the tokens, and each encountered 
	// delimiter as a separate string, to the result vector.
	// Returns the count of tokens (including delimiters) captured.
	size_t string::split_with(char delimiter, std::vector<string>& result) const
	{
		if (delimiter == '\0' || is_empty())
			return 0;

		char* copy;
		char* temp = copy = duplicate(base_type::c_str()); // Make a local writable copy.

		if (temp == nullptr) return 0;

		char token[2] = { delimiter, '\0' }; // Pre-fab delimiter token we will use.
		size_t count = 0;

		// Process any leading delimiters.
		while (*temp && *temp == delimiter)
		{
			result.push_back(string(token));
			++count;
			++temp;
		}

		while (*temp)
		{
			char* start = temp; // Mark token start.

			++temp;

			while (*temp && *temp != delimiter) // Accumulate token characters.
				++temp;

			bool ending = false; // Is there an ending token?

			if (*temp && *temp == delimiter)
				ending = true;

			bool stop = (!*temp); // If we are at end.

			*temp = 0; // Terminate and store the current non-delimiter token.
			result.push_back(string(start));
			++count;

			if (ending) // Keep any previously stored ending delimiter token.
			{
				result.push_back(string(token));
				++count;
			}

			if (stop) break; // At end so bail out.

			++temp; // Step over the null we made.

			while (*temp && *temp == delimiter) // Process any trailing delimiters.
			{
				result.push_back(string(token));
				++count;
				++temp;
			}
		}

		std::free(copy);

		return count;
	}

	// Tokenizes this using the single delimiter, and returns a vector of the tokens
	// with each separate delimiter as a string, in order.
	std::vector<string> string::split_with(char delimiter) const
	{
		std::vector<string> result;

		if (delimiter != '\0')
			split_with(delimiter, result);

		return result;
	}

	// Returns true if this is formatted as a decimal integer with an optional sign.
	bool string::is_decimal_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim();
		if (value.length() == 0) return false;
		char lead = value[0];
		if (lead == '+' || lead == '-')
			value = value.substr(1);
		return (is_one_of(value[0], "123456789") && value.consists_of("0123456789"));
	}

	// Returns true if this is formatted as a decimal integer with a sign.
	bool string::is_signed_decimal_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim();
		if (value.length() == 0) return false;
		char lead = value[0];
		if (lead != '+' && lead != '-') return false;
		value = value.substr(1);
		return is_one_of(value[0], "123456789") && value.consists_of("0123456789");
	}

	// Returns true if this is formatted as an unsigned decimal integer.
	bool string::is_unsigned_decimal_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim();
		if (value.length() == 0) return false;
		char lead = value[0];
		if (lead == '+' || lead == '-') return false;
		value = value.substr(1);
		return is_one_of(value[0], "123456789") && value.consists_of("0123456789");
	}

	// Returns true if this is formatted as a hexadecimal integer.
	bool string::is_hexadecimal_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim().to_lower();
		if (value.length() == 0) return false;
		if (!value.starts_with("0x")) return false;
		value = value.substring(2);
		return value.consists_of("0123456789abcdef");
	}

	// Returns true if this is formatted as an octal integer with the "0" prefix.
	bool string::is_octal_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim();
		if (length() == 0 || value[0] != '0') return false;
		return value.consists_of("01234567");
	}

	// Returns true if this is formatted as a binary integer with the "0b" prefix.
	bool string::is_binary_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim().to_lower();
		if (value.length() == 0 || !value.starts_with("0b")) return false;
		value = value.substring(2);
		return value.consists_of("01");
	}

	// Returns true if this is formatted as a decimal, hexadecimal, or octal integer.
	bool string::is_integer() const
	{
		if (_null || base_type::length() == 0) return false;
		return 
			is_decimal_integer() || 
			is_hexadecimal_integer() || 
			is_octal_integer() || 
			is_binary_integer();
	}

	// Returns true if this is formatted as a boolean.
	bool string::is_boolean() const
	{
		if (_null || base_type::length() == 0) return false;
		string value = trim().to_lower();
		return (value == "true" || value == "false" || value == "1" || value == "0");
	}

	// Returns true if this is formatted as a decimal floating point value.
	bool string::is_decimal_float() const
	{
		if (_null || base_type::length() == 0)  return false;

		string value = trim();

		if (value.length() == 0)
			return false;

		value.to_lower();

		if (value == "nan" || value == "qnan" || value == "snan")
			return true;

		char lead = value[0];

		if (lead == '+' || lead == '-')
			value = value.substring(1);

		if (value == "inf") return true;

		int dot = value.index_of('.');

		if (dot == -1) return false;

		string whole = value.substring(0, dot);

		if (whole.non_empty() && (!is_one_of(whole[0], "123456789") || !whole.consists_of("0123456789")))
			return false;

		string fraction = value.substring(dot + 1, value.length() - dot);
		string exponent;

		int exp = fraction.index_of('e');

		if (exp > -1)
		{
			exponent = fraction.substr(exp + 1);
			fraction = fraction.substring(0, exp);
		}

		if (!fraction.consists_of("0123456789"))
			return false;

		if (exponent.non_empty())
		{
			if (exponent[0] == '+' || exponent[0] == '-')
				exponent = exponent.substring(1);

			return (is_one_of(exponent[0], "123456789") && exponent.consists_of("0123456789"));
		}
		else return true;
	}

	// Returns true if this is formatted as a hexadecimal floating point value.
	bool string::is_hexadecimal_float() const
	{
		if (_null || base_type::length() == 0)  return false;

		string value = trim();

		if (value.length() == 0)
			return false;

		value.to_lower();

		if (value == "nan" || value == "qnan" || value == "snan")
			return true;

		char lead = value[0];

		if (lead == '+' || lead == '-')
			value = value.substring(1);

		if (value == "inf") return true;

		if (!value.starts_with("0x"))
			return false;

		value = value.substring(2);

		int dot = value.index_of('.');

		if (dot == -1) return false;

		string whole = value.substring(0, dot);

		if (whole.non_empty() && !whole.consists_of("0123456789abcdef"))
			return false;

		string fraction = value.substring(dot + 1, value.length() - dot);
		string exponent;

		int exp = fraction.index_of('p');

		if (exp > -1)
		{
			exponent = fraction.substr(exp + 1);
			fraction = fraction.substring(0, exp);
		}

		if (!fraction.consists_of("0123456789abcdef"))
			return false;

		if (exponent.non_empty())
		{
			if (exponent[0] == '+' || exponent[0] == '-')
				exponent = exponent.substring(1);

			return exponent.consists_of("0123456789abcdef");
		}
		else return true;
	}

	// Returns true if this is formatted as a decimal or hexadecimal floating point value.
	bool string::is_float() const
	{
		return is_decimal_float() || is_hexadecimal_float();
	}

	// Attempts to parse this as an integer value in binary, octal, decimal or hexadecimal format.
	// Returns true if the operation succeeded.
	// If the operation failed, result and negative will not be set.
	bool string::parse_integer(unsigned long long& result, bool& negative) const
	{
		if (_null || base_type::length() == 0) return false;

		string str = trim();

		if (str.length() == 0) return false;

		if (str.is_binary_integer())
		{
			str = str.substring(2);

			const char* start = str.c_str();
			char* end = nullptr;

			unsigned long long val = strtoull(start, &end, 2);

			if ((val == ULLONG_MAX || val == LLONG_MIN) && errno == ERANGE)
				return false;

			negative = false;
			result = val;

			return true;
		}
		else if (str.is_octal_integer())
		{
			str = str.substring(1);

			const char* start = str.c_str();
			char* end = nullptr;

			unsigned long long val = strtoull(start, &end, 8);

			if ((val == ULLONG_MAX || val == 0) && errno == ERANGE)
				return false;

			negative = false;
			result = val;

			return true;
		}
		else if (str.is_decimal_integer())
		{
			bool neg = false;

			if (str[0] == '+' || str[0] == '-')
			{
				neg = (str[0] == '-');
				str = str.substring(1);
			}

			const char* start = str.c_str();
			char* end = nullptr;

			unsigned long long val = strtoull(start, &end, 10);

			if ((val == ULLONG_MAX || val == 0) && errno == ERANGE)
				return false;

			negative = neg;
			result = val;

			return true;
		}
		else if (str.is_hexadecimal_integer())
		{
			str = str.substring(2);

			const char* start = str.c_str();
			char* end = nullptr;
			unsigned long long val = strtoull(start, &end, 16);

			if ((val == ULLONG_MAX || val == 0) && errno == ERANGE)
				return false;

			negative = false;
			result = val;

			return true;
		}

		return false;
	}

	// Attempts to parse this as a floating point number.
	// Returns true if the operation succeeded, and result will be set.
	// If the operation failed, result will not be set.
	bool string::parse_float(long double& result) const
	{
		if (_null || base_type::length() == 0) return false;

		string str = trim();

		if (str.length() == 0) return false;

		str = str.to_lower();

		if (str == "nan" || str == "qnan" || str == "nan?")
		{
			result = std::numeric_limits<long double>::quiet_NaN();
			return true;
		}
		else if (str == "snan")
		{
			result = std::numeric_limits<long double>::signaling_NaN();
			return true;
		}
		else if (str == "inf" || str == "+inf")
		{
			result = std::numeric_limits<long double>::infinity();
			return true;
		}
		else if (str == "-inf")
		{
			result = -std::numeric_limits<long double>::infinity();
			return true;
		}

		if (str.is_decimal_float() || str.is_hexadecimal_float())
		{
			const char* start = str.c_str();
			char* end = nullptr;
			long double val = strtold(start, &end);

			if ((val == HUGE_VALL || val == 0.0L) && errno == ERANGE)
				return false;

			result = val;

			return true;
		}

		return false;
	}

	// Attempts to parse this as a boolean.
	// Returns true if the operation succeeded, and result will be set.
	// If the operation failed, result will not be set.
	bool string::parse_boolean(bool& result) const
	{
		if (_null || base_type::length() == 0) return false;

		string str = trim();

		if (str.length() == 0) return false;

		str.to_lower();

		if (str == "true" || str == "1")
		{
			result = true;
			return true;
		}
		else if (str == "false" || str == "0")
		{
			result = false;
			return true;
		}
		else return false;
	}

	bool string::to_bool() const
	{
		if (_null || trim().length() == 0) 
			return false;
		else if (is_boolean())
		{
			bool boolean = false;

			if (parse_boolean(boolean)) 
				return boolean;
			else return false;
		}
		else if (is_float())
		{
			long double real = 0.0L;

			if (parse_float(real))
			{
				if (std::isnan(real)) 
					return false;
				else if (real == std::numeric_limits<long double>::infinity()) 
					return true;
				else if (real == -std::numeric_limits<long double>::infinity()) 
					return false;
				else return real > 0.0L;
			}
			else return false;
		}
		else if (is_integer())
		{
			unsigned long long integer = 0ULL;
			bool negative = false;

			if (parse_integer(integer, negative))
			{
				if (negative) 
					return false;
				else return integer > 0ULL;
			}
			else return false;
		}
		else return trim().length() > 0;
	}

	// Attempts to parse this as a numeric type, and cast it to the specified type.
	// Returns the parsed numeric value cast to type, if parsed, else one of the arguments.
	// Result may be one of zero, one, minimum, maximum, or nan depending upon the conversion.
	template <class T>
	inline static T parse_cast(const string& value, T zero, T one, T minimum, T maximum, T nan)
	{
		if (value.is_empty())
			return nan;
		else if (value.is_boolean())
		{
			bool boolean = false;

			if (value.parse_boolean(boolean))
				return boolean ? one : zero;
			else return nan;
		}
		else if (value.is_float())
		{
			long double real = 0.0L;

			if (value.parse_float(real))
			{
				if (std::is_floating_point<T>::value)
					return static_cast<T>(real);
				else
				{
					if (std::isnan(real))
						return nan;
					else if (real == std::numeric_limits<long double>::infinity())
						return maximum;
					else if (real == -std::numeric_limits<long double>::infinity())
						return minimum;
					else if (real < long double(minimum))
						return minimum;
					else if (real > long double(maximum))
						return maximum;
					else return static_cast<T>(real);
				}
			}
			else return nan;
		}
		else if (value.is_integer())
		{
			unsigned long long integer = 0ULL;
			bool negative = false;

			if (value.parse_integer(integer, negative))
			{
				if (!negative && integer > unsigned long long(maximum))
					return maximum;
				else if (negative && integer > unsigned long long(maximum))
					return minimum;
				else if (negative)
					return -static_cast<T>(integer);
				else return static_cast<T>(integer);
			}
			else return nan;
		}
		else return nan;
	}

	char string::to_char() const
	{
		if (is_empty()) return '\0';
		else return base_type::operator[](0);
	}

	unsigned char string::to_byte() const { return parse_cast<unsigned char>(*this, 0, 1, 0, 0xFF, 0); }
	short string::to_short() const { return parse_cast<short>(*this, 0, 1, SHRT_MIN, SHRT_MAX, 0); }
	unsigned short string::to_ushort() const { return parse_cast<unsigned short>(*this, 0, 1, 0, USHRT_MAX, 0); }
	int string::to_int() const { return parse_cast<int>(*this, 0, 1, INT_MIN, INT_MAX, 0); }
	unsigned int string::to_uint() const { return parse_cast<unsigned int>(*this, 0U, 1U, 0U, UINT_MAX, 0U); }
	long string::to_long() const { return parse_cast<long>(*this, 0L, 1L, LONG_MIN, LONG_MAX, 0L); }
	unsigned long string::to_ulong() const { return parse_cast<unsigned long>(*this, 0UL, 1UL, 0UL, ULONG_MAX, 0UL); }
	long long string::to_llong() const { return parse_cast<long long>(*this, 0LL, 1LL, LLONG_MIN, LLONG_MAX, 0LL); }
	unsigned long long string::to_ullong() const { return parse_cast<unsigned long long>(*this, 0ULL, 1ULL, 0ULL, ULLONG_MAX, 0ULL); }
	float string::to_float() const { return parse_cast<float>(*this, 0.0F, 1.0F, FLT_MIN, FLT_MAX, NAN); }
	double string::to_double() const { return parse_cast<double>(*this, 0.0, 1.0, DBL_MIN, DBL_MAX, NAN); }
	long double string::to_ldouble() const { return parse_cast<double>(*this, 0.0L, 1.0L, LDBL_MIN, LDBL_MAX, NAN); }

	string& string::append(const std::string& value)
	{
		base_type::append(value);
		_null = false;

		return *this;
	}

	string string::append(const char* value) const
	{
		string result = clone();

		if (value == nullptr || !*value) 
			return result;

		result.append(std::string(value));
		result._null = false;

		return result;
	}

	string string::append(const std::string& value) const
	{
		string result = clone();

		result.append(value);
		result._null = false;

		return result;
	}

	string& string::append(char value)
	{
		if (is_empty() && value == '\0')
		{
			_null = false;

			return *this;
		}

		base_type::push_back(value);

		return *this;
	}

	string string::append(char value) const
	{
		string result = clone();

		if (result.is_empty() && value == '\0')
		{
			result._null = false;

			return result;
		}

		result.push_back(value);

		return result;
	}

	string& string::prepend(const char* value)
	{
		if (value == nullptr || !*value)
			return *this;

		base_type::insert(0, value);
		_null = false;

		return *this;
	}

	string& string::prepend(const std::string& value)
	{
		base_type::insert(0, value);
		_null = false;

		return *this;
	}

	string string::prepend(const char* value) const
	{
		string result = clone();

		if (value == nullptr || !*value)
			return result;

		result.insert(0, value);
		result._null = false;

		return result;
	}

	string string::prepend(const std::string& value) const
	{
		string result = clone();

		result.insert(0, value);
		result._null = false;

		return result;
	}

	string& string::prepend(char value)
	{
		if (is_empty() && value == '\0')
		{
			_null = false;
			return *this;
		}

		base_type::insert(0, 1, value);
		_null = false;

		return *this;
	}

	string string::prepend(char value) const
	{
		string result = clone();

		if (result.is_empty() && value == '\0')
		{
			result._null = false;
			return result;
		}

		result.insert(0, 1, value);
		result._null = false;

		return result;
	}

#if defined(_MSC_VER)
#pragma intrinsic(_rotl64)
#define rotl_64(a, n) _rotl64((a), n)
#else
#if defined(__clang__)
#define rotl_64(a, n) __builtin_rotateleft64((a), n)
#elif defined(__x86_64__)
#define rotl_64(a, n) ({ u64 r; asm("rolq %1,%0" : "=r"(r) : "J"(n),"0"(a) : "cc"); r; })
#else
#define rotl_64(a, n) (((x) << (r)) | ((x) >> (64 - (r))))
#endif
#endif

	inline static uint64_t fmix_64(uint64_t k)
	{
		k ^= k >> 33;
		k *= UINT64_C(0xFF51AFD7ED558CCD);
		k ^= k >> 33;
		k *= UINT64_C(0xC4CEB9FE1A85EC53);
		k ^= k >> 33;

		return k;
	}

	// The following is derived from MurmurHash3 written by Austin Appleby, and is placed in the public domain. 
	// Austin Appleby disclaims copyright to this source code.
	inline static void murmur3_x64_128(const void* value, const size_t bytes, const uint64_t seed, uint64_t* result)
	{
		const uint8_t* data = (const uint8_t*)value;
		const size_t blocks = bytes / UINT64_C(16);

		uint64_t h1 = seed;
		uint64_t h2 = seed;

		const uint64_t c1 = UINT64_C(0x87C37B91114253D5);
		const uint64_t c2 = UINT64_C(0x4CF5AD432745937F);
		const uint64_t* block = (const uint64_t*)data;

		for (size_t i = 0; i < blocks; ++i)
		{
			uint64_t k1 = block[i * 2 + 0];
			uint64_t k2 = block[i * 2 + 1];

			k1 *= c1; k1 = rotl_64(k1, 31); k1 *= c2; h1 ^= k1;
			h1 = rotl_64(h1, 27); h1 += h2; h1 = h1 * UINT64_C(5) + UINT64_C(0x52DCE729);
			k2 *= c2; k2 = rotl_64(k2, 33); k2 *= c1; h2 ^= k2;
			h2 = rotl_64(h2, 31); h2 += h1; h2 = h2 * UINT64_C(5) + UINT64_C(0x38495AB5);
		}

		const uint8_t* tail = (const uint8_t*)(data + blocks * 16);

		uint64_t k1 = UINT64_C(0);
		uint64_t k2 = UINT64_C(0);

		switch (bytes & 15)
		{
		case 15: k2 ^= ((uint64_t)tail[14]) << 48;
		case 14: k2 ^= ((uint64_t)tail[13]) << 40;
		case 13: k2 ^= ((uint64_t)tail[12]) << 32;
		case 12: k2 ^= ((uint64_t)tail[11]) << 24;
		case 11: k2 ^= ((uint64_t)tail[10]) << 16;
		case 10: k2 ^= ((uint64_t)tail[ 9]) <<  8;
		case  9: k2 ^= ((uint64_t)tail[ 8]) <<  0;

			k2 *= c2; 
			k2 = rotl_64(k2, 33); 
			k2 *= c1; 
			h2 ^= k2;

		case  8: k1 ^= ((uint64_t)tail[7]) << 56;
		case  7: k1 ^= ((uint64_t)tail[6]) << 48;
		case  6: k1 ^= ((uint64_t)tail[5]) << 40;
		case  5: k1 ^= ((uint64_t)tail[4]) << 32;
		case  4: k1 ^= ((uint64_t)tail[3]) << 24;
		case  3: k1 ^= ((uint64_t)tail[2]) << 16;
		case  2: k1 ^= ((uint64_t)tail[1]) <<  8;
		case  1: k1 ^= ((uint64_t)tail[0]) <<  0;

			k1 *= c1; 
			k1 = rotl_64(k1, 31); 
			k1 *= c2; 
			h1 ^= k1;
		}

		h1 ^= bytes;
		h2 ^= bytes;

		h1 += h2;
		h2 += h1;

		h1 = fmix_64(h1);
		h2 = fmix_64(h2);

		h1 += h2;
		h2 += h1;

		result[0] = h1;
		result[1] = h2;
	}

	// Computes a 64-bit hash code using the Murmur3 method, with the optional seed.
	// Returns the specified qword part of the 128-bit result.
	unsigned long long string::hash_code(hash_code_part part /*= hash_code_part::low*/, unsigned long long seed /*= 0*/) const
	{
		if (_null) // Special case 1.
			return 0ULL;

		size_t n = base_type::length();

		if (n == 0) // Special case 2.
			return ~0ULL;

		uint64_t result[2] = { 0, 0 };

		murmur3_x64_128(base_type::c_str(), n * sizeof(char), seed, result);

		if (part == hash_code_part::both)
			return result[0] ^ result[1];
		else if (part == hash_code_part::high)
			return result[1];
		else if (part == hash_code_part::low)
			return result[0];
		else return result[0];
	}

	// Formats to the specified buffer. Must be freed using free().
	static bool do_format(char** result, const char* format, va_list ap)
	{
		if (format == nullptr || result == nullptr)
			return false;

		*result = nullptr;

		size_t size = std::strlen(format);
		char* buffer = (char*)std::malloc(size * 2);

		if (buffer == nullptr)
			return false;

		buffer[0] = '\0';

		va_list tp = ap;
		int need = vsnprintf(buffer, size, format, tp);

		if (need < 0)
		{
			std::free(buffer);
			return false;
		}
		else if (need > size)
		{
			size = need + 1;
			char* temp = (char*)std::realloc(buffer, size);

			if (temp == nullptr)
			{
				std::free(buffer);
				return false;
			}
			else buffer = temp;

			buffer[0] = '\0';

			tp = ap;
			need = vsnprintf(buffer, size, format, tp);

			if (need < 0)
			{
				std::free(buffer);
				return false;
			}
		}

		*result = buffer;

		return true;
	}

	// Formats and appends to this using the specified format string, and optional arguments.
	// Returns true if the operation succeeded, else false.
	bool string::append(const char* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		char* buffer = nullptr;
		bool succeeded = do_format(&buffer, format, ap);
		va_end(ap);

		if (succeeded && buffer != nullptr)
		{
			base_type::append(buffer);
			std::free(buffer);

			return true;
		}
		else return false;
	}

	// Attempts to format this using the specified format string, and optional arguments.
	// Returns true if the operation succeeded, else false.
	bool string::format(const char* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		char* buffer = nullptr;
		bool succeeded = do_format(&buffer, format, ap);
		va_end(ap);

		if (succeeded && buffer != nullptr)
		{
			base_type::operator=(buffer);
			std::free(buffer);

			return true;
		}
		else return false;
	}

	string& string::append(bool value)
	{
		static const char* true_str = "true";
		static const char* false_str = "false";
		base_type::append(value ? true_str : false_str);
		return *this;
	}

	string& string::append(unsigned char value)
	{
		char buffer[4] = { '\0' };
		snprintf(buffer, 4, "%u", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(short value)
	{
		char buffer[8] = { '\0' };
		snprintf(buffer, 8, "%d", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(unsigned short value)
	{
		char buffer[8] = { '\0' };
		snprintf(buffer, 8, "%u", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(int value)
	{
		char buffer[16] = { '\0' };
		snprintf(buffer, 16, "%d", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(unsigned int value)
	{
		char buffer[16] = { '\0' };
		snprintf(buffer, 16, "%u", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(long value)
	{
		char buffer[16] = { '\0' };
		snprintf(buffer, 16, "%ld", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(unsigned long value)
	{
		char buffer[16] = { '\0' };
		snprintf(buffer, 16, "%lu", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(long long value)
	{
		char buffer[32] = { '\0' };
		snprintf(buffer, 32, "%lld", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(unsigned long long value)
	{
		char buffer[32] = { '\0' };
		snprintf(buffer, 32, "%llu", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(float value)
	{
		char buffer[32] = { '\0' };
		snprintf(buffer, 32, "%f", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(double value)
	{
		char buffer[32] = { '\0' };
		snprintf(buffer, 32, "%lf", value);
		base_type::append(buffer);
		return *this;
	}

	string& string::append(long double value)
	{
		char buffer[32] = { '\0' };
		snprintf(buffer, 32, "%llf", value);
		base_type::append(buffer);
		return *this;
	}

	// Appends count generated characters to this by calling the specified method count times, 
	// unless method returns '\0', in which case it stops.
	// Returns this after the generation completes.
	string& string::generate(size_t count, char (*method)(size_t index))
	{
		if (method == nullptr)
			return *this;

		base_type::clear();

		for (size_t i = 0; i < count; ++i)
		{
			char c = method(i);

			if (c == '\0')
				return *this;

			append(c);
		}

		return *this;
	}

	// Takes the left-hand count of characters as a substring.
	string string::left(size_t count) const
	{ 
		size_t n = base_type::length();

		if (_null || n == 0 || count == 0) 
			return string();

		if (count > n) count = n;

		return string(base_type::substr(0, count));
	}

	// Takes the right-hand count of characters as a substring.
	string string::right(size_t count) const
	{
		size_t n = base_type::length();

		if (_null || n == 0 || count == 0) 
			return string();

		if (count > n) count = n;

		size_t start = n - count;

		return string(base_type::substr(start, count));
	}

	std::vector<uint8_t> string::get_bytes() const
	{
		if (is_empty())
			return std::vector<uint8_t>();

		const uint8_t m = 0xFF;

		std::vector<uint8_t> result;

		for (char c : *this)
			result.push_back(uint8_t(c & m));
		
		return result;
	}

	// Returns whether this matches the regular expression, using the flags:
	// std::regex_constants::extended | std::regex_constants::nosubs.
	// If caseless is true, or C++17 and multiline is true, adds these flags
	// as well.
	bool string::match(const char* expression, bool caseless /*= false*/
#if __cplusplus > 201402L
		, bool multiline /*= false*/
#endif
	) const
	{
		if (expression == nullptr || is_empty())
			return false;

		auto flags = std::regex_constants::extended;

		if (caseless) flags |= std::regex_constants::icase;

#if __cplusplus > 201402L
		if (multiline) flags |= std::regex_constants::multiline;
#endif
		
		std::regex regex(expression, flags);

		return std::regex_search(*this, regex);
	}

	// Matches the expression like in match above, and returns all of the matches, 
	// or an empty vector if no matches or is_empty.
	std::vector<string> string::match_all(const char* expression, bool caseless /*= false*/
#if __cplusplus > 201402L
		, bool multiline /*= false*/
#endif
	) const
	{
		std::vector<string> result;

		if (expression == nullptr || is_empty())
			return result;

		auto flags = std::regex_constants::extended;

		if (caseless) flags |= std::regex_constants::icase;

#if __cplusplus > 201402L
		if (multiline) flags |= std::regex_constants::multiline;
#endif

		std::regex regex(expression, flags);

		auto begin = std::sregex_iterator(base_type::begin(), base_type::end(), regex);
		auto end = std::sregex_iterator();

		if (std::distance(begin, end) > 0)
		{
			for (auto i = begin; i != end; ++i)
			{
				auto m = *i;

				result.push_back(string(m.str()));
			}
		}

		return result;
	}

	// Computes the total length of an escaped string.
	inline static size_t escaped_length(const string& value)
	{
		size_t result = 0;

		for (const auto& ch : value)
		{
			switch (ch)
			{
			case '"': case '\\': case '\b': case '\f': case '\n': case '\r': case '\t': 
				result += 2; break;
			default:
				result++;
				if (ch <= 0x1F && ch >= 0x00) 
					result += 5;
				break;
			}
		}

		return result;
	}

	// Escapes this string using C-style or JSON escape sequences. Return *this.
	inline static string escape_json(string& value)
	{
		size_t n = value.length();

		if (n == 0) return string(value);

		const size_t size = escaped_length(value);

		if (size == n) return string(value);

		size_t i = 0;
		string result(size, '\\');

		for (const auto& c : value)
		{
			switch (c)
			{
			case  '"': result[i++] = '"'; i++; break;
			case '\\': i += 2; break;
			case '\b': result[i++] = 'b'; i++; break;
			case '\f': result[i++] = 'f'; i++; break;
			case '\n': result[i++] = 'n'; i++; break;
			case '\r': result[i++] = 'r'; i++; break;
			case '\t': result[i++] = 't'; i++; break;
			default:
				if (c >= 0x00 && c <= 0x1F)
				{
					snprintf(&result[i + 1], 6, "u%04X", int(c));
					i += 6;
					result[i] = '\\';
				}
				else result[i++] = c;
				break;
			}
		}

		return result;
	}

	inline static bool parse_octal(const char* src, size_t& i, size_t& n, char* dst, size_t& j)
	{
		if ((long(n) - long(i)) < 2 || !string::is_one_of(src[i], "0123567") || !string::is_one_of(src[i + 1], "0123567")) 
			return false;

		dst[j] = ((src[i] - '0') * 8) + (src[i + 1] - '0');

		char buffer[4] = { src[i + 0], src[i + 1], '\0', '\0' };

		i += 2;
		dst[j] = char(strtoul(buffer, nullptr, 8) & 0xFFU);
		++j;

		return true;
	}

	inline static bool parse_hexadecimal(const char* src, size_t& i, size_t& n, char* dst, size_t& j)
	{
		if ((long(n) - long(i)) < 2) 
			return false;

		char buffer[4] = { src[i + 0], src[i + 1], '\0', '\0' };

		i += 2;
		dst[j] = char(strtoul(buffer, nullptr, 16) & 0xFFU);
		++j;

		return true;
	}

	inline static bool parse_unicode(const char* src, size_t& i, size_t& n, char* dst, size_t& j)
	{
		if ((long(n) - long(i)) < 4) 
			return false;

		char buffer[8] = { src[i], src[i + 1], src[i + 2], src[i + 3], '\0', };

		i += 4;
		*((unsigned short*)&dst[j]) = (strtoul(buffer, nullptr, 16) & 0xFFFFU);
		++j, ++j;

		return true;
	}

	// Removes UTF-8 C-style or JSON escapes from this.
	inline static bool unescape_json(string& value, string& result)
	{
		size_t n = value.length();

		if (n == 0) return string();

		result.clear();
		result.reserve(n);

		const char* src = value.c_str();
		char* dst = &result[0];

		for (size_t i = 0, j = 0; i < n; ++i)
		{
			char c1 = src[i];

			if (c1 == '\\' && (i + 1) < n)
			{
				char c2 = src[i++];

				switch (c2)
				{
				case 'a': dst[j++] = 0x07; break;
				case 'b': dst[j++] = 0x08; break;
				case 'f': dst[j++] = 0x0C; break;
				case 'n': dst[j++] = 0x0A; break;
				case 'r': dst[j++] = 0x0D; break;
				case 't': dst[j++] = 0x09; break;
				case 'v': dst[j++] = 0x0B; break;
				case '\\': dst[j++] = 0x5C; break;
				case '\'': dst[j++] = 0x27; break;
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
					if (!parse_octal(src, i, n, dst, j)) 
						return false; 
					break;
				case 'x': 
					if (!parse_hexadecimal(src, i, n, dst, j)) 
						return false; 
					break;
				case 'u': 
					if (!parse_unicode(src, i, n, dst, j)) 
						return false; 
					break;
				default: 
					dst[j++] = c1, dst[j++] = c2; 
					break;
				}
			}
			else dst[j++] = c1;
		}

		return true;
	}

	static std::mutex string_escape_url_mutex_;
	static bool string_escape_url_initialized_ = false;
	static char string_escape_url_rfc3986_[256] = { 0 };
	static char string_escape_url_html5_[256] = { 0 };
	static const char string_escape_url_hex_[] = "0123456789ABCDEF";

	void string_escape_url_initialize()
	{
		if (string_escape_url_initialized_)
			return;

		string_escape_url_mutex_.lock();

		if (string_escape_url_initialized_)
		{
			string_escape_url_mutex_.unlock();
			return;
		}

		for (size_t i = 0; i < 256; ++i)
		{
			string_escape_url_rfc3986_[i] = 
				std::isalnum(i) || i == '~' || i == '-' || i == '.' || i == '_' ? i : 0;

			string_escape_url_html5_[i] = 
				std::isalnum(i) || i == '*' || i == '-' || i == '.' || i == '_' ? i : i == ' ' ? '+' : 0;
		}

		string_escape_url_initialized_ = true;

		string_escape_url_mutex_.unlock();
	}

	inline static void escape_url_hex(unsigned value, char* result)
	{
		result[0] = string_escape_url_hex_[value % 16U];
		result[1] = string_escape_url_hex_[(value / 16U) % 16U];
	}

	inline static string escape_url(const char* table, string& value)
	{
		string_escape_url_initialize();

		string result;

		if (value.length() == 0)
			return result;

		const char* s = value.c_str();
		char temp[2];

		for (; *s; ++s) 
		{
			temp[0] = '\0';

			if (table[*s])
				result.push_back(table[*s]);
			else
			{
				result.push_back('%');
				escape_url_hex(*s, temp);
				result.push_back(temp[0]);
				result.push_back(temp[1]);
			}
		}

		return result;
	}

	const char* escape_entities_table_[] =
	{
		"nbsp",
		"iexcl",
		"cent",
		"pound",
		"curren",
		"yen",
		"brvbar",
		"sect",
		"uml",
		"copy",
		"ordf",
		"laquo",
		"not",
		"shy",
		"reg",
		"macr",
		"deg",
		"plusmn",
		"sup2",
		"sup3",
		"acute",
		"micro",
		"para",
		"middot",
		"cedil",
		"sup1",
		"ordm",
		"raquo",
		"frac14",
		"frac12",
		"frac34",
		"iquest",
		"Agrave",
		"Aacute",
		"Acirc",
		"Atilde",
		"Auml",
		"Aring",
		"AElig",
		"Ccedil",
		"Egrave",
		"Eacute",
		"Ecirc",
		"Euml",
		"Igrave",
		"Iacute",
		"Icirc",
		"Iuml",
		"ETH",
		"Ntilde",
		"Ograve",
		"Oacute",
		"Ocirc",
		"Otilde",
		"Ouml",
		"times",
		"Oslash",
		"Ugrave",
		"Uacute",
		"Ucirc",
		"Uuml",
		"Yacute",
		"THORN",
		"szlig",
		"agrave",
		"aacute",
		"acirc",
		"atilde",
		"auml",
		"aring",
		"aelig",
		"ccedil",
		"egrave",
		"eacute",
		"ecirc",
		"euml",
		"igrave",
		"iacute",
		"icirc",
		"iuml",
		"eth",
		"ntilde",
		"ograve",
		"oacute",
		"ocirc",
		"otilde",
		"ouml",
		"divide",
		"oslash",
		"ugrave",
		"uacute",
		"ucirc",
		"uuml",
		"yacute",
		"thorn",
		"yuml",
		nullptr
	};

	// Just the ASCII.
	inline static string escape_entities(const string& value)
	{
		string result;

		const char* s = value.c_str();

		for (; *s; ++s)
		{
			switch (*s)
			{
			case '\'': result.append("&apos;"); break;
			case '"': result.append("&quot;"); break;
			case '&': result.append("&amp;"); break;
			case '<': result.append("&lt;"); break;
			case '>': result.append("&gt;"); break;
			case char(0x80): result.append("&euro;"); break;
			case char(0x85): result.append("&hellip;"); break;
			case char(0x86): result.append("&dagger;"); break;
			case char(0x87): result.append("&ddagger;"); break;
			case char(0x89): result.append("&permil;"); break;
			case char(0x95): result.append("&bullet"); break;
			case char(0x96): result.append("&endash;"); break;
			case char(0x97): result.append("&emdash;"); break;
			case char(0x99): result.append("&trade;"); break;
			default:
				if (*s >= char(0xA0))
				{
					result.append('&');
					result.append(escape_entities_table_[(*s - 0xA0U)]);
					result.append(';');
				}
				else result.append(*s);
				break;
			}
		}

		return result;
	}

	typedef struct entity_entry_s
	{
		const char* name;
		unsigned value;
	}
	entity_entry_t;

	static const entity_entry_t string_unescape_entity_names_[] =
	{
		{ "nbsp", 160 },
		{ "iexcl", 161 },
		{ "cent", 162 },
		{ "pound", 163 },
		{ "curren", 164 },
		{ "yen", 165 },
		{ "brvbar", 166 },
		{ "sect", 167 },
		{ "uml", 168 },
		{ "copy", 169 },
		{ "ordf", 170 },
		{ "laquo", 171 },
		{ "not", 172 },
		{ "shy", 173 },
		{ "reg", 174 },
		{ "macr", 175 },
		{ "deg", 176 },
		{ "plusmn", 177 },
		{ "sup2", 178 },
		{ "sup3", 179 },
		{ "acute", 180 },
		{ "micro", 181 },
		{ "para", 182 },
		{ "middot", 183 },
		{ "cedil", 184 },
		{ "sup1", 185 },
		{ "ordm", 186 },
		{ "raquo", 187 },
		{ "frac14", 188 },
		{ "frac12", 189 },
		{ "frac34", 190 },
		{ "iquest", 191 },
		{ "Agrave", 192 },
		{ "Aacute", 193 },
		{ "Acirc", 194 },
		{ "Atilde", 195 },
		{ "Auml", 196 },
		{ "Aring", 197 },
		{ "AElig", 198 },
		{ "Ccedil", 199 },
		{ "Egrave", 200 },
		{ "Eacute", 201 },
		{ "Ecirc", 202 },
		{ "Euml", 203 },
		{ "Igrave", 204 },
		{ "Iacute", 205 },
		{ "Icirc", 206 },
		{ "Iuml", 207 },
		{ "ETH", 208 },
		{ "Ntilde", 209 },
		{ "Ograve", 210 },
		{ "Oacute", 211 },
		{ "Ocirc", 212 },
		{ "Otilde", 213 },
		{ "Ouml", 214 },
		{ "times", 215 },
		{ "Oslash", 216 },
		{ "Ugrave", 217 },
		{ "Uacute", 218 },
		{ "Ucirc", 219 },
		{ "Uuml", 220 },
		{ "Yacute", 221 },
		{ "THORN", 222 },
		{ "szlig", 223 },
		{ "agrave", 224 },
		{ "aacute", 225 },
		{ "acirc", 226 },
		{ "atilde", 227 },
		{ "auml", 228 },
		{ "aring", 229 },
		{ "aelig", 230 },
		{ "ccedil", 231 },
		{ "egrave", 232 },
		{ "eacute", 233 },
		{ "ecirc", 234 },
		{ "euml", 235 },
		{ "igrave", 236 },
		{ "iacute", 237 },
		{ "icirc", 238 },
		{ "iuml", 239 },
		{ "eth", 240 },
		{ "ntilde", 241 },
		{ "ograve", 242 },
		{ "oacute", 243 },
		{ "ocirc", 244 },
		{ "otilde", 245 },
		{ "ouml", 246 },
		{ "divide", 247 },
		{ "oslash", 248 },
		{ "ugrave", 249 },
		{ "uacute", 250 },
		{ "ucirc", 251 },
		{ "uuml", 252 },
		{ "yacute", 253 },
		{ "thorn", 254 },
		{ "yuml", 255 },
		{ "fnof", 402 },
		{ "Alpha", 913 },
		{ "Beta", 914 },
		{ "Gamma", 915 },
		{ "Delta", 916 },
		{ "Epsilon", 917 },
		{ "Zeta", 918 },
		{ "Eta", 919 },
		{ "Theta", 920 },
		{ "Iota", 921 },
		{ "Kappa", 922 },
		{ "Lambda", 923 },
		{ "Mu", 924 },
		{ "Nu", 925 },
		{ "Xi", 926 },
		{ "Omicron", 927 },
		{ "Pi", 928 },
		{ "Rho", 929 },
		{ "Sigma", 931 },
		{ "Tau", 932 },
		{ "Upsilon", 933 },
		{ "Phi", 934 },
		{ "Chi", 935 },
		{ "Psi", 936 },
		{ "Omega", 937 },
		{ "alpha", 945 },
		{ "beta", 946 },
		{ "gamma", 947 },
		{ "delta", 948 },
		{ "epsilon", 949 },
		{ "zeta", 950 },
		{ "eta", 951 },
		{ "theta", 952 },
		{ "iota", 953 },
		{ "kappa", 954 },
		{ "lambda", 955 },
		{ "mu", 956 },
		{ "nu", 957 },
		{ "xi", 958 },
		{ "omicron", 959 },
		{ "pi", 960 },
		{ "rho", 961 },
		{ "sigmaf", 962 },
		{ "sigma", 963 },
		{ "tau", 964 },
		{ "upsilon", 965 },
		{ "phi", 966 },
		{ "chi", 967 },
		{ "psi", 968 },
		{ "omega", 969 },
		{ "thetasym", 977 },
		{ "upsih", 978 },
		{ "piv", 982 },
		{ "bull", 8226 },
		{ "hellip", 8230 },
		{ "prime", 8242 },
		{ "Prime", 8243 },
		{ "oline", 8254 },
		{ "frasl", 8260 },
		{ "weierp", 8472 },
		{ "image", 8465 },
		{ "real", 8476 },
		{ "trade", 8482 },
		{ "alefsym", 8501 },
		{ "larr", 8592 },
		{ "uarr", 8593 },
		{ "rarr", 8594 },
		{ "darr", 8595 },
		{ "harr", 8596 },
		{ "crarr", 8629 },
		{ "lArr", 8656 },
		{ "uArr", 8657 },
		{ "rArr", 8658 },
		{ "dArr", 8659 },
		{ "hArr", 8660 },
		{ "forall", 8704 },
		{ "part", 8706 },
		{ "exist", 8707 },
		{ "empty", 8709 },
		{ "nabla", 8711 },
		{ "isin", 8712 },
		{ "notin", 8713 },
		{ "ni", 8715 },
		{ "prod", 8719 },
		{ "sum", 8721 },
		{ "minus", 8722 },
		{ "lowast", 8727 },
		{ "radic", 8730 },
		{ "prop", 8733 },
		{ "infin", 8734 },
		{ "and", 8743 },
		{ "or", 8744 },
		{ "cap", 8745 },
		{ "cup", 8746 },
		{ "int", 8747 },
		{ "there4", 8756 },
		{ "sim", 8764 },
		{ "cong", 8773 },
		{ "asymp", 8776 },
		{ "ne", 8800 },
		{ "equiv", 8801 },
		{ "le", 8804 },
		{ "ge", 8805 },
		{ "sub", 8834 },
		{ "sup", 8835 },
		{ "nsub", 8836 },
		{ "sube", 8838 },
		{ "supe", 8839 },
		{ "oplus", 8853 },
		{ "otimes", 8855 },
		{ "perp", 8869 },
		{ "sdot", 8901 },
		{ "lceil", 8968 },
		{ "rceil", 8969 },
		{ "lfloor", 8970 },
		{ "rfloor", 8971 },
		{ "lang", 9001 },
		{ "rang", 9002 },
		{ "loz", 9674 },
		{ "spades", 9824 },
		{ "clubs", 9827 },
		{ "hearts", 9829 },
		{ "diams", 9830 },
		{ "quot", 34 },
		{ "amp", 38 },
		{ "lt", 60 },
		{ "gt", 62 },
		{ "OElig", 338 },
		{ "oelig", 339 },
		{ "Scaron", 352 },
		{ "scaron", 353 },
		{ "Yuml", 376 },
		{ "circ", 710 },
		{ "tilde", 732 },
		{ "ensp", 8194 },
		{ "emsp", 8195 },
		{ "thinsp", 8201 },
		{ "zwnj", 8204 },
		{ "zwj", 8205 },
		{ "lrm", 8206 },
		{ "rlm", 8207 },
		{ "ndash", 8211 },
		{ "mdash", 8212 },
		{ "lsquo", 8216 },
		{ "rsquo", 8217 },
		{ "sbquo", 8218 },
		{ "ldquo", 8220 },
		{ "rdquo", 8221 },
		{ "bdquo", 8222 },
		{ "dagger", 8224 },
		{ "Dagger", 8225 },
		{ "permil", 8240 },
		{ "lsaquo", 8249 },
		{ "rsaquo", 8250 },
		{ "euro", 8364 },
		{ nullptr, 0 }
	};

	static bool string_unescape_entity_initialized_ = false;
	static std::mutex string_unescape_entity_mutex_;
	static std::map<std::string, unsigned> string_unescape_entity_map_;

	static void string_unescape_entity_initialize()
	{
		if (string_unescape_entity_initialized_)
			return;

		string_unescape_entity_mutex_.lock();

		if (string_unescape_entity_initialized_)
		{
			string_unescape_entity_mutex_.unlock();

			return;
		}

		const entity_entry_t* p = string_unescape_entity_names_;

		for (;;)
		{
			if (p->name == nullptr)
				break;

			string_unescape_entity_map_.insert(std::pair<std::string, unsigned>(p->name, p->value));

			++p;
		}

		string_unescape_entity_initialized_ = true;
		string_unescape_entity_mutex_.unlock();
	}

	// Expands any known XML/HTML entity references into literal
	// UTF-8 characters.
	inline static std::string unescape_entities(const string& value)
	{
		std::istringstream is(value);
		std::ostringstream os;

		string_unescape_entity_initialize();

		std::string mn;
		unsigned cc;
		bool pc, gc;

		enum class state
		{
			none,
			begin,
			name,
			number,
			decimal,
			hexadecimal,
		}
		ms = state::none;

		for (;;)
		{
			const unsigned char ch = is.get();

			if (is.eof()) 
				break;

			pc = false;
			gc = false;

			switch (ms)
			{
			case state::begin:

				if (ch == '#')
				{
					ms = state::number;
					pc = true;
				}
				else if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
				{
					mn.append(1, ch);
					ms = state::name;
					pc = true;
				}
				else
				{
					os.put('&');
					ms = state::none;
				}

				break;

			case state::name:

				if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9')
				{
					mn.append(1, ch);
					pc = true;
				}
				else if (ch == ';')
				{
					auto e = string_unescape_entity_map_.find(mn);

					if (e != string_unescape_entity_map_.end())
					{
						cc = e->second;
						pc = true;
						gc = true;
					}
				}

				if (!pc)
				{
					os.put('&');

					size_t n = mn.size();

					for (size_t i = 0; i < n; ++i)
						os.put(mn[i]);

					ms = state::none;
				}

				break;

			case state::number:

				if (ch == 'x' || ch == 'X')
				{
					pc = true;
					ms = state::hexadecimal;
					cc = 0;
				}
				else if (ch >= '0' && ch <= '9')
				{
					cc = ch - '0';
					ms = state::decimal;
					pc = true;
				}
				else ms = state::none;

				break;

			case state::decimal:

				if (ch >= '0' && ch <= '9')
				{
					cc = cc * 10 + ch - '0';
					pc = true;
				}
				else if (ch == ';')
				{
					pc = true;
					gc = true;
				}
				else ms = state::none;

				break;

			case state::hexadecimal:

				if (ch >= '0' && ch <= '9')
				{
					cc = cc * 16 + ch - '0';
					pc = true;
				}
				else if (ch >= 'a' && ch <= 'f')
				{
					cc = cc * 16 + ch - 'a' + 10;
					pc = true;
				}
				else if (ch >= 'A' && ch <= 'F')
				{
					cc = cc * 16 + ch - 'A' + 10;
					pc = true;
				}
				else if (ch == ';')
				{
					pc = true;
					gc = true;
				}
				else ms = state::none;

				break;
			}

			if (gc)
			{
				if (cc >= 0x800U)
				{
					os.put(0xE0 | (cc >> 12) & 0x0FU);
					os.put(0x80 | (cc >> 6) & 0x3FU);
					os.put(0x80 | cc & 0x3FU);
				}
				else if (cc >= 0x80U)
				{
					os.put(0xC0 | (cc >> 6) & 0x1FU);
					os.put(0x80 | cc & 0x3FU);
				}
				else os.put(cc);

				ms = state::none;
			}
			else if (!pc && ms == state::none)
			{
				if (ch == '&')
				{
					ms = state::begin;
					mn.erase();
				}
				else os.put(ch);
			}
		}

		return os.str();
	}

	inline static char unescape_url_hex(char value)
	{ return isdigit(value) ? value - char('0') : tolower(value) - char('a') + 10; }

	inline static std::string unescape_url(const string& value)
	{
		std::ostringstream os;

		os.fill('0');

		auto n = value.end();

		for (auto i = value.begin(); i != n; ++i)
		{
			char c = *i;

			if (c == '%') 
			{
				if (i[1] && i[2])
				{
					char h = unescape_url_hex(i[1]) << 4 | unescape_url_hex(i[2]);
					os << h;
					i += 2;
				}
			}
			else if (c == '+') 
				os << ' ';
			else os << c;
		}

		return os.str();
	}

	// Escaping for literal patterns used in regular expressions.
	inline static string escape_regex(const string& value)
	{
		string result;

		for (char c : value)
		{
			bool e = false;

			switch (c)
			{
			case '\t': e = true, c = 't'; break;
			case '\n': e = true, c = 'n'; break;
			case '\r': e = true, c = 'r'; break;
			case '\v': e = true, c = 'v'; break;
			case '\f': e = true, c = 'f'; break;
			case '.': case '^': case '$': case '*': case '+': case '-': case '?':
			case '(': case ')': case '[': case ']': case '{': case '}': case '\\': case '/':
				e = true; break;
			default: break;
			}

			if (e)
			{
				result.append('\\');
				result.append(c);
			}
			else result.append(c);
		}

		return result;
	}

	// Copies this and returns an escaped copy.
	string& string::escape(escaping value)
	{
		switch (value)
		{
		case escaping::json: 
			assign(escape_json(*this)); 
			break;
		case escaping::url: 
			assign(escape_url(string_escape_url_html5_, *this)); 
			break;
		case escaping::rfc3986: 
			assign(escape_url(string_escape_url_rfc3986_, *this)); 
			break;
		case escaping::entities: 
			assign(escape_entities(*this)); 
			break;
		case escaping::regex:
			assign(escape_regex(*this));
			break;
		default: return *this;
		}

		return *this;
	}

	// Removes the specified escaping from this.
	string& string::unescape(escaping value)
	{
		string temp(*this);

		switch (value)
		{
		case escaping::json: 
		{
			string result;
			if (!unescape_json(*this, result))
				return *this;
			assign(result);
			return *this;
		}
		case escaping::rfc3986:
		case escaping::url:
			return assign(unescape_url(*this));
		case escaping::entities:
			return assign(unescape_entities(*this));
		case escaping::regex:
			// TODO
		default: return *this;
		}
	}
};

