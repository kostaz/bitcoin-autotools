#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <boost/thread.hpp>

#include "init.h"
#include "util.h"

volatile bool fRequestShutdown = false;

void StartShutdown()
{
    fRequestShutdown = true;
}

bool ShutdownRequested()
{
    return fRequestShutdown;
}

void Shutdown()
{
	// Temporary empty.
}

void HandleSIGTERM(int)
{
	cout << __func__ << ": " << __LINE__ << endl;
	fRequestShutdown = true;
}

void HandleSIGHUP(int)
{
	cout << __func__ << ": " << __LINE__ << endl;
	// fReopenDebugLog = true;
}

void InitSigHandlers()
{
	struct sigaction sa;
	sa.sa_handler = HandleSIGTERM;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	// Reopen debug.log on SIGHUP
	struct sigaction sa_hup;
	sa_hup.sa_handler = HandleSIGHUP;
	sigemptyset(&sa_hup.sa_mask);
	sa_hup.sa_flags = 0;
	sigaction(SIGHUP, &sa_hup, NULL);
}

void InitParams()
{
	// When specifying an explicit binding address, you want to listen on it
	// even when -connect or -proxy is specified
	if (mapArgs.count("-bind")) {
		if (SoftSetBoolArg("-listen", true))
		{
			LogPrintf("-bind set -> setting -listen=1\n");
		}
	}

	// when only connecting to trusted nodes, do not seed via DNS, or listen by default
	if (mapArgs.count("-connect") && mapMultiArgs["-connect"].size() > 0)
	{
		if (SoftSetBoolArg("-dnsseed", false))
		{
			LogPrintf("-connect set -> setting -dnsseed=0\n");
		}

		if (SoftSetBoolArg("-listen", false))
		{
			LogPrintf("-connect set -> setting -listen=0\n");
		}
	}

	// To protect privacy, do not listen by default if a default proxy server is specified
	if (mapArgs.count("-proxy"))
	{
		if (SoftSetBoolArg("-listen", false))
		{
			LogPrintf("-proxy set -> setting -listen=0\n");
		}
	}

	// do not map ports or try to retrieve public IP when not listening (pointless)
	if (!GetBoolArg("-listen", true))
	{
		if (SoftSetBoolArg("-upnp", false))
		{
			LogPrintf("-listen=0 -> setting -upnp=0\n");
		}

		if (SoftSetBoolArg("-discover", false))
		{
			LogPrintf("-listen=0 -> setting -discover=0\n");
		}
	}

	// if an explicit public IP is specified, do not try to find others
	if (mapArgs.count("-externalip"))
	{
		if (SoftSetBoolArg("-discover", false))
		{
			LogPrintf("-externalip set -> setting -discover=0\n");
		}
	}

	// Rewrite just private keys: rescan to find transactions
	if (GetBoolArg("-salvagewallet", false))
	{
		if (SoftSetBoolArg("-rescan", true))
		{
			LogPrintf("-salvagewallet=1 -> setting -rescan=1\n");
		}
	}

	// -zapwallettx implies a rescan
	if (GetBoolArg("-zapwallettxes", false))
	{
		if (SoftSetBoolArg("-rescan", true))
		{
			LogPrintf("-zapwallettxes=1 -> setting -rescan=1\n");
		}
	}
}

void VerityFileDescriptors()
{

}

void InitParamsInternalFlags()
{

}

void InitCoinBaseFlags()
{
	// Continue to put "/P2SH/" in the coinbase to monitor BIP16 support.
	// This can be removed eventually.
	const char* p2sh = "/P2SH/";
	// COINBASE_FLAGS << vector<unsigned char>(p2sh, p2sh + strlen(p2sh));
}

bool AppInit2(boost::thread_group& threadGroup)
{
	umask(077);

	InitSigHandlers();

	InitParams();

	VerityFileDescriptors();

	InitParamsInternalFlags();

	InitCoinBaseFlags();
	
	return true;
}

