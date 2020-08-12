#ifndef INCLUDED_STRING_UTIL_H
#define INCLUDED_STRING_UTIL_H

#include <string>
#include <vector>

// tokenizer
 //void Tokenize(const std::string& str,const std::string& sep,std::vector<std::string>& ret);
 void Tokenize(
	 const std::string& str,
	 const std::string& sep,
	 std::vector<std::string>& ret);

// convert from native type to string
 const char * ConvertToString(std::string& str, int8_t t);
 const char * ConvertToString(std::string& str, uint8_t t);
 const char * ConvertToString(std::string& str, int16_t t);
 const char * ConvertToString(std::string& str, uint16_t t);
 const char * ConvertToString(std::string& str, int32_t t);
 const char * ConvertToString(std::string& str, uint32_t t);
 const char * ConvertToString(std::string& str, int64_t t);
 const char * ConvertToString(std::string& str, uint64_t t);
 const char * ConvertToString(std::string& str, double t, int decDigits = 9);
 const char * ConvertToString(std::string& str, bool t);
 const char * ConvertToString(std::string& str, const std::string& t);


// bin to asci hex conversion
 void ConvertBinToAsciiHex(const std::string& bin, std::string& hex, const char pad = ' ');
 void ConvertAsciiHexToBin(const std::string& hex, std::string& bin);

// Function to be used in conjunction to the wildcard library, which resides
// in common

 void ConvertHexWildcardExpr(const std::string& hex,
                                       std::string& bin,
                                       const std::string delim="@");

// output function
 void AppendCommaSepList(std::string& target, const std::vector<std::string>& source, const std::string& andOr);

 std::string StringToLower(const std::string& str);
 std::string StringToUpper(const std::string& str);

 std::string RemoveChars(const std::string& input, const std::string& trimChars);
 std::string Replace(const std::string& input, const std::string& search, const std::string& replace);
 std::string NoCaseReplace(const std::string& input, const std::string& search, const std::string& replace);

 std::string Trim(const std::string& input, const std::string& trimChars);

// string comparisons
int  CaselessStringCmp(const char * s1, const char * s2);
int  CaselessStringCmp(const std::string s1, const std::string s2);
bool CaselessStartsWith(const char * s1, const char * s2); // true if s1 starts with s2
bool CaselessEndsWith(const char * s1, const char * s2); // true if s1 ends with s2
int  CaselessFindBestMatch(const char * str, const char ** table, unsigned int tableSize);
int  CaselessStringDiff(const std::string& s1, const std::string& s2, int max = -1);
LPCWSTR ConvertW(const char* str);
std::string Convert2String(wchar_t*wchar);


bool Contails(std::vector<std::string> vec, std::string key);

#endif  //INCLUDED_STRING_UTIL_H

