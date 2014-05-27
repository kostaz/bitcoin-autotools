#include <assert.h>
#include "chainparams.h"
#include "util.h"

class CMainParams : public CChainParams
{
public:
	CMainParams()
	{
		pchMessageStart.bytes[0] = 0xf9;
		pchMessageStart.bytes[1] = 0xbe;
		pchMessageStart.bytes[2] = 0xb4;
		pchMessageStart.bytes[3] = 0xd9;
	}

	Network NetworkID() const
	{
		cout << __func__ << ": " << __LINE__ << endl;
		while (1) ;
		return CChainParams::MAIN;
	}
};

static CMainParams mainParams;

class CTestNetParams : public CChainParams
{
public:
	CTestNetParams()
	{
		pchMessageStart.bytes[0] = 0x0b;
		pchMessageStart.bytes[1] = 0x11;
		pchMessageStart.bytes[2] = 0x09;
		pchMessageStart.bytes[3] = 0x07;
	}

	virtual Network NetworkID() const
	{
		return CChainParams::TESTNET;
	}
};

static CTestNetParams testNetParams;

class CRegTestParams : public CChainParams
{
public:
	CRegTestParams()
	{
		pchMessageStart.bytes[0] = 0xfa;
		pchMessageStart.bytes[1] = 0xbf;
		pchMessageStart.bytes[2] = 0xb5;
		pchMessageStart.bytes[3] = 0xda;
	}

	virtual Network NetworkID() const
	{
		return CChainParams::TESTNET;
	}
};

static CRegTestParams regTestParams;

CChainParams *pCurrentParams = &mainParams;

const CChainParams& Params()
{
	return mainParams;
	// return *pCurrentParams;
}

void SelectParams(CChainParams::Network network)
{
	switch (network)
	{
	case CChainParams::MAIN:
		pCurrentParams = &mainParams;
		break;
	case CChainParams::TESTNET:
		pCurrentParams = &testNetParams;
		break;
	case CChainParams::REGTEST:
		pCurrentParams = &regTestParams;
		break;
	defaul:
		assert(false && "Unimplemented network!");
	}
}

bool SelectParamsFromCommandLine()
{
	bool regTest = GetBoolArg("-regtest", false);
	bool testNet = GetBoolArg("-testnet", false);

	if (regTest && testNet)
	{
		return false;
	}

	if (regTest)
	{
		SelectParams(CChainParams::REGTEST);
	}
	else if (testNet)
	{
		SelectParams(CChainParams::TESTNET);
	}
	else
	{
		SelectParams(CChainParams::MAIN);
	}

	return true;
}

