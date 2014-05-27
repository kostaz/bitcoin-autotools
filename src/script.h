#ifndef BITCOIN_SCRIPT_H
#define BITCOIN_SCRIPT_H

#include <vector>

#include "bignum.h"
#include "util.h"

using namespace std;

enum opcodetype
{
	OP_0 		= 0x00,
	OP_FALSE	= OP_0,
	OP_PUSHDATA1	= 0x4c,
	OP_PUSHDATA2	= 0x4d,
	OP_PUSHDATA4	= 0x4e,
	OP_1NEGATE	= 0x4f,
	OP_RESERVED	= 0x50,
	OP_1		= 0x51,

};

class CScript : public vector<unsigned char>
{
protected:
	CScript& push_int64(int64_t n);

	CScript& operator<<(const vector<unsigned char>& b)
	{
		if (b.size() < OP_PUSHDATA1)
		{
		}
	}
};

#endif // BITCOIN_SCRIPT_H

