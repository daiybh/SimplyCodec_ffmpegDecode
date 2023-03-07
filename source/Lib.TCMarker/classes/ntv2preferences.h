#ifndef NTV2PREFERNCES
#define NTV2PREFERNCES

#if	MSWindows
	#include <Windows.h>
	#define	REG_PATH "SOFTWARE\\aja\\Preferences\\"
#endif

#if	AJAMac
	#include <Carbon/Carbon.h>
#endif

#define PREF_STRING_SIZE  256

//------------------------------------------------------AJALinux---------------------------
//	Class CNTV2Preferences
//---------------------------------------------------------------------------------

class CNTV2Preferences
{
public:
					CNTV2Preferences (const char *prefID, bool user);
	virtual			~CNTV2Preferences();
	
	char *			CopyString (const char *key, const char *defaultStr);
	void			GetString(const char *key, const char *defaultStr, char *outStr, long size);
	void			SetString (const char *key, char *defaultStr);
	
	bool			GetBool (const char *key, bool defaultVal);
	void			SetBool (const char *key, bool defaultVal);
	
	int				GetInt (const char *key, int defaultVal);
	void			SetInt (const char *key, int defaultVal);
	
	double			GetDouble(const char *key, double defaultVal);
	void			SetDouble(const char *key, double defaultVal);
	
	void			Remove();
	
protected:
	const char *	LoadString(const char *key, const char *defaultStr);

protected:
	bool			_user;
	mutable char	_prefString[PREF_STRING_SIZE];

#if	MSWindows
	const HKEY		GetUser() const { return _user ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE; }
	char *			_prefID;
#endif

#if AJAMac
	const CFStringRef GetUser() const { return _user ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser; }
	CFStringRef		_prefID;
#endif
};


#endif
