#include "script.h"

CScript& CScript::push_int64(int64_t n)
{
	if (n == -1 || (n >= 1 && n <= 16)) {
		push_back(n + (OP_1 - 1));
	} else {
		CBigNum bn(n);
		*this << bn.getvch();
	}
}

