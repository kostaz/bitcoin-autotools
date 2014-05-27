#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <stdarg.h>

#include "util.h"
#include "chainparams.h"

using namespace std;

map<string, string> mapArgs;
map<string, vector<string> > mapMultiArgs;

// Interpret -nofoo as -foo=0; -nofoo=0 as -foo=1 as long
// as -foo is not set.
static void InterpretNegativeSettings(string singleDashArgName, map<string, string>& map)
{
	string name = singleDashArgName;

	if (name.find("-no"))
		return;

	string positive("-");
	positive.append(name.begin() + 3, name.end());

	if (map.count(positive) != 0)
		return;

	bool val = !GetBoolArg(name, false);
	map[positive] = (val ? "1" : "0");
	map.erase(name);
}

bool GetBoolArg(const string& argName, bool fDefault)
{
	if (mapArgs.count(argName) == 0)
		return fDefault;

	string argVal = mapArgs[argName];
	if (argVal.empty())
		return true;

	int n = atoi(argVal.c_str());
	return n != 0;
}

bool SoftSetArg(const string& arg, const string& val)
{
	if (mapArgs.count(arg))
	{
		return false;
	}

	mapArgs[arg] = val;
	return true;
}

bool SoftSetBoolArg(const string& arg, bool set)
{
	if (set)
	{
		return SoftSetArg(arg, string("1"));
	}
	else
	{
		return SoftSetArg(arg, string("0"));
	}
}

namespace fs = boost::filesystem;

fs::path GetDefaultDataDir()
{
	fs::path dir;

	cout << __func__ << ": " << __LINE__ << ": ENTER" << endl;

	char* home = getenv("HOME");

	if (!home || strlen(home) == 0)
	{
		dir = fs::path("/");
	}
	else
	{
		dir = fs::path(home);
	}

	cout << __func__ << ": " << __LINE__ << ": EXIT" << endl;
	return dir / ".bitcoin";
}

static fs::path pathCached[CChainParams::MAX_NETWORK_TYPES + 1];

const fs::path& GetDataDir(bool fNetSpecific)
{
	boost::thread::id thread_id = boost::this_thread::get_id();

	int nNet = CChainParams::MAX_NETWORK_TYPES;

	if (fNetSpecific)
	{
		// nNet = Params().NetworkID(); // crashes the program ! --- kostaz
		nNet = 0;
	}

	fs::path &path = pathCached[nNet];

	if (!path.empty())
	{
		return path;
	}

	if (mapArgs.count("-datadir"))
	{
		path = fs::system_complete(mapArgs["-datadir"]);
		if (!fs::is_directory(path))
		{
			path = "";
		}
	}
	else
	{
		path = GetDefaultDataDir();
	}

	return path;
}

void ClearDataDirCache()
{
	fs::path* first = &pathCached[0];
	fs::path* last  = &pathCached[CChainParams::MAX_NETWORK_TYPES + 1];

	fill(first, last, fs::path());
}

fs::path GetConfigFile()
{
	string configFileStr = GetArg("-conf", "bitcoin.conf");
	fs::path configFile(configFileStr);

	if (!configFile.is_complete())
	{
		configFile = GetDataDir(false) / configFile;
	}

	return configFile;
}

void ReadConfigFile(map<string, string>& mapSettingsReg,
		    map<string, vector<string> >& mapMultiSettingsReg)
{
	fs::path file = GetConfigFile();

	fs::ifstream streamConfig(file);

	if (!streamConfig.good())
	{
		cout << "Config file " << file << " doesn't exist!" << endl;
		return;
	}

	set<string> setOptions;
	setOptions.insert("*");

	for (boost::program_options::detail::config_file_iterator
	     it(streamConfig, setOptions), end; it != end; ++it)
	{
		string key = string("-") + it->string_key;
		if (mapSettingsReg.count(key) == 0)
		{
			mapSettingsReg[key] = it->value[0];
			InterpretNegativeSettings(key, mapSettingsReg);
		}

		mapMultiSettingsReg[key].push_back(it->value[0]);
	}

	ClearDataDirCache();
}

void DumpMap(const map<string, string>& map)
{
	for (std::map<string, string>::const_iterator it = map.begin();
	     it != map.end(); ++it)
	{
		cout << __func__ << ": " << it->first << "=" << it->second << endl;
	}
}

void ParseParameters(int argc, const char* const argv[])
{
	mapArgs.clear();
	mapMultiArgs.clear();

	for (int i = 1; i < argc; i++)
	{
		string str(argv[i]);

		boost::regex regex("[-]{1,2}\\w+([=].+)?");
		if (!boost::regex_match(str, regex))
		{
			cout << "Wrong param: " << str << endl;
			continue;
		}

		string strValue;
		size_t is_index = str.find('=');
		if (is_index != string::npos)
		{
			strValue = str.substr(is_index + 1);
			str = str.substr(0, is_index);
		}

		if (str[0] != '-')
		{
			cout << "Wrong param!" << endl;
			break;
		}

		mapArgs[str] = strValue;
		mapMultiArgs[str].push_back(strValue);
	}

	map<string, string> tmpMapArgs(mapArgs);

	BOOST_FOREACH(const PAIRTYPE(string, string)& entry, tmpMapArgs)
	{
		string name = entry.first;

		// Interpret --foo as -foo as long as both are not set.
		if (name.find("--") == 0)
		{
			string singleDash(name.begin() + 1, name.end());
			if (mapArgs.count(singleDash) == 0)
			{
				mapArgs[singleDash] = entry.second;
				mapArgs.erase(name);
			}

			name = singleDash;
		}

		InterpretNegativeSettings(name, mapArgs);
	}
}

string GetArg(const string& arg, const string& argDefault)
{
	if (mapArgs.count(arg))
	{
		return mapArgs[arg];
	}

	return argDefault;
}

