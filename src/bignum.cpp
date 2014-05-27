#include <iostream>
#include <openssl/bn.h>
#include "bignum.h"

using namespace std;

CBigNum::CBigNum()
{
	BN_init(this);
}

CBigNum::CBigNum(const CBigNum& b)
{
	BN_init(this);

	if (!BN_copy(this, &b)) {
		BN_clear_free(this);
		cout << __func__ << ": BN_copy failed." << endl;
		throw bignum_error("BN_copy failed.");
	}
}

CBigNum& CBigNum::operator=(const CBigNum& b)
{
	if (!BN_copy(this, &b)) {
		BN_clear_free(this);
		throw bignum_error("CBigNum::operator= : BN_copy failed");
	}

	return *this;
}

CBigNum::~CBigNum()
{
	BN_clear_free(this);
}

CBigNum::CBigNum(signed char n)
{
	BN_init(this);

	if (n >= 0)
		setulong(n);
	else
		setint64(n);
}

CBigNum::CBigNum(short n)
{
	BN_init(this);
	
	if (n >= 0)
		setulong(n);
	else
		setint64(n);
}

CBigNum::CBigNum(int n)
{
	BN_init(this);
	
	if (n >= 0)
		setulong(n);
	else
		setint64(n);
}

CBigNum::CBigNum(long n)
{
	BN_init(this);
	
	if (n >= 0)
		setulong(n);
	else
		setint64(n);
}

CBigNum::CBigNum(long long n)
{
	BN_init(this);
	setint64(n);
}

CBigNum::CBigNum(unsigned char n)
{
	BN_init(this);
	setulong(n);
}

CBigNum::CBigNum(unsigned short n)
{
	BN_init(this);
	setulong(n);
}

CBigNum::CBigNum(unsigned int n)
{
	BN_init(this);
	setulong(n);
}

CBigNum::CBigNum(unsigned long n)
{
	BN_init(this);
	setulong(n);
}

CBigNum::CBigNum(unsigned long long n)
{
	BN_init(this);
	setuint64(n);
}

CBigNum::CBigNum(uint256 n)
{
	BN_init(this);
	setuint256(n);
}

CBigNum::CBigNum(const std::vector<unsigned char>& vch)
{
	BN_init(this);
	setvch(vch);
}

void CBigNum::setulong(unsigned long n)
{
	if (!BN_set_word(this, n))
		throw bignum_error("BN_set_word failed");
}

unsigned long CBigNum::getulong() const
{
	return BN_get_word(this);
}

unsigned int CBigNum::getuint() const
{
	return BN_get_word(this);
}

int CBigNum::getint() const
{
	unsigned long n = BN_get_word(this);
	if (!BN_is_negative(this))
		return (n > (unsigned long)std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : n);
	else
		return (n > (unsigned long)std::numeric_limits<int>::max() ? std::numeric_limits<int>::min() : -(int)n);
}

void CBigNum::setint64(int64_t sn)
{
	unsigned char pch[sizeof(sn) + 6];
	unsigned char* p = pch + 4;
	bool fNegative;
	uint64_t n;

	if (sn < (int64_t)0)
	{
		// Since the minimum signed integer cannot be represented as positive so long as its type is signed, 
		// and it's not well-defined what happens if you make it unsigned before negating it,
		// we instead increment the negative integer by 1, convert it, then increment the (now positive) unsigned integer by 1 to compensate
		n = -(sn + 1);
		++n;
		fNegative = true;
	} else {
		n = sn;
		fNegative = false;
	}

	bool fLeadingZeroes = true;
	for (int i = 0; i < 8; i++)
	{
		unsigned char c = (n >> 56) & 0xff;
		n <<= 8;
		if (fLeadingZeroes)
		{
			if (c == 0)
				continue;
			if (c & 0x80)
				*p++ = (fNegative ? 0x80 : 0);
			else if (fNegative)
				c |= 0x80;
			fLeadingZeroes = false;
		}
		*p++ = c;
	}
	unsigned int nSize = p - (pch + 4);
	pch[0] = (nSize >> 24) & 0xff;
	pch[1] = (nSize >> 16) & 0xff;
	pch[2] = (nSize >> 8) & 0xff;
	pch[3] = (nSize) & 0xff;
	BN_mpi2bn(pch, p - pch, this);
}

void CBigNum::setuint64(uint64_t n)
{
	unsigned char pch[sizeof(n) + 6];
	unsigned char* p = pch + 4;
	bool fLeadingZeroes = true;
	for (int i = 0; i < 8; i++)
	{
		unsigned char c = (n >> 56) & 0xff;
		n <<= 8;
		if (fLeadingZeroes)
		{
			if (c == 0)
				continue;
			if (c & 0x80)
				*p++ = 0;
			fLeadingZeroes = false;
		}
		*p++ = c;
	}
	unsigned int nSize = p - (pch + 4);
	pch[0] = (nSize >> 24) & 0xff;
	pch[1] = (nSize >> 16) & 0xff;
	pch[2] = (nSize >> 8) & 0xff;
	pch[3] = (nSize) & 0xff;
	BN_mpi2bn(pch, p - pch, this);
}

void CBigNum::setuint256(uint256 n)
{
	unsigned char pch[sizeof(n) + 6];
	unsigned char* p = pch + 4;
	bool fLeadingZeroes = true;
	unsigned char* pbegin = (unsigned char*)&n;
	unsigned char* psrc = pbegin + sizeof(n);
	while (psrc != pbegin)
	{
		unsigned char c = *(--psrc);
		if (fLeadingZeroes)
		{
			if (c == 0)
				continue;
			if (c & 0x80)
				*p++ = 0;
			fLeadingZeroes = false;
		}
		*p++ = c;
	}
	unsigned int nSize = p - (pch + 4);
	pch[0] = (nSize >> 24) & 0xff;
	pch[1] = (nSize >> 16) & 0xff;
	pch[2] = (nSize >> 8) & 0xff;
	pch[3] = (nSize >> 0) & 0xff;
	BN_mpi2bn(pch, p - pch, this);
}

uint256 CBigNum::getuint256() const
{
	unsigned int nSize = BN_bn2mpi(this, NULL);
	if (nSize < 4)
		return 0;
	std::vector<unsigned char> vch(nSize);
	BN_bn2mpi(this, &vch[0]);
	if (vch.size() > 4)
		vch[4] &= 0x7f;
	uint256 n = 0;
	for (unsigned int i = 0, j = vch.size()-1; i < sizeof(n) && j >= 4; i++, j--)
		((unsigned char*)&n)[i] = vch[j];
	return n;
}

void CBigNum::setvch(const std::vector<unsigned char>& vch)
{
	std::vector<unsigned char> vch2(vch.size() + 4);
	unsigned int nSize = vch.size();
	// BIGNUM's byte stream format expects 4 bytes of
	// big endian size data info at the front
	vch2[0] = (nSize >> 24) & 0xff;
	vch2[1] = (nSize >> 16) & 0xff;
	vch2[2] = (nSize >> 8) & 0xff;
	vch2[3] = (nSize >> 0) & 0xff;
	// swap data to big endian
	reverse_copy(vch.begin(), vch.end(), vch2.begin() + 4);
	BN_mpi2bn(&vch2[0], vch2.size(), this);
}

std::vector<unsigned char> CBigNum::getvch() const
{
	unsigned int nSize = BN_bn2mpi(this, NULL);
	if (nSize <= 4)
		return std::vector<unsigned char>();
	std::vector<unsigned char> vch(nSize);
	BN_bn2mpi(this, &vch[0]);
	vch.erase(vch.begin(), vch.begin() + 4);
	reverse(vch.begin(), vch.end());
	return vch;
}

// The "compact" format is a representation of a whole
// number N using an unsigned 32bit number similar to a
// floating point format.
// The most significant 8 bits are the unsigned exponent of base 256.
// This exponent can be thought of as "number of bytes of N".
// The lower 23 bits are the mantissa.
// Bit number 24 (0x800000) represents the sign of N.
// N = (-1^sign) * mantissa * 256^(exponent-3)
//
// Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
// MPI uses the most significant bit of the first byte as sign.
// Thus 0x1234560000 is compact (0x05123456)
// and  0xc0de000000 is compact (0x0600c0de)
// (0x05c0de00) would be -0x40de000000
//
// Bitcoin only uses this "compact" format for encoding difficulty
// targets, which are unsigned 256bit quantities.  Thus, all the
// complexities of the sign bit and using base 256 are probably an
// implementation accident.
//
// This implementation directly uses shifts instead of going
// through an intermediate MPI representation.
CBigNum& CBigNum::SetCompact(unsigned int nCompact)
{
	unsigned int nSize = nCompact >> 24;
	bool fNegative     =(nCompact & 0x00800000) != 0;
	unsigned int nWord = nCompact & 0x007fffff;
	if (nSize <= 3)
	{
		nWord >>= 8*(3-nSize);
		BN_set_word(this, nWord);
	}
	else
	{
		BN_set_word(this, nWord);
		BN_lshift(this, this, 8*(nSize-3));
	}
	BN_set_negative(this, fNegative);
	return *this;
}

unsigned int CBigNum::GetCompact() const
{
	unsigned int nSize = BN_num_bytes(this);
	unsigned int nCompact = 0;
	if (nSize <= 3)
		nCompact = BN_get_word(this) << 8*(3-nSize);
	else
	{
		CBigNum bn;
		BN_rshift(&bn, this, 8*(nSize-3));
		nCompact = BN_get_word(&bn);
	}
	// The 0x00800000 bit denotes the sign.
	// Thus, if it is already set, divide the mantissa by 256 and increase the exponent.
	if (nCompact & 0x00800000)
	{
		nCompact >>= 8;
		nSize++;
	}
	nCompact |= nSize << 24;
	nCompact |= (BN_is_negative(this) ? 0x00800000 : 0);
	return nCompact;
}

void CBigNum::SetHex(const std::string& str)
{
	// skip 0x
	const char* psz = str.c_str();
	while (isspace(*psz))
		psz++;
	bool fNegative = false;
	if (*psz == '-')
	{
		fNegative = true;
		psz++;
	}
	if (psz[0] == '0' && tolower(psz[1]) == 'x')
		psz += 2;
	while (isspace(*psz))
		psz++;

	// hex string to bignum
	*this = 0;
	int n;
	while ((n = HexDigit(*psz)) != -1)
	{
		*this <<= 4;
		*this += n;
		++psz;
	}
	if (fNegative)
		*this = 0 - *this;
}

std::string CBigNum::ToString(int nBase) const
{
	CAutoBN_CTX pctx;
	CBigNum bnBase = nBase;
	CBigNum bn0 = 0;
	std::string str;
	CBigNum bn = *this;
	BN_set_negative(&bn, false);
	CBigNum dv;
	CBigNum rem;
	if (BN_cmp(&bn, &bn0) == 0)
		return "0";
	while (BN_cmp(&bn, &bn0) > 0)
	{
		if (!BN_div(&dv, &rem, &bn, &bnBase, pctx))
			throw bignum_error("CBigNum::ToString() : BN_div failed");
		bn = dv;
		unsigned int c = rem.getulong();
		str += "0123456789abcdef"[c];
	}
	if (BN_is_negative(this))
		str += "-";
	reverse(str.begin(), str.end());
	return str;
}

std::string CBigNum::GetHex() const
{
	return ToString(16);
}

unsigned int CBigNum::GetSerializeSize(int nType, int nVersion) const
{
	return ::GetSerializeSize(getvch(), nType, nVersion);
}

template<typename Stream>
void CBigNum::Serialize(Stream& s, int nType, int nVersion) const
{
	::Serialize(s, getvch(), nType, nVersion);
}

	template<typename Stream>
void CBigNum::Unserialize(Stream& s, int nType, int nVersion)
{
	std::vector<unsigned char> vch;
	::Unserialize(s, vch, nType, nVersion);
	setvch(vch);
}


bool CBigNum::operator!() const
{
	return BN_is_zero(this);
}

CBigNum& CBigNum::operator+=(const CBigNum& b)
{
	if (!BN_add(this, this, &b))
		throw bignum_error("CBigNum::operator+= : BN_add failed");
	return *this;
}

CBigNum& CBigNum::operator-=(const CBigNum& b)
{
	*this = *this - b;
	return *this;
}

CBigNum& CBigNum::operator*=(const CBigNum& b)
{
	CAutoBN_CTX pctx;
	if (!BN_mul(this, this, &b, pctx))
		throw bignum_error("CBigNum::operator*= : BN_mul failed");
	return *this;
}

CBigNum& CBigNum::operator/=(const CBigNum& b)
{
	*this = *this / b;
	return *this;
}

CBigNum& CBigNum::operator%=(const CBigNum& b)
{
	*this = *this % b;
	return *this;
}

CBigNum& CBigNum::operator<<=(unsigned int shift)
{
	if (!BN_lshift(this, this, shift))
		throw bignum_error("CBigNum:operator<<= : BN_lshift failed");
	return *this;
}

CBigNum& CBigNum::operator>>=(unsigned int shift)
{
	// Note: BN_rshift segfaults on 64-bit if 2^shift is greater than the number
	//   if built on ubuntu 9.04 or 9.10, probably depends on version of OpenSSL
	CBigNum a = 1;
	a <<= shift;
	if (BN_cmp(&a, this) > 0)
	{
		*this = 0;
		return *this;
	}

	if (!BN_rshift(this, this, shift))
		throw bignum_error("CBigNum:operator>>= : BN_rshift failed");
	return *this;
}

CBigNum& CBigNum::operator++()
{
	// prefix operator
	if (!BN_add(this, this, BN_value_one()))
		throw bignum_error("CBigNum::operator++ : BN_add failed");
	return *this;
}

const CBigNum CBigNum::operator++(int)
{
	// postfix operator
	const CBigNum ret = *this;
	++(*this);
	return ret;
}

CBigNum& CBigNum::operator--()
{
	// prefix operator
	CBigNum r;
	if (!BN_sub(&r, this, BN_value_one()))
		throw bignum_error("CBigNum::operator-- : BN_sub failed");
	*this = r;
	return *this;
}

const CBigNum CBigNum::operator--(int)
{
	// postfix operator
	const CBigNum ret = *this;
	--(*this);
	return ret;
}

