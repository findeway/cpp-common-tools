#include "Encoder.h"
#include "utf8.hpp"
namespace inner
{
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";
	// static const std::wstring base64_wchars = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ" L"abcdefghijklmnopqrstuvwxyz" L"0123456789+/";

    inline bool is_base64( unsigned char c ) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	inline bool w_is_base64( unsigned short c ) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
}

std::wstring urlencode(const std::wstring& original)
{
	std::string strbase;
	strbase = W2Utf8(original.c_str());
	std::string strout;
	unsigned char c;
	for (size_t i = 0; i < strbase.size(); ++i)
	{
		c = strbase.at(i);
		if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			strout.append(1,c);
		}
// 		else if(c == ' ')
// 		{
// 			strout.append(1,'+');
// 		}
		else
		{
			strout.append(1,'%');
			strout.append(1,(c >= 0xA0) ? ((c >> 4) - 10 + 'A') : ((c >> 4) + '0'));
			strout.append(1,((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + 'A') : ((c & 0xF) + '0'));
		}
	}
	return Utf82W(strout.c_str());
}

std::wstring urldecode(const std::wstring& original)
{
	std::string strbase;
	strbase = W2Utf8(original.c_str());
	std::string strout;
	size_t i = 0;
	unsigned char c;
	while (i < strbase.size())
	{
		c = strbase.at(i);
		if(c == '%')
		{
			int c1 = 0;
			//高位
			if(i + 1 < strbase.size())
			{
				if(strbase.at(i+1) >= 'A' && strbase.at(i+1) <= 'F')
					c1 += (strbase.at(i+1) - 'A' + 10) * 0x10;
				else if(strbase.at(i+1) >= 'a' && strbase.at(i+1) <= 'f')
					c1 += (strbase.at(i+1) - 'a' + 10) * 0x10;
				else
					c1 += (strbase.at(i+1) - '0') * 0x10;
			}
			//低位
			if(i + 2 < strbase.size())
			{
				if(strbase.at(i+2) >= 'A' && strbase.at(i+2) <= 'F')
					c1 += (strbase.at(i+2) - 'A' + 10);
				else if(strbase.at(i+2) >= 'a' && strbase.at(i+2) <= 'f')
					c1 += (strbase.at(i+2) - 'a' + 10);
				else
					c1 += (strbase.at(i+2) - '0');
			}
			//
			i += 3;
			strout.append(1,c1);
		}
		else if(c == '+')
		{
			strout.append(1,'+');
			++i;
		}
		else
		{
			strout.append(1,c);
			++i;
		}
	}
	return Utf82W(strout.c_str());
}

unsigned int hex2int(char c)
{
	if(c > '0' && c < '9')
	{
		return (unsigned int)(c-48);
	}
	else if(c >= 'A' && c <= 'F')
	{
		return (unsigned int)(c-55);
	}
	else if(c >= 'a' && c <= 'f')
	{
		return (unsigned int)(c-87);
	}
	return 0;
}

std::wstring hex2string(const char* hex)
{
	int result_index = 0;
	std::string result_string;
	result_string.resize(strlen(hex));
	for(int j = 0;j < strlen(hex) - 1;)
	{
		unsigned int high = hex2int(hex[j++]);
		unsigned int low = hex2int(hex[j++]);
		result_string[result_index++] = char(high * 16 + low);
	}
	result_string[result_index] = '\0';
	return Utf82W(result_string.c_str());
}

std::wstring string2hex(const char* string)
{
	std::string original_string = string;
	std::string hex_string;
	hex_string.resize(2*original_string.size() + 1);
	const char* hex_base = "0123456789abcdef";
	int result_index = 0;
	for(int i = 0; i< original_string.size(); i++)
	{
		unsigned char hex = (unsigned char)original_string[i];
		hex_string[result_index++] = hex_base[(hex&0xf0)>>4];
		hex_string[result_index++] = hex_base[(hex&0x0f)];
	}
	hex_string[result_index] = '\0';
	return Utf82W(hex_string.c_str());
}

std::wstring escape(const std::wstring& source)
{
	std::string base = W2Utf8(source.c_str());
	std::string result_string;
	result_string.resize((base.size() + 1)*10);
	u8_escape((char*)result_string.data(), result_string.size(), base.c_str(),0);
	return Utf82W(result_string.c_str());
}

std::wstring unescape(const std::wstring& source)
{
	std::string base = W2Utf8(source.c_str());
	std::string result_string;
	result_string.resize(base.size());
	u8_unescape((char*)result_string.data(),result_string.size(),base.c_str());
	return Utf82W(result_string.c_str());
}

std::string convert2base64(const std::string& sourcestring)
{
	 std::string ret;
	 const unsigned char* bytes_to_encode = reinterpret_cast< const unsigned char* >( sourcestring.data() );
	 int in_len = sourcestring.size();
	 int i = 0;
     int j = 0;
 	 unsigned char char_array_3[3];
     unsigned char char_array_4[4];

	 while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 1) {
		    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		    char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
		   	  ret += inner::base64_chars[char_array_4[i]];
			i = 0;
		}
	 }

	 if (i)
	 {
		for(j = i; j < 3; j++)
		   char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

	    for (j = 0; (j < i + 1); j++)
			 ret += inner::base64_chars[char_array_4[j]];

		while((i++ < 3))
			 ret += '=';

	 }
	 return ret;
}

std::string convertfrombase64(const std::string& base64)
{
	int in_len = base64.size();
    int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( base64[in_] != '=') && inner::is_base64(base64[in_])) {
	   char_array_4[i++] = base64[in_]; in_++;
	   if (i ==4) {
			for (i = 0; i <4; i++)
				 char_array_4[i] = inner::base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				 ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		 for (j = i; j <4; j++)
			 char_array_4[j] = 0;

		 for (j = 0; j <4; j++)
			 char_array_4[j] = inner::base64_chars.find(char_array_4[j]);

		 char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		 char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		 char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		 for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
	return ret;
}

//md5实现
typedef unsigned int uint32;

/* MD5 context. */
typedef struct _MD5_CTX
{
    uint32 state[4];				/* state (ABCD) */
    uint32 count[2];				/* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];	/* input buffer */
} MD5_CTX;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform(uint32[4], const unsigned char[64]);
static void Encode(unsigned char *, uint32 *, unsigned int);
static void Decode(uint32 *, const unsigned char *, unsigned int);

static unsigned char PADDING[64] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac) { (a) += F ((b), (c), (d)) + (x) + (uint32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) { (a) += G ((b), (c), (d)) + (x) + (uint32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) { (a) += H ((b), (c), (d)) + (x) + (uint32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) { (a) += I ((b), (c), (d)) + (x) + (uint32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }

void MD5Init(MD5_CTX * context)
{
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX * context,const unsigned char *input,
               unsigned int inputLen)
{
    unsigned int i, index, partLen;

    index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

    if ((context->count[0] += ((uint32) inputLen << 3))
            < ((uint32) inputLen << 3))
        context->count[1]++;
    context->count[1] += ((uint32) inputLen >> 29);

    partLen = 64 - index;

    if (inputLen >= partLen)
    {
        memcpy
        ((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context->state, &input[i]);

        index = 0;
    }
    else
        i = 0;

    memcpy
    ((unsigned char*) & context->buffer[index], (unsigned char*) & input[i],
     inputLen - i);
}

void MD5Final(unsigned char digest[16], MD5_CTX * context)
{
    unsigned char bits[8];
    unsigned int index, padLen;

    Encode(bits, context->count, 8);

    index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);

    MD5Update(context, bits, 8);

    Encode(digest, context->state, 16);

    memset((unsigned char*) context, 0, sizeof(*context));
}

static void MD5Transform(uint32 state[4], const unsigned char block[64])
{
    uint32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d, x[0], S11, 0xd76aa478);	/* 1 */
    FF(d, a, b, c, x[1], S12, 0xe8c7b756);	/* 2 */
    FF(c, d, a, b, x[2], S13, 0x242070db);	/* 3 */
    FF(b, c, d, a, x[3], S14, 0xc1bdceee);	/* 4 */
    FF(a, b, c, d, x[4], S11, 0xf57c0faf);	/* 5 */
    FF(d, a, b, c, x[5], S12, 0x4787c62a);	/* 6 */
    FF(c, d, a, b, x[6], S13, 0xa8304613);	/* 7 */
    FF(b, c, d, a, x[7], S14, 0xfd469501);	/* 8 */
    FF(a, b, c, d, x[8], S11, 0x698098d8);	/* 9 */
    FF(d, a, b, c, x[9], S12, 0x8b44f7af);	/* 10 */
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);		/* 11 */
    FF(b, c, d, a, x[11], S14, 0x895cd7be);		/* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122);		/* 13 */
    FF(d, a, b, c, x[13], S12, 0xfd987193);		/* 14 */
    FF(c, d, a, b, x[14], S13, 0xa679438e);		/* 15 */
    FF(b, c, d, a, x[15], S14, 0x49b40821);		/* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[1], S21, 0xf61e2562);	/* 17 */
    GG(d, a, b, c, x[6], S22, 0xc040b340);	/* 18 */
    GG(c, d, a, b, x[11], S23, 0x265e5a51);		/* 19 */
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);	/* 20 */
    GG(a, b, c, d, x[5], S21, 0xd62f105d);	/* 21 */
    GG(d, a, b, c, x[10], S22, 0x2441453);	/* 22 */
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);		/* 23 */
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);	/* 24 */
    GG(a, b, c, d, x[9], S21, 0x21e1cde6);	/* 25 */
    GG(d, a, b, c, x[14], S22, 0xc33707d6);		/* 26 */
    GG(c, d, a, b, x[3], S23, 0xf4d50d87);	/* 27 */
    GG(b, c, d, a, x[8], S24, 0x455a14ed);	/* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);		/* 29 */
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8);	/* 30 */
    GG(c, d, a, b, x[7], S23, 0x676f02d9);	/* 31 */
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);		/* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[5], S31, 0xfffa3942);	/* 33 */
    HH(d, a, b, c, x[8], S32, 0x8771f681);	/* 34 */
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);		/* 35 */
    HH(b, c, d, a, x[14], S34, 0xfde5380c);		/* 36 */
    HH(a, b, c, d, x[1], S31, 0xa4beea44);	/* 37 */
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9);	/* 38 */
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60);	/* 39 */
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);		/* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);		/* 41 */
    HH(d, a, b, c, x[0], S32, 0xeaa127fa);	/* 42 */
    HH(c, d, a, b, x[3], S33, 0xd4ef3085);	/* 43 */
    HH(b, c, d, a, x[6], S34, 0x4881d05);	/* 44 */
    HH(a, b, c, d, x[9], S31, 0xd9d4d039);	/* 45 */
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);		/* 46 */
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);		/* 47 */
    HH(b, c, d, a, x[2], S34, 0xc4ac5665);	/* 48 */

    /* Round 4 */
    II(a, b, c, d, x[0], S41, 0xf4292244);	/* 49 */
    II(d, a, b, c, x[7], S42, 0x432aff97);	/* 50 */
    II(c, d, a, b, x[14], S43, 0xab9423a7);		/* 51 */
    II(b, c, d, a, x[5], S44, 0xfc93a039);	/* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3);		/* 53 */
    II(d, a, b, c, x[3], S42, 0x8f0ccc92);	/* 54 */
    II(c, d, a, b, x[10], S43, 0xffeff47d);		/* 55 */
    II(b, c, d, a, x[1], S44, 0x85845dd1);	/* 56 */
    II(a, b, c, d, x[8], S41, 0x6fa87e4f);	/* 57 */
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);		/* 58 */
    II(c, d, a, b, x[6], S43, 0xa3014314);	/* 59 */
    II(b, c, d, a, x[13], S44, 0x4e0811a1);		/* 60 */
    II(a, b, c, d, x[4], S41, 0xf7537e82);	/* 61 */
    II(d, a, b, c, x[11], S42, 0xbd3af235);		/* 62 */
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb);	/* 63 */
    II(b, c, d, a, x[9], S44, 0xeb86d391);	/* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    memset((unsigned char*) x, 0, sizeof(x));
}

static void Encode(unsigned char *output, uint32 *input, unsigned int len)
{
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (unsigned char) (input[i] & 0xff);
        output[j + 1] = (unsigned char) ((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char) ((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char) ((input[i] >> 24) & 0xff);
    }
}

static void Decode(uint32 *output, const unsigned char *input, unsigned int len)
{
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((uint32) input[j]) | (((uint32) input[j + 1]) << 8) |
                    (((uint32) input[j + 2]) << 16) | (((uint32) input[j + 3]) << 24);
}

/* instr 输入字符串，
 * outstr 输出加密后的字符串
 */

std::string MD5Hash(char *instr)
{
    unsigned char MD5[16];
    int bufferlen=strlen(instr);
	std::string outstr;
	outstr.resize(128);
    unsigned char *buffer=(unsigned char*)malloc(bufferlen*sizeof(unsigned char));
    int i=0;

    for (i=0; i<bufferlen; i++)
    {
        buffer[i]=(unsigned char)instr[i];
    }

    MD5_CTX context;
    if (bufferlen < 1)
    {
        outstr[0]='\0';
        return "";
    }
    MD5Init(&context);
    MD5Update(&context, buffer, bufferlen);
    MD5Final(MD5, &context);

    for(i=0; i<16; i++)
    {
        unsigned char ch=MD5[i];
        char H_ch,L_ch;

        H_ch=ch >> 4;
        L_ch=ch & 0x0f;

        if(H_ch>0x09)
            H_ch=H_ch+0x57;
        else
            H_ch=H_ch+0x30;

        if(L_ch>0x09)
            L_ch=L_ch+0x57;
        else
            L_ch=L_ch+0x30;

        outstr[i*2]=H_ch;
        outstr[i*2+1]=L_ch;
    }
    outstr[32]='\0';

    free(buffer);
    buffer=NULL;
    return outstr;
}

