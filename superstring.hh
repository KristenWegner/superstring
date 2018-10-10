// superstring.hh - An extended string class built on std::string.
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


#ifndef INCLUDE_SUPERSTRING_HH
#define INCLUDE_SUPERSTRING_HH 1


#include <string>
#include <vector>
#include <regex>
#include <type_traits>
#include <limits>
#include <cstdint>


namespace super
{
	// Hash code part for 128 bit hashes.
	enum class hash_code_part
	{
		// Low qword.
		low,

		// High qword.
		high,

		// XOR of low, high.
		both
	};

	class string : public std::string
	{
	public:

		// Our base type.
		typedef std::string base_type;

	private:

		bool _null;

	public:

		string();
		string(size_t size, char initial);
		string(const string& value);
		string(string&& value);
		explicit string(const char* value);
		explicit string(const std::string& value);

	public:

		// Simple assignment.
		string& assign(const string& value);
		string& assign(const std::string& value);
		string& assign(const char* value);

		string& operator=(const char* value);
		string& operator=(const string& value);
		string& operator=(const std::string& value);
		string& operator=(string&& value);
		string& operator=(std::string&& value);

		operator const char*() const;
		operator const std::string() const;

	public:

		// Compares this instance with value, from left to right, returning the relative difference.
		// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
		int compare_to(const char* value) const;

		// Compares this instance with value, from left to right, returning the relative difference.
		// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
		int compare_to(const std::string& value) const;

		// Compares this instance with value, from right to left, returning the relative difference.
		// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
		int reverse_compare_to(const char* value) const;

		// Compares this instance with value, from right to left, returning the relative difference.
		// Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.
		int reverse_compare_to(const std::string& value) const;

		// Computes the Levenshtein distance between this and value.
		unsigned long long distance(const std::string& value) const;

		// Computes a 128-bit hash code using the Murmur3 method, with the optional seed.
		// Returns the specified qword part of the 128-bit result.
		// Part defaults to the low qword, with a seed of zero.
		unsigned long long hash_code(hash_code_part part = hash_code_part::low, unsigned long long seed = 0) const;

	public:

		// Nullifies this instance.
		string& nullify();

		// Clears this, returning this.
		string& clear();

		// Clones this instance.
		string clone() const;

	public:

		// Returns true if value is one of the characters in chars, excluding the null terminator.
		static bool is_one_of(char value, const char* chars);

	public:

		// Gets the length of this instance in characters, or zero if null.
		size_t length() const;

		// True if the string is marked as null.
		bool is_null() const;

		// True if the string is null or zero-length.
		bool is_empty() const;

		// True if the string is not null and not zero-length.
		bool non_empty() const;

		// True if this has a length of one.
		bool is_char() const;

		// Gets the first character, or '\0'.
		char head() const;

		// Gets the last character, or '\0'.
		char tail() const;

	public:

		// Returns whether this matches the regular expression, using the flags:
		// std::regex_constants::extended.
		// If caseless is true, or C++17 and multiline is true, adds these flags
		// as well.
		bool match(const char* expression, bool caseless = false
#if __cplusplus > 201402L
			, bool multiline = false
#endif
		) const;

		// Matches the expression like in match above, and returns all of the matches, 
		// or an empty vector if no matches or is_empty.
		std::vector<string> match_all(const char* expression, bool caseless = false
#if __cplusplus > 201402L
			, bool multiline = false
#endif
		) const;

	private:

		size_t collect(bool strict, std::smatch& parts, size_t& count, size_t& index) const { return index; }

		// Strictly collects elements for parse.
		template <typename T, typename... Args>
		size_t collect(bool strict, std::smatch& parts, size_t& count, size_t& index, T& value, Args&... args) const
		{
			++index;
			if (index < count)
			{
				string str(parts[index].str());
				if (strict && !is_strictly<T>(str)) // Validate it.
					return index - 1;
				value = T(str);
				return collect(strict, parts, count, index, args ...);
			}
			else return index;
		}

	public:

		// Using the supplied regular expression, assigns the captured groups to the 
		// arguments in order (like scanf).
		// Returns the count of parameters converted.
		template<typename T, typename... Args>
		size_t parse(const char* expression, T& first, Args&... args) const
		{
			std::regex regex;
			
			try { regex = std::regex(expression); }
			catch (std::regex_error& e) 
			{
				std::cerr << e.what() << std::endl;
				return 0; 
			}

			std::smatch parts;

			if (!std::regex_search(*this, parts, regex))
				return 0;

			size_t count = parts.size();
			size_t index = 0;

			if (count > 1)
			{
				++index;
				first = T(string(parts[index].str()));
				if (count > index)
					return collect(false, parts, count, index, args ...);
				else return 1;
			}
			else return 0;
		}

		// Using the supplied regular expression, strictly assigns the captured groups to the 
		// arguments in order (like scanf).
		// Returns the count of parameters converted.
		template<typename T, typename... Args>
		size_t parse(bool strict, const char* expression, T& first, Args&... args) const
		{
			std::regex regex;

			try { regex = std::regex(expression); }
			catch (std::regex_error& e)
			{
				std::cerr << e.what() << std::endl;
				return 0;
			}

			std::smatch parts;

			if (!std::regex_search(*this, parts, regex))
				return 0;

			size_t count = parts.size();
			size_t index = 0;

			if (count > 1)
			{
				++index;
				string str(parts[index].str());
				if (strict && !is_strictly<T>(str))
					return index - 1;
				first = T(str);
				if (count > index)
					return collect(strict, parts, count, index, args ...);
				else return 1;
			}
			else return 0;
		}

	public:

		// Gets the substring copy at start, with count characters.
		string substring(size_t start, size_t count = std::string::npos) const;

		// Trims leading whitespace, in place.
		string& trim_left();

		// Copies this and trims leading whitespace
		string trim_left() const;

		// Trims leading occurences of the specified characters, in place.
		string& trim_left(const char* chars);

		// Copies this and trims leading whitespace.
		string trim_left(const char* chars) const;

		// Trims following whitespace, in place.
		string& trim_right();

		// Copies this and trims following whitespace.
		string trim_right() const;

		// Trims following occurences of the specified characters, in place.
		string& trim_right(const char* chars);

		// Copies this and trims following occurences of the specified characters.
		string trim_right(const char* chars) const;

		// Trims all leading and following whitespace, in place.
		string& trim();

		// Copies this and trims all leading and following whitespace.
		string trim() const;

		// Trims all leading and following occurences of the specified characters, in place.
		string& trim(const char* chars);

		// Copies this and trims all leading and following occurences of the specified characters.
		string trim(const char* chars) const;

		// Transforms this to upper-case, in place.
		string& to_upper();

		// Copies this and transforms it to upper-case.
		string to_upper() const;

		// Transforms this to lower-case, in place.
		string& to_lower();

		// Copies this and transforms it to lower-case.
		string to_lower() const;

		// Transforms this using the specified method, in place.
		string& transform(char (*method)(char));

		// Copies this and transforms it using the specified method.
		string transform(char (*method)(char)) const;

		// Appends count generated characters to this by calling the specified method count times, 
		// unless method returns '\0', in which case it stops.
		// Returns this after the generation completes.
		string& generate(size_t count, char (*method)(size_t index));

		// Reverses this, in place.
		string& reverse();

		// Copies this and reverses it.
		string reverse() const;

		// Takes the left-hand count of characters as a substring.
		string left(size_t count) const;

		// Takes the right-hand count of characters as a substring.
		string right(size_t count) const;

		// True if this is composed only of the given characters (excluding the terminating null).
		bool consists_of(const char* chars) const;

		// True if this starts with value. If insensitive is true, then compare caseless.
		bool starts_with(const char* value, bool insensitive = false) const;

		// True if this ends with value. If insensitive is true, then compare caseless.
		bool ends_with(const char* value, bool insensitive = false) const;

		// Finds the first index of value, from left to right, starting at the optional offset.
		// Returns the index, or -1 if not found.
		int index_of(char value, size_t start = std::string::npos) const;

		// Finds the last index of value, from right to left, starting at the optional offset from end.
		// Returns the index, or -1 if not found.
		int last_index_of(char value, size_t start = std::string::npos) const;

		// Replaces the first instance of value in this with substitute. 
		// Returns this modified instance.
		string& replace(const std::string& value, const std::string& substitute);

		// Copies and replaces the first instance of value in with substitute. 
		// Returns the modified copy.
		string replace(const std::string& value, const std::string& substitute) const;

		// Replaces the first instance of value, in-place, with substitute. 
		// Returns this modified instance.
		string& replace(const char* value, const char* substitute);

		// Copies and replaces the first instance of value in with substitute. 
		// Returns the modified copy.
		string replace(const char* value, const char* substitute) const;

		// Replaces all instances of value in this with replacement. 
		// Returns this modified instance.
		string& replace_all(const std::string& value, const std::string& replacement);

		// Copies and replaces all instances of value in the copy with replacement. 
		// Returns the modified copy.
		string replace_all(const std::string& value, const std::string& replacement) const;

		// Replaces all instances of value in this with replacement. 
		// Returns this modified instance.
		string& replace_all(const char* value, const char* replacement);

		// Copies and replaces all instances of value in the copy with replacement. 
		// Returns the modified copy.
		string replace_all(const char* value, const char* replacement) const;

		// Appends the specified value to this instance, returning this.
		string& append(const std::string& value);
		string append(const char* value) const;
		string append(const std::string& value) const;
		string& append(char value);
		string append(char value) const;
		string& append(bool value);
		string& append(unsigned char value);
		string& append(short value);
		string& append(unsigned short value);
		string& append(int value);
		string& append(unsigned int value);
		string& append(long value);
		string& append(unsigned long value);
		string& append(long long value);
		string& append(unsigned long long value);
		string& append(float value);
		string& append(double value);
		string& append(long double value);

		// Formats and appends to this using the specified format string, and optional arguments.
		// Returns true if the operation succeeded, else false.
		bool append(const char* format, ...);

		// Prepends the specified value to this instance, returning this.
		string& prepend(const char* value);
		string& prepend(const std::string& value);
		string prepend(const char* value) const;
		string prepend(const std::string& value) const;
		string& prepend(char value);
		string prepend(char value) const;

		// Attempts to format this using the specified format string, and optional arguments.
		// Overwrites any existing value with the formatted result.
		// Returns true if the operation succeeded, else false.
		bool format(const char* format, ...);

	public:

		// Tokenizes this using the single delimiter, and appends the tokens to result vector.
		// Returns the count of tokens appended.
		size_t split(char delimiter, std::vector<string>& result) const;

		// Tokenizes this using the single delimiter, and returns a vector of the tokens.
		std::vector<string> split(char delimiter) const;

		// Tokenizes this using the delimiters, and appends the tokens to result vector.
		// Returns the count of tokens appended.
		size_t split(const char* delimiters, std::vector<string>& result) const;

		// Tokenizes this using the delimiters, and appends the tokens to result vector.
		// Returns the count of tokens appended.
		size_t split(const std::string& delimiters, std::vector<string>& result) const;

		// Tokenizes this using the delimiters, and returns a vector of the tokens.
		std::vector<string> split(const char* delimiters) const;

		// Tokenizes this using the delimiters, and returns a vector of the tokens.
		std::vector<string> split(const std::string& delimiters) const;

		// Tokenizes this using the delimiters, and appends the tokens, and each encountered 
		// delimiter as a separate string, to the result vector.
		// Returns the count of tokens (including delimiters) captured.
		size_t split_with(const char* delimiters, std::vector<string>& result, const char* omit = nullptr) const;

		// Tokenizes this using the delimiters, and appends the tokens, and each encountered 
		// delimiter as a separate string, to the result vector.
		// Returns the count of tokens (including delimiters) captured.
		size_t split_with(const std::string& delimiters, std::vector<string>& result) const;

		// Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered 
		// delimiter as a separate string, to the result vector.
		std::vector<string> split_with(const char* delimiters, const char* omit = nullptr) const;

		// Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered 
		// delimiter as a separate string, to the result vector.
		std::vector<string> split_with(const std::string& delimiters) const;

		// Tokenizes this using the single delimiter, and appends the tokens, and each encountered 
		// delimiter as a separate string, to the result vector.
		// Returns the count of tokens (including delimiters) captured.
		size_t split_with(char delimiter, std::vector<string>& result) const;

		// Tokenizes this using the single delimiter, and returns a vector of the tokens
		// with each separate delimiter as a string, in order.
		std::vector<string> split_with(char delimiter) const;

	public:

		// Supported escape sequences.
		enum class escaping
		{
			// An undefined escape; just pass-through.
			undefined,

			// UTF-8 JSON or C-style escaping with reverse solidus.
			json,

			// RFC 3986 URL-encoded escaping.
			rfc3986,

			// HTML 5 URL-encoded escaping.
			url,

			// XML or HTML-encoded entity escaping.
			entities,

			// Escaping for literal patterns used in regular expressions.
			regex
		};

		// Escapes this string. Return *this.
		string& escape(escaping value);

		// Copies this and returns an escaped copy.
		string escape(escaping value) const;

		// Removes C-style escapes from this.
		string& unescape(escaping value);

		// Copies then removes C-style escapes from this.
		string unescape(escaping value) const;

	public:

		// Returns true if this is formatted as a decimal integer with an optional sign.
		bool is_decimal_integer() const;

		// Returns true if this is formatted as a decimal integer with a sign.
		bool is_signed_decimal_integer() const;

		// Returns true if this is formatted as an unsigned decimal integer.
		bool is_unsigned_decimal_integer() const;

		// Returns true if this is formatted as a hexadecimal integer.
		bool is_hexadecimal_integer() const;

		// Returns true if this is formatted as an octal integer with the "0" prefix.
		bool is_octal_integer() const;

		// Returns true if this is formatted as a binary integer with the "0b" prefix.
		bool is_binary_integer() const;

		// Returns true if this is formatted as a decimal, hexadecimal, or octal integer.
		bool is_integer() const;

		// Returns true if this is formatted as a boolean.
		bool is_boolean() const;

		// Returns true if this is formatted as a decimal floating point value.
		bool is_decimal_float() const;

		// Returns true if this is formatted as a hexadecimal floating point value.
		bool is_hexadecimal_float() const;

		// Returns true if this is formatted as a decimal or hexadecimal floating point value.
		bool is_float() const;

		// Determines whether value can be converted to the given type without overflow.
		template <typename T>
		inline static bool is_strictly(const string& value)
		{
			if (std::is_arithmetic<T>::value)
			{
				if (std::numeric_limits<T>::is_integer)
				{
					if (std::is_signed<T>::value)
					{
						unsigned long long v = 0;
						
						const intmax_t vl = intmax_t(std::numeric_limits<T>::min());
						const intmax_t vh = intmax_t(std::numeric_limits<T>::max());
						bool n = false;
						if ((value.is_decimal_integer() ||
							value.is_signed_decimal_integer() ||
							value.is_unsigned_decimal_integer() ||
							value.is_hexadecimal_integer() ||
							value.is_octal_integer() ||
							value.is_binary_integer()) &&
							value.parse_integer(v, n))
							return v >= nl && v <= vh;
						else return false;
					}
					else
					{
						unsigned long long v;
						const unsigned long long vh = std::numeric_limits<T>::max();
						bool n = false;
						if ((value.is_unsigned_decimal_integer() ||
							value.is_hexadecimal_integer() ||
							value.is_octal_integer() ||
							value.is_binary_integer()) &&
							value.parse_integer(v, n))
							return v <= vh;
						else return false;
					}
				}
				else if (std::is_floating_point<T>::value)
				{
					long double v = 0.0;
					const long double vl = long double(std::numeric_limits<T>::min());
					const long double vh = long double(std::numeric_limits<T>::max());
					if ((value.is_decimal_integer() ||
						value.is_signed_decimal_integer() ||
						value.is_unsigned_decimal_integer() ||
						value.is_decimal_float() ||
						value.is_hexadecimal_float()) &&
						value.parse_float(v))
						return v >= vl && v <= vh;
					else return false;
				}
				else if (std::is_same<T, decltype('c')>::value)
				{
					if (value.is_char()) return true;
					else
					{
						unsigned long long v;
						bool n = false;
						if ((value.is_unsigned_decimal_integer() ||
							value.is_hexadecimal_integer() ||
							value.is_octal_integer() ||
							value.is_binary_integer()) &&
							value.parse_integer(v, n))
							return v >= 0ULL && v <= 0xFFULL;
						else return false;
					}
				}
				else if (std::is_same<T, decltype(false)>::value)
					return (value.is_boolean() || (value.length() == 1 && (value.head() == '1' || value.head() == '0')));
				else return false;
			}
			else if (std::is_same<T, std::string>::value)
				return !value.is_null();
			else if (std::is_same<T, const char*>::value)
				return true;
			else if (std::is_same<T, string>::value)
				return true;
			else return false;
		}

	public:

		// Attempts to parse this as an integer value in binary, octal, decimal or hexadecimal format.
		// Returns true if the operation succeeded.
		// If the operation failed, result and negative will not be set.
		bool parse_integer(unsigned long long& result, bool& negative) const;

		// Attempts to parse this as a floating point number.
		// Returns true if the operation succeeded, and result will be set.
		// If the operation failed, result will not be set.
		bool parse_float(long double& result) const;

		// Attempts to parse this as a boolean.
		// Returns true if the operation succeeded, and result will be set.
		// If the operation failed, result will not be set.
		bool parse_boolean(bool& result) const;

	public:

		std::vector<uint8_t> get_bytes() const;

		bool to_bool() const;
		char to_char() const;
		unsigned char to_byte() const;
		short to_short() const;
		unsigned short to_ushort() const;
		int to_int() const;
		unsigned int to_uint() const;
		long to_long() const;
		unsigned long to_ulong() const;
		long long to_llong() const;
		unsigned long long to_ullong() const;
		float to_float() const;
		double to_double() const;
		long double to_ldouble() const;

		inline operator bool() const { return to_bool(); }
		inline operator char() const { return to_char(); }
		inline operator unsigned char() const { return to_byte(); }
		inline operator short() const { return to_short(); }
		inline operator unsigned short() const { return to_ushort(); }
		inline operator int() const { return to_int(); }
		inline operator unsigned int() const { return to_uint(); }
		inline operator long() const { return to_long(); }
		inline operator unsigned long() const { return to_ulong(); }
		inline operator long long() const { return to_llong(); }
		inline operator unsigned long long() const { return to_ullong(); }
		inline operator float() const { return to_float(); }
		inline operator double() const { return to_double(); }
		inline operator long double() const { return to_ldouble(); }
	};
};


#endif // INCLUDE_SUPERSTRING_HH

