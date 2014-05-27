#ifndef BITCOIN_CORE_H
#define BITCOIN_CORE_H

#include <stdint.h>

#include "uint256.h"

class CTransaction;

class COutPoint
{
public:
	uint256 hash;
	unsigned int n;

	COutPoint()
	{
		SetNull();
	}

	COutPoint(uint256 hashIn, unsigned int nIn)
	{
		hash = hashIn;
		n    = nIn;
	}

	void SetNull()
	{
		hash = 0;
		n = (unsigned int) -1;
	}

	bool IsNull()
	{
		if (hash == 0 && n == (unsigned int)-1)
		{
			return true;
		}

		return false;
	}

	friend bool operator<(const COutPoint& a, const COutPoint& b)
	{
		if (a.hash < b.hash || (a.hash == b.hash && a.n < b.n))
		{
			return true;
		}

		return false;
	}

	friend bool operator==(const COutPoint& a, const COutPoint& b)
	{
		if (a.hash == b.hash && a.n == b.n)
		{
			return true;
		}

		return false;
	}

	friend bool operator!=(const COutPoint& a, const COutPoint& b)
	{
		return !(a == b);
	}

};

class CInPoint
{
public:
	const CTransaction *ptx;
	unsigned int n;

	CInPoint()
	{
		SetNull();
	}

	CInPoint(const CTransaction* ptxIn, unsigned int nIn)
	{
		ptx = ptxIn;
		n   = nIn;
	}

	void SetNull()
	{
		ptx = NULL;
		n = (unsigned int) -1;
	}

	bool IsNull()
	{
		if (ptx == NULL && n == 0)
		{
			return true;
		}

		return false;
	}
};

class CTxIn
{
public:
	COutPoint prevout;
};

class CTxOut
{

};

class CTransaction
{
public:
	static int64_t nMinTxFee;
	static int64_t nMinRelayTxFee;
	static const int CURRENT_VERSION = 1;
	int nVersion;
};

#endif // BITCOIN_CORE_H

