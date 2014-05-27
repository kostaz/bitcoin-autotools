#ifndef BITCOIN_UTIL_H
#define BITCOIN_UTIL_H

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

using namespace std;

inline void MilliSleep(int64_t n)
{
	// boost::this_thread::sleep_for(boost::chrono::milliseconds(n));
	boost::this_thread::sleep(boost::posix_time::milliseconds(n));
}

#define LogPrintf(...)

extern std::map<string, string> mapArgs;
extern std::map<string, vector<string> > mapMultiArgs;

#define PAIRTYPE(t1, t2)	std::pair<t1, t2>

void ParseParameters(int argc, const char* const argv[]);

boost::filesystem::path GetDefaultDataDir();
const boost::filesystem::path& GetDataDir(bool fNetSpecific = true);
void ReadConfigFile(map<string, string>& mapSettingsReg,
		    map<string, vector<string> >& mapMultiSettingsReg);

inline bool IsSwitchChar(char c)
{
	return c == '-';
}

bool GetBoolArg(const string& argName, bool fDefault);
bool SoftSetArg(const string& arg, const string& val);
bool SoftSetBoolArg(const string& arg, bool set);
string GetArg(const string& arg, const string& argDefault);

#endif // BITCOIN_UTIL_H

