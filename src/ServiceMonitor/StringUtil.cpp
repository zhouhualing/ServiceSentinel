#include "StdAfx.h"

#include "StringUtil.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <iomanip>
#include <locale>
#include <sstream>


#ifdef WIN32
    #include <windows.h>
#else
    #include <time.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <memory.h>
    #include <netinet/in.h>
#endif

#ifdef _LINUX_64
    #define SCN_OCT_FMT "%lx%c"
    #define SCN_DEC_FMT "%ld%c"
    #define SCN_UL_FMT  "%lu%c"
    #define SCN_DEC_FMT_NO_C "%ld"
    #define SCN_UL_FMT_NO_C  "%lu"
#else
    #define SCN_OCT_FMT "%llx%c"
    #define SCN_DEC_FMT "%lld%c"
    #define SCN_UL_FMT  "%llu%c"
    #define SCN_DEC_FMT_NO_C "%lld"
    #define SCN_UL_FMT_NO_C  "%llu"
#endif

/* Helper traits template class for int conversion */
template<typename T>
struct IntTraits { };

template<>
struct IntTraits<int8_t>
{
    typedef uint8_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xff;}
    static const char * name() {return "int8";}
};

template<>
struct IntTraits<uint8_t>
{
    typedef uint8_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xff;}
    static const char * name() {return "uint8";}
};

template<>
struct IntTraits<int16_t>
{
    typedef uint16_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xffff;}
    static const char * name() {return "int16";}
};

template<>
struct IntTraits<uint16_t>
{
    typedef uint16_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xffff;}
    static const char * name() {return "uint16";}
};

template<>
struct IntTraits<int32_t>
{
    typedef uint32_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xffffffff;}
    static const char * name() {return "int32";}
};

template<>
struct IntTraits<uint32_t>
{
    typedef uint32_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xffffffff;}
    static const char * name() {return "uint32";}
};


void Tokenize(const std::string& str,const std::string& sep,std::vector<std::string>& ret)
{
	std::string::size_type curPos = 0;
	std::string::size_type prePos = 0;

	while (curPos != std::string::npos)
	{
		prePos = str.find_first_not_of(sep, curPos);
		if (prePos == std::string::npos)
			break;

		curPos = str.find_first_of(sep, prePos);
		assert(curPos >= prePos);
		std::string curToken =
			str.substr(prePos, curPos - prePos);

		if (curToken.empty())
			continue;

		ret.push_back(curToken);
	}
}


const char * ConvertToString(std::string& str, int8_t t)
{
    char tmp[8];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, uint8_t t)
{
    char tmp[8];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, int16_t t)
{
    char tmp[8];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, uint16_t t)
{
    char tmp[8];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, int32_t t)
{
    char tmp[16];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, uint32_t t)
{
    char tmp[16];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, int64_t t)
{
    char tmp[32];
    // xxx todo add support for hex
    sprintf(tmp, SCN_DEC_FMT_NO_C, t);
    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, uint64_t t)
{
    char tmp[32];
    // TODO: add support for hex
    sprintf(tmp, SCN_UL_FMT_NO_C, t);
    str = tmp;
    return str.c_str();
}

const char * ConvertToString(std::string& str, double t, int decDigits)
{
	if (t != t)
	{
		str = "N/A";
		return str.c_str();
	}


	std::ostringstream os;
	os.imbue(std::locale("C"));
	os << std::setprecision(15) << t;

    str = os.str();
    return str.c_str();
}

const char * ConvertToString(std::string& str, bool t)
{
    str = (t? "TRUE":"FALSE");
    return str.c_str();
}

const char * ConvertToString(std::string& str, const std::string& t)
{
    str = t;
    return str.c_str();
}


/*****************************************************************************/

void ConvertBinToAsciiHex(const std::string& bin, std::string& hex, const char pad)
{
    unsigned i = 0;

    const bool insPad = (pad != 0);
    int stepSize = 2;
    int hexSize = bin.size() * stepSize;
    if (insPad)
    {
        ++stepSize;
        if (bin.size() > 1)
            hexSize += (bin.size() - 1);
    }

    hex.resize(hexSize);
    for (i = 0; i < bin.size(); ++i)
    {
        char ch = bin[i];
        ch >>= 4;
        ch &= 0x0F;
        hex[i * stepSize]     = (ch < 10? (ch + '0'): (ch - 10 + 'A'));

        ch = bin[i];
        ch &= 0x0F;
        hex[i * stepSize + 1] = (ch < 10? (ch + '0'): (ch - 10 + 'A'));

        // if need insert pad and not last byte
        if (insPad && ((i + 1) != bin.size()))
        {
            hex[i * stepSize + 2] = pad;
        }
    }
}

void ConvertAsciiHexToBin(const std::string& hex, std::string& bin)
{
    unsigned i = 0;
    std::string nhex;
    std::string run;
    // filter out non hex char
    // we will also need to pad it
    nhex.reserve(hex.size());
    for (i = 0; i < hex.size(); ++i)
    {
        const char ch = hex[i];
        if ((ch >= '0' && ch <= '9') ||
            (ch >= 'A' && ch <= 'F') ||
            (ch >= 'a' && ch <= 'f'))
        {
            run.append(1, ch);
        }
        // if it's not in range we will treat it as pad
        else
        {
            // this is for the case where
            // we have "44:3:22" ==> 44 03 22
            if (!run.empty())
            {
                if (run.size() % 2 != 0)
                {
                    nhex.append(1, '0');
                }
                nhex.append(run);
                run = "";
            }
        }
    }
    // flush additional
    if (!run.empty())
    {
        if (run.size() % 2 != 0)
        {
            nhex.append(1, '0');
        }
        nhex.append(run);
        run = "";
    }

    //ASSERT((nhex.size() % 2) == 0,
    //  "Internal Error: hex string has non-even length");
    assert((nhex.size() % 2) == 0);

    bin.resize(nhex.size()/2);
    for (i = 0; i < bin.size(); ++i)
    {
        std::string str;
        str += nhex[i*2];
        str += nhex[i*2 + 1];

        int val = 0;
        sscanf(str.c_str(), "%x", &val);
        bin[i] = val & 0x000000FF;
    }
}

// Function used in conjunction with the Wildcard library. Used for parsing of
// hexadecimal wildcard expressions
void ConvertHexWildcardExpr(const std::string& hex,
                                            std::string& bin,
                                            const std::string delim)
{
    std::string::size_type curPos = 0, exprBegin, exprEnd;
    std::string seg, segbin, tmpStr, tmpBin;
    char delch;

    delch = delim[0];
    tmpStr = hex;
    tmpBin = "";
    while ((exprBegin = tmpStr.find("@", curPos)) != std::string::npos)

    {
        // Handle everything up to the expression
        if (curPos < exprBegin)
        {
            std::string::size_type atbegin = 0, atpos;
            seg = tmpStr.substr(curPos, exprBegin - curPos);
            ConvertAsciiHexToBin(seg, segbin);
            while ((atpos = segbin.find(delim, atbegin)) != std::string::npos)
            {
                segbin.insert(atpos, 1, delch);
                atbegin = atpos + 2; // Skip two characters
            }
            tmpBin += segbin;
        }
        // Determine expression length
        char ch = tmpStr[exprBegin + 1];
        exprEnd = exprBegin + 1;
        if (ch != delch)
        {
            std::string::size_type pBegin = std::string::npos,
                pEnd = std::string::npos;
            pBegin = tmpStr.find("(", exprBegin + 2);
            if (pBegin == exprBegin + 2)
            {
                pEnd = tmpStr.find(")", exprBegin + 3);
                if (pEnd != std::string::npos)
                    exprEnd = pEnd;
            }
        }
        // Copy the entire expression into the "binary" string
        tmpBin += tmpStr.substr(exprBegin, exprEnd - exprBegin + 1);
        curPos = exprEnd + 1;
    } // End of parse routine
    if (curPos != std::string::npos)
    {
        std::string::size_type atbegin = 0, atpos;
        seg = tmpStr.substr(curPos);
        ConvertAsciiHexToBin(seg, segbin);
        while ((atpos = segbin.find("@", atbegin)) != std::string::npos)
        {
            segbin.insert(atpos, 1, '@');
            atbegin = atpos + 2; // Skip two characters
        }
        tmpBin += segbin;
    } // Tail end sub-expression
    // Copy the new payload
    bin = tmpBin;
}


void AppendCommaSepList(std::string& target, const std::vector<std::string>& source, const std::string& andOr)
{
    typedef std::vector<std::string>::const_iterator sv_citer_t;
    std::ostringstream ostr;

    const bool sizeTwo = source.size() == 2;
    for (sv_citer_t iter = source.begin(); iter != source.end(); ++iter)
    {
        if (iter != source.begin())
        {
            if (!sizeTwo)
                ostr << ", ";
            else
                ostr << " ";

            if (iter + 1 == source.end())
                ostr << andOr << " ";
        }
        ostr << *iter;
    }

    target += ostr.str();
}


/*****************************************************************************/
int  CaselessStringCmp(const char * s1, const char * s2)
{
#ifdef WIN32
    return(_stricmp(s1, s2));
#else
    return(strcasecmp(s1, s2));
#endif
}

int  CaselessStringCmp(const std::string s1, const std::string s2)
{
	return CaselessStringCmp(s1.c_str(),s2.c_str());
}


// Returns true if s1 starts with s2 (caselessly)
bool CaselessStartsWith(const char * s1, const char * s2)
{
    int s1len = strlen(s1);
    int s2len = strlen(s2);

    if (s1len < s2len)
        return false;

    for (int i = 0; i < s2len; ++i, ++s1, ++s2)
    {
        if (tolower(*s1) != tolower(*s2))
            return false;
    }

    return true;
}

// Returns true if s1 end with s2 (caselessly)
bool CaselessEndsWith(const char * s1, const char * s2)
{
    int s1len = strlen(s1);
    int s2len = strlen(s2);

    if (s1len < s2len)
        return false;

    s1 += s1len - s2len;

    for (int i = 0; i < s2len; ++i, ++s1, ++s2)
    {
        if (tolower(*s1) != tolower(*s2))
            return false;
    }

    return true;
}

// returns the index of str in table, including unique abbreviations
// returns -1 if no match found, or -2 if multiple abbreviations are found
int CaselessFindBestMatch(const char * str, const char ** table, unsigned int tableSize)
{
    int numAbbrevs = 0;
    int matchIndex = -1;
    int lenStr = strlen(str);

    for (unsigned int i = 0; i < tableSize; ++i)
    {
        if (CaselessStartsWith(table[i], str))
        {
            if (strlen(table[i]) == lenStr)
                return i;  // identical

            if (++numAbbrevs > 1)
                return -2; // ambiguous input

            matchIndex = i;
        }
    }

    return matchIndex;
}

// Return the number of "edits" (insertions and deletions) needed to
// turn string a into string b case insensitively (or vice versa).
// Implemented based on the algorithm in:
// "An O(ND) Difference Algorithm and Its Variations" (1986) by Eugene W. Myers
// Max limits the max number of edits, if set. If the number needed is
// greater than max, -1 is returned.
int CaselessStringDiff(const std::string& a, const std::string& b, int max)
{
    const int n = a.size();
    const int m = b.size();

    if (max == 0)
    {
        // special case b/c of v[1+max] line below
        return CaselessStringCmp(a.c_str(), b.c_str()) ? -1 : 0;
    }

    if (max < 0 || max >= (m + n))
    {
        max = m + n;
    }

    std::vector<int> v(max*2+1);
    v[1+max] = 0;
    for (int d = 0; d <= max; d++)
    {
        for (int k = -d; k <= d; k += 2)
        {
            int x, y;
            if (k == -d || k != d && v[k-1+max] < v[k+1+max])
            {
                x = v[k+1+max];
            }
            else
            {
                x = v[k-1+max] + 1;
            }
            y = x - k;
            while ((x < n && y < m) &&
                   (tolower(a[x]) == tolower(b[y])))
            {
                ++x;
                ++y;
            }
            v[k+max] = x;
            if (x >= n && y >= m)
                return d;
        }
    }

    return -1;
}


std::string StringToLower(const std::string& str)
{
	std::string result(str);
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}

std::string StringToUpper(const std::string& str)
{
	std::string result(str);
	std::transform(result.begin(), result.end(), result.begin(), toupper);
	return result;
}

std::string RemoveChars(const std::string& input, const std::string& trimChars)
{
	size_t start_pos = 0, end_pos = 0;
	std::string result;
	do
	{
		// Find blocks of valid characters and copy them to the result
		start_pos = input.find_first_not_of(trimChars, start_pos);
		if (start_pos == std::string::npos)
			break;

		end_pos = input.find_first_of(trimChars, start_pos);

		result += input.substr(start_pos,
			(end_pos == std::string::npos) ? end_pos : end_pos - start_pos);
		start_pos = end_pos;
	} while (end_pos != std::string::npos);

	return result;
}

std::string Replace(const std::string& input, const std::string& search, const std::string& replace)
{
	size_t found_pos = 0,
		replace_length = search.size(),
		post_replace_incr = replace.size();

	std::string result = input;

	while ((found_pos = result.find(search, found_pos)) != std::string::npos)
	{
		result.replace(found_pos, replace_length, replace);
		found_pos += post_replace_incr;
	}

	return result;
}

std::string NoCaseReplace(const std::string& input, const std::string& search, const std::string& replace)
{
	// Case-insensitivity is achieved by working on both a tolowered string and the original input
	// Cons: twice as slow, Pros: simple to implement
	size_t found_pos = 0,
		replace_length = search.size(),
		post_replace_incr = replace.size();

	std::string result = input;
	std::string lowResult = StringToLower(input);
	std::string lowSearch = StringToLower(search);

	while ((found_pos = lowResult.find(lowSearch, found_pos)) != std::string::npos)
	{
		result.replace(found_pos, replace_length, replace);
		lowResult.replace(found_pos, replace_length, replace);
		found_pos += post_replace_incr;
	}

	return result;
}

std::string Trim(const std::string& input, const std::string& trimChars)
{
	std::string::size_type pos = input.find_first_not_of(trimChars);
	if (pos == std::string::npos)
		return "";

	std::string output = input.substr(pos);

	pos = output.find_last_not_of(trimChars) + 1;
	if (pos >= output.size())
		return output;

	return output.substr(0, pos);
}


LPCWSTR ConvertW(const char* str)
{
	//int len=str.GetLength();


	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0) + 1;


	wchar_t* pUnicode;
	pUnicode = new wchar_t[unicodeLen];


	memset(pUnicode, 0, (unicodeLen) * sizeof(wchar_t));


	MultiByteToWideChar(CP_ACP, CP_ACP, str, -1, (LPWSTR)pUnicode, unicodeLen);


	return pUnicode;

}
std::string Convert2String( wchar_t*wchar)
{
	std::string szDst;
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);//WideCharToMultiByte的运用
	char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);//WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
	return szDst;
}

bool Contails(std::vector<std::string> vec, std::string key) {
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if (CaselessStringCmp(*it, key) == 0)
			return true;
	}
	return false;
}