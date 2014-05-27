#ifndef BITCOIN_BIGNUM_H
#define BITCOIN_BIGNUM_H

#include <stdexcept>
#include <stdint.h>
#include <vector>

#include <openssl/bn.h>

#include "serialize.h"
#include "uint256.h"
#include "version.h"

/**
 * Errors thrown by the bignum class
 */
class bignum_error : public std::runtime_error
{
public:
	explicit bignum_error(const std::string& str)
		: std::runtime_error(str)
	{
		// nothing
	}
};

class CAutoBN_CTX
{
protected:
    BN_CTX* pctx;
    BN_CTX* operator=(BN_CTX* pnew) { return pctx = pnew; }

public:
    CAutoBN_CTX()
    {
        pctx = BN_CTX_new();
        if (pctx == NULL)
            throw bignum_error("CAutoBN_CTX : BN_CTX_new() returned NULL");
    }

    ~CAutoBN_CTX()
    {
        if (pctx != NULL)
            BN_CTX_free(pctx);
    }

    operator BN_CTX*()
    {
	    return pctx;
    }

    BN_CTX& operator*()
    {
	    return *pctx;
    }

    BN_CTX** operator&()
    {
	    return &pctx;
    }

    bool operator!()
    {
	    return pctx == NULL;
    }
};

class CBigNum : public BIGNUM
{
public:
	CBigNum();
	CBigNum(const CBigNum& b);
	CBigNum(signed char n);
	CBigNum(short n);
	CBigNum(int n);
	CBigNum(long n);
	CBigNum(long long n);
	CBigNum(unsigned char n);
	CBigNum(unsigned short n);
	CBigNum(unsigned int n);
	CBigNum(unsigned long n);
	CBigNum(unsigned long long n);
	explicit CBigNum(uint256 n);
	explicit CBigNum(const std::vector<unsigned char>& vch);
	// CBigNum(char n) is not portable.
	// Use 'signed char' or 'unsigned char'.

	~CBigNum();

	void setulong(unsigned long n);
	unsigned long getulong() const;
	unsigned int getuint() const;
	int getint() const;
	void setint64(int64_t sn);
	void setuint64(uint64_t n);
	void setuint256(uint256 n);
	uint256 getuint256() const;
	void setvch(const std::vector<unsigned char>& vch);
	std::vector<unsigned char> getvch() const;

	CBigNum& SetCompact(unsigned int nCompact);
	unsigned int GetCompact() const;

	void SetHex(const std::string& str);
	std::string GetHex() const;

	std::string ToString(int nBase=10) const;

	unsigned int GetSerializeSize(int nType=0, int nVersion=PROTOCOL_VERSION) const;

	template<typename Stream>
	void Serialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION) const;

	template<typename Stream>
	void Unserialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION);

	CBigNum& operator=(const CBigNum& b);
	bool operator!() const;
	CBigNum& operator+=(const CBigNum& b);
	CBigNum& operator-=(const CBigNum& b);
	CBigNum& operator*=(const CBigNum& b);
	CBigNum& operator/=(const CBigNum& b);
	CBigNum& operator%=(const CBigNum& b);
	CBigNum& operator<<=(unsigned int shift);
	CBigNum& operator>>=(unsigned int shift);
	CBigNum& operator++();
	const CBigNum operator++(int);
	CBigNum& operator--();
	const CBigNum operator--(int);

	friend inline const CBigNum operator-(const CBigNum& a, const CBigNum& b);
	friend inline const CBigNum operator/(const CBigNum& a, const CBigNum& b);
	friend inline const CBigNum operator%(const CBigNum& a, const CBigNum& b);
};

inline const CBigNum operator+(const CBigNum& a, const CBigNum& b)
{
	CBigNum r;

	if (!BN_add(&r, &a, &b))
		throw bignum_error("CBigNum::operator+ : BN_add failed");

	return r;
}

inline const CBigNum operator-(const CBigNum& a, const CBigNum& b)
{
	CBigNum r;

	if (!BN_sub(&r, &a, &b))
		throw bignum_error("CBigNum::operator- : BN_sub failed");

	return r;
}

inline const CBigNum operator-(const CBigNum& a)
{
	CBigNum r(a);

	BN_set_negative(&r, !BN_is_negative(&r));

	return r;
}

inline const CBigNum operator*(const CBigNum& a, const CBigNum& b)
{
	CAutoBN_CTX pctx;
	CBigNum r;

	if (!BN_mul(&r, &a, &b, pctx))
		throw bignum_error("CBigNum::operator* : BN_mul failed");

	return r;
}

inline const CBigNum operator/(const CBigNum& a, const CBigNum& b)
{
	CAutoBN_CTX pctx;
	CBigNum r;

	if (!BN_div(&r, NULL, &a, &b, pctx))
		throw bignum_error("CBigNum::operator/ : BN_div failed");

	return r;
}

inline const CBigNum operator%(const CBigNum& a, const CBigNum& b)
{
	CAutoBN_CTX pctx;
	CBigNum r;

	if (!BN_mod(&r, &a, &b, pctx))
		throw bignum_error("CBigNum::operator% : BN_div failed");

	return r;
}

inline const CBigNum operator<<(const CBigNum& a, unsigned int shift)
{
	CBigNum r;

	if (!BN_lshift(&r, &a, shift))
		throw bignum_error("CBigNum:operator<< : BN_lshift failed");

	return r;
}

inline const CBigNum operator>>(const CBigNum& a, unsigned int shift)
{
	CBigNum r = a;
	r >>= shift;
	return r;
}

inline bool operator==(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) == 0); }
inline bool operator!=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) != 0); }
inline bool operator<=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) <= 0); }
inline bool operator>=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) >= 0); }
inline bool operator<(const CBigNum& a, const CBigNum& b)  { return (BN_cmp(&a, &b) < 0); }
inline bool operator>(const CBigNum& a, const CBigNum& b)  { return (BN_cmp(&a, &b) > 0); }

#endif // BITCOIN_BIGNUM_H
