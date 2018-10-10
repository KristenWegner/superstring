# superstring

A super string class for C++

```
                        .    .
                         )  (
   _ _ _ _ _ _ _ _ _ _ _(.--.)
 {{ { { { { { { { { { { ( '_')
  >>>>>>>>>>>>>>>>>>>>>>>`--'>
```

This class implements a kitchen sink full of extra features and functions, including tokenization, parsing, and others, similar to Java or Scala's String class.

The next update (?) will include unit tests, UTF-8 support, and per-project feature sets. For now, this is unoptimized and *untested*, so use it at your own peril. Much of this code consists of various bits and pieces, and functions I have aggregated over the years from one project or another. If you are writing mission-critical code, you might be better served using the [boost::string](https://www.boost.org/doc/libs/1_68_0/doc/html/string_algo.html) library.

This is Protected by the MIT License, and is Copyright (c) 2018 by Kristen Wegner.

Methods:

<div>Simple assignment.</div>

<pre>string& assign(const string& value)</pre>
<pre>string& assign(const std::string& value)</pre>
<pre>string& assign(const char* value)</pre>

<div>Assignment operators.</div>

<pre>string& operator=(const char* value)</pre>
<pre>string& operator=(const string& value)</pre>
<pre>string& operator=(const std::string& value)</pre>
<pre>string& operator=(string&& value)</pre>
<pre>string& operator=(std::string&& value)</pre>

<div>Compares this instance with value, from left to right, returning the relative difference. Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.</div>

<pre>int compare_to(const char* value) const</pre>

<div>Compares this instance with value, from left to right, returning the relative difference. Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.</div>

<pre>int compare_to(const std::string& value) const</pre>

<div>Compares this instance with value, from right to left, returning the relative difference. Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.</div>

<pre>int reverse_compare_to(const char* value) const</pre>

<div>Compares this instance with value, from right to left, returning the relative difference. Returns 1 if this is greater than value, -1 if value is greater than this, else 0 if they are equal.</div>

<pre>int reverse_compare_to(const std::string& value) const</pre>

<div>Computes the Levenshtein distance between this and value.</div>

<pre>unsigned long long distance(const std::string& value) const</pre>

<div>Computes a 128-bit hash code using the Murmur3 method, with the optional seed. Returns the specified qword part of the 128-bit result. Part defaults to the low qword, with a seed of zero.</div>

<pre>unsigned long long hash_code(hash_code_part part = hash_code_part::low, unsigned long long seed = 0) const</pre>

<div>Nullifies this instance.</div>

<pre>string& nullify()</pre>

<div>Clears this, returning this.</div>

<pre>string& clear()</pre>

<div>Clones this instance.</div>

<pre>string clone() const</pre>

<div>Returns true if value is one of the characters in chars, excluding the null terminator.</div>

<pre>static bool is_one_of(char value, const char* chars)</pre>

<div>Gets the length of this instance in characters, or zero if null.</div>

<pre>size_t length() const</pre>

<div>True if the string is marked as null.</div>

<pre>bool is_null() const</pre>

<div>True if the string is null or zero-length.</div>

<pre>bool is_empty() const</pre>

<div>True if the string is not null and not zero-length.</div>

<pre>bool non_empty() const</pre>

<div>True if this has a length of one.</div>

<pre>bool is_char() const</pre>

<div>Gets the first character, or '\0'.</div>

<pre>char head() const</pre>

<div>Gets the last character, or '\0'.</div>

<pre>char tail() const</pre>

<div>Returns whether this matches the regular expression.</div>

<pre>bool match(const char* expression, bool caseless = false) const</pre>

<div>Matches the expression like in match above, and returns all of the matches, or an empty vector if no matches or is_empty.</div>

<pre>std::vector<string> match_all(const char* expression, bool caseless = false) const</pre>

<div>Using the supplied regular expression, assigns the captured groups to the arguments in order (like scanf). Returns the count of parameters converted.</div>

<pre>
template &lt;typename T, typename... Args&gt;
size_t parse(const char* expression, T& first, Args&... args) const
</pre>

<div>Using the supplied regular expression, strictly assigns the captured groups to the arguments in order (like scanf). Returns the count of parameters converted.</div>

<pre>
template &lt;typename T, typename... Args&gt;
size_t parse(bool strict, const char* expression, T& first, Args&... args) const
</pre>

<div>Gets the substring copy at start, with count characters.</div>

<pre>string substring(size_t start, size_t count = std::string::npos) const</pre>

<div>Trims leading whitespace, in place.</div>

<pre>string& trim_left()</pre>

<div>Copies this and trims leading whitespace.</div>

<pre>string trim_left() const</pre>

<div>Trims leading occurences of the specified characters, in place.</div>

<pre>string& trim_left(const char* chars)</pre>

<div>Copies this and trims leading whitespace.</div>

<pre>string trim_left(const char* chars) const</pre>

<div>Trims following whitespace, in place.</div>

<pre>string& trim_right()</pre>

<div>Copies this and trims following whitespace.</div>

<pre>string trim_right() const</pre>

<div>Trims following occurences of the specified characters, in place.</div>

<pre>string& trim_right(const char* chars)</pre>

<div>Copies this and trims following occurences of the specified characters.</div>

<pre>string trim_right(const char* chars) const</pre>

<div>Trims all leading and following whitespace, in place.</div>

<pre>string& trim()</pre>

<div>Copies this and trims all leading and following whitespace.</div>

<pre>string trim() const</pre>

<div>Trims all leading and following occurences of the specified characters, in place.</div>

<pre>string& trim(const char* chars)</pre>

<div>Copies this and trims all leading and following occurences of the specified characters.</div>

<pre>string trim(const char* chars) const</pre>

<div>Transforms this to upper-case, in place.</div>

<pre>string& to_upper()</pre>

<div>Copies this and transforms it to upper-case.</div>

<pre>string to_upper() const</pre>

<div>Transforms this to lower-case, in place.</div>

<pre>string& to_lower()</pre>

<div>Copies this and transforms it to lower-case.</div>

<pre>string to_lower() const</pre>

<div>Transforms this using the specified method, in place.</div>

<pre>string& transform(char (*method)(char))</pre>

<div>Copies this and transforms it using the specified method.</div>

<pre>string transform(char (*method)(char)) const</pre>

<div>Appends count generated characters to this by calling the specified method count times, unless method returns '\0', in which case it stops. Returns this after the generation completes.</div>

<pre>string& generate(size_t count, char (*method)(size_t index))</pre>

<div>Reverses this, in place.</div>

<pre>string& reverse()</pre>

<div>Copies this and reverses it.</div>

<pre>string reverse() const</pre>

<div>Takes the left-hand count of characters as a substring.</div>

<pre>string left(size_t count) const</pre>

<div>Takes the right-hand count of characters as a substring.</div>

<pre>string right(size_t count) const</pre>

<div>True if this is composed only of the given characters (excluding the terminating null).</div>

<pre>bool consists_of(const char* chars) const</pre>

<div>True if this starts with value. If insensitive is true, then compare caseless.</div>

<pre>bool starts_with(const char* value, bool insensitive = false) const</pre>

<div>True if this ends with value. If insensitive is true, then compare caseless.</div>

<pre>bool ends_with(const char* value, bool insensitive = false) const</pre>

<div>Finds the first index of value, from left to right, starting at the optional offset. Returns the index, or -1 if not found.</div>

<pre>int index_of(char value, size_t start = std::string::npos) const</pre>

<div>Finds the last index of value, from right to left, starting at the optional offset from end. Returns the index, or -1 if not found.</div>

<pre>int last_index_of(char value, size_t start = std::string::npos) const</pre>

<div>Replaces the first instance of value in this with substitute. Returns this modified instance.</div>

<pre>string& replace(const std::string& value, const std::string& substitute)</pre>

<div>Copies and replaces the first instance of value in with substitute. Returns the modified copy.</div>

<pre>string replace(const std::string& value, const std::string& substitute) const</pre>

<div>Replaces the first instance of value, in-place, with substitute. Returns this modified instance.</div>

<pre>string& replace(const char* value, const char* substitute)</pre>

<div>Copies and replaces the first instance of value in with substitute. Returns the modified copy.</div>

<pre>string replace(const char* value, const char* substitute) const</pre>

<div>Replaces all instances of value in this with replacement. Returns this modified instance.</div>

<pre>string& replace_all(const std::string& value, const std::string& replacement)</pre>

<div>Copies and replaces all instances of value in the copy with replacement. Returns the modified copy.</div>

<pre>string replace_all(const std::string& value, const std::string& replacement) const</pre>

<div>Replaces all instances of value in this with replacement. Returns this modified instance.</div>

<pre>string& replace_all(const char* value, const char* replacement)</pre>

<div>Copies and replaces all instances of value in the copy with replacement. Returns the modified copy.</div>

<pre>string replace_all(const char* value, const char* replacement) const</pre>

<div>Appends the specified value to this instance, returning this.</div>

<pre>string& append(const std::string& value)</pre>
<pre>string append(const char* value) const</pre>
<pre>string append(const std::string& value) const</pre>
<pre>string& append(char value)</pre>
<pre>string append(char value) const</pre>
<pre>string& append(bool value)</pre>
<pre>string& append(unsigned char value)</pre>
<pre>string& append(short value)</pre>
<pre>string& append(unsigned short value)</pre>
<pre>string& append(int value)</pre>
<pre>string& append(unsigned int value)</pre>
<pre>string& append(long value)</pre>
<pre>string& append(unsigned long value)</pre>
<pre>string& append(long long value)</pre>
<pre>string& append(unsigned long long value)</pre>
<pre>string& append(float value)</pre>
<pre>string& append(double value)</pre>
<pre>string& append(long double value)</pre>

<div>Formats and appends to this using the specified format string, and optional arguments. Returns true if the operation succeeded, else false.</div>

<pre>bool append(const char* format, ...)</pre>

<div>Prepends the specified value to this instance, returning this.</div>

<pre>string& prepend(const char* value)</pre>
<pre>string& prepend(const std::string& value)</pre>
<pre>string prepend(const char* value) const</pre>
<pre>string prepend(const std::string& value) const</pre>
<pre>string& prepend(char value)</pre>
<pre>string prepend(char value) const</pre>

<div>Attempts to format this using the specified format string, and optional arguments. Overwrites any existing value with the formatted result. Returns true if the operation succeeded, else false.</div>

<pre>bool format(const char* format, ...)</pre>

<div>Tokenizes this using the single delimiter, and appends the tokens to result vector. Returns the count of tokens appended.</div>

<pre>size_t split(char delimiter, std::vector<string>& result) const</pre>

<div>Tokenizes this using the single delimiter, and returns a vector of the tokens.</div>

<pre>std::vector<string> split(char delimiter) const</pre>

<div>Tokenizes this using the delimiters, and appends the tokens to result vector. Returns the count of tokens appended.</div>

<pre>size_t split(const char* delimiters, std::vector<string>& result) const</pre>

<div>Tokenizes this using the delimiters, and appends the tokens to result vector. Returns the count of tokens appended.</div>

<pre>size_t split(const std::string& delimiters, std::vector<string>& result) const</pre>

<div>Tokenizes this using the delimiters, and returns a vector of the tokens.</div>

<pre>std::vector<string> split(const char* delimiters) const</pre>

<div>Tokenizes this using the delimiters, and returns a vector of the tokens.</div>

<pre>std::vector<string> split(const std::string& delimiters) const</pre>

<div>Tokenizes this using the delimiters, and appends the tokens, and each encountered delimiter as a separate string, to the result vector. Returns the count of tokens (including delimiters) captured.</div>

<pre>size_t split_with(const char* delimiters, std::vector<string>& result, const char* omit = nullptr) const</pre>

<div>Tokenizes this using the delimiters, and appends the tokens, and each encountered delimiter as a separate string, to the result vector. Returns the count of tokens (including delimiters) captured.</div>

<pre>size_t split_with(const std::string& delimiters, std::vector<string>& result) const</pre>

<div>Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered delimiter as a separate string, to the result vector.</div>

<pre>std::vector<string> split_with(const char* delimiters, const char* omit = nullptr) const</pre>

<div>Tokenizes this using the delimiters, and returns a vector of the tokens, and each encountered delimiter as a separate string, to the result vector.</div>

<pre>std::vector<string> split_with(const std::string& delimiters) const</pre>

<div>Tokenizes this using the single delimiter, and appends the tokens, and each encountered delimiter as a separate string, to the result vector. Returns the count of tokens (including delimiters) captured.</div>

<pre>size_t split_with(char delimiter, std::vector<string>& result) const</pre>

<div>Tokenizes this using the single delimiter, and returns a vector of the tokens with each separate delimiter as a string, in order.</div>

<pre>std::vector<string> split_with(char delimiter) const</pre>

<div>Escapes this string. Return *this.</div>

<pre>string& escape(escaping value)</pre>

<div>Copies this and returns an escaped copy.</div>

<pre>string escape(escaping value) const</pre>

<div>Removes C-style escapes from this.</div>

<pre>string& unescape(escaping value)</pre>

<div>Copies then removes C-style escapes from this.</div>

<pre>string unescape(escaping value) const</pre>

<div>Returns true if this is formatted as a decimal integer with an optional sign.</div>

<pre>bool is_decimal_integer() const</pre>

<div>Returns true if this is formatted as a decimal integer with a sign.</div>

<pre>bool is_signed_decimal_integer() const</pre>

<div>Returns true if this is formatted as an unsigned decimal integer.</div>

<pre>bool is_unsigned_decimal_integer() const</pre>

<div>Returns true if this is formatted as a hexadecimal integer.</div>

<pre>bool is_hexadecimal_integer() const</pre>

<div>Returns true if this is formatted as an octal integer with the "0" prefix.</div>

<pre>bool is_octal_integer() const</pre>

<div>Returns true if this is formatted as a binary integer with the "0b" prefix.</div>

<pre>bool is_binary_integer() const</pre>

<div>Returns true if this is formatted as a decimal, hexadecimal, or octal integer.</div>

<pre>bool is_integer() const</pre>

<div>Returns true if this is formatted as a boolean.</div>

<pre>bool is_boolean() const</pre>

<div>Returns true if this is formatted as a decimal floating point value.</div>

<pre>bool is_decimal_float() const</pre>

<div>Returns true if this is formatted as a hexadecimal floating point value.</div>

<pre>bool is_hexadecimal_float() const</pre>

<div>Returns true if this is formatted as a decimal or hexadecimal floating point value.</div>

<pre>bool is_float() const</pre>

<div>Determines whether value can be converted to the given type without overflow.</div>

<pre>
template &lt;typename T&gt;
static bool is_strictly(const string& value)
</pre>

<div>Attempts to parse this as an integer value in binary, octal, decimal or hexadecimal format. Returns true if the operation succeeded. If the operation failed, result and negative will not be set.</div>

<pre>bool parse_integer(unsigned long long& result, bool& negative) const</pre>

<div>Attempts to parse this as a floating point number. Returns true if the operation succeeded, and result will be set. If the operation failed, result will not be set.</div>

<pre>bool parse_float(long double& result) const</pre>

<div>Attempts to parse this as a boolean. Returns true if the operation succeeded, and result will be set. If the operation failed, result will not be set.</div>

<pre>bool parse_boolean(bool& result) const</pre>

<pre>std::vector<uint8_t> get_bytes() const</pre>

<div>Conversions.</div>

<pre>bool to_bool() const</pre>
<pre>char to_char() const</pre>
<pre>unsigned char to_byte() const</pre>
<pre>short to_short() const</pre>
<pre>unsigned short to_ushort() const</pre>
<pre>int to_int() const</pre>
<pre>unsigned int to_uint() const</pre>
<pre>unsigned long to_ulong() const</pre>
<pre>long long to_llong() const</pre>
<pre>unsigned long long to_ullong() const</pre>
<pre>float to_float() const</pre>
<pre>double to_double() const</pre>
<pre>long double to_ldouble() const</pre>

<div>Casts.</div>

<pre>operator const char*() const</pre>
<pre>operator const std::string() const</pre>
<pre>operator bool() const</pre>
<pre>operator char() const</pre> 
<pre>operator unsigned char() const</pre> 
<pre>operator short() const</pre> 
<pre>operator unsigned short() const</pre> 
<pre>operator int() const</pre> 
<pre>operator unsigned int() const</pre> 
<pre>operator long() const</pre> 
<pre>operator unsigned long() const</pre> 
<pre>operator long long() const</pre> 
<pre>operator unsigned long long() const</pre> 
<pre>operator float() const</pre> 
<pre>operator double() const</pre> 
<pre>operator long double() const</pre> 

Last Updated: 2018-10-10.

@kwegner
