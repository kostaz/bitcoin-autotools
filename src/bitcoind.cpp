#include <stdio.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "noui.h"
#include "init.h"
#include "ui_interface.h"
#include "util.h"
#include "chainparams.h"

void DetectShutdownThread(boost::thread_group* threadGroup)
{
	bool fShutdown = ShutdownRequested();

	while (!fShutdown)
	{
		MilliSleep(200);
		fShutdown = ShutdownRequested();
	}

	if (threadGroup)
	{
		threadGroup->interrupt_all();
		threadGroup->join_all();
	}
}

bool AppInit(int argc, char* argv[])
{
	boost::thread_group threadGroup;
	boost::thread *detectShutdownThread = 0;
	bool fRet = false;

	ParseParameters(argc, argv);

	namespace fs = boost::filesystem;

	fs::path dataDir = GetDataDir();

	cout << __func__ << ": " << __LINE__ << endl;

	if (!fs::is_directory(dataDir))
	{
	cout << __func__ << ": " << __LINE__ << endl;
		fprintf(stderr, "%s: Error: Specified data directory "
			"\"%s\" does not exists.\n", __func__, mapArgs["-datadir"].c_str());
		return false;
	}

	cout << __func__ << ": " << __LINE__ << endl;
	try
	{
	cout << __func__ << ": " << __LINE__ << endl;
		ReadConfigFile(mapArgs, mapMultiArgs);
	}
	catch (std::exception& e)
	{
	cout << __func__ << ": " << __LINE__ << endl;
		fprintf(stderr, "%s: Error: Error reading config file %s.\n",
			__func__, e.what());
		return false;
	}

	cout << __func__ << ": " << __LINE__ << endl;
	if (!SelectParamsFromCommandLine())
	{
	cout << __func__ << ": " << __LINE__ << endl;
		fprintf(stderr, "%s: Invalid combination of -regtest and -tested.\n", __func__);
		return false;
	}

	cout << __func__ << ": " << __LINE__ << endl;
	if (mapArgs.count("-?") || mapArgs.count("--help"))
	{
		fprintf(stdout, "Printing help message...\n");
		return false;
	}

	cout << __func__ << ": " << __LINE__ << endl;
	SoftSetBoolArg("-server", true);

	cout << __func__ << ": " << __LINE__ << endl;
	detectShutdownThread = new boost::thread(
		boost::bind(&DetectShutdownThread, &threadGroup));

	cout << __func__ << ": " << __LINE__ << endl;
	fRet = AppInit2(threadGroup);
	cout << __func__ << ": " << __LINE__ << endl;

	if (!fRet)
	{
	cout << __func__ << ": " << __LINE__ << endl;
		if (detectShutdownThread)
		{
	cout << __func__ << ": " << __LINE__ << endl;
			detectShutdownThread->interrupt();
		}

	cout << __func__ << ": " << __LINE__ << endl;
		threadGroup.interrupt_all();
	}
	cout << __func__ << ": " << __LINE__ << endl;

	if (detectShutdownThread)
	{
	cout << __func__ << ": " << __LINE__ << endl;
		detectShutdownThread->join();
		delete detectShutdownThread;
		detectShutdownThread = NULL;
	}
	cout << __func__ << ": " << __LINE__ << endl;

	Shutdown();
	cout << __func__ << ": " << __LINE__ << endl;

	return fRet;
}

int main(int argc, char* argv[])
{
	noui_connect();

	if (!AppInit(argc, argv))
	{
		fprintf(stderr, "%s: AppInit() failed.\n", __func__);
		return 1;
	}

	return 0;
}

