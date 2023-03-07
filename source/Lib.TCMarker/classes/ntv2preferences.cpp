#include "ntv2preferences.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//---------------------------------------------------------------------------------
//	CNTV2Preferences
//---------------------------------------------------------------------------------

CNTV2Preferences::CNTV2Preferences (const char *prefID, bool user)
{
#if	MSWindows
	_prefID = (char *) malloc (strlen (REG_PATH) + strlen (prefID) + 1);
	strcpy (_prefID, REG_PATH);
	strcat (_prefID, prefID);

	for (int i = 0; _prefID[i] != '\0'; i++)
	{
		if (_prefID[i] != '.') 
			continue;
		_prefID[i] = '\\';
	}
#endif

#if	AJAMac
	_prefID = CFStringCreateWithCString (kCFAllocatorDefault, prefID, kCFStringEncodingASCII);
#endif

	_user = user;
}




//---------------------------------------------------------------------------------
//	~CNTV2Preferences
//---------------------------------------------------------------------------------

CNTV2Preferences::~CNTV2Preferences()
{
#if	MSWindows
	if (_prefID != NULL)
	{
		free (_prefID);
		_prefID = NULL;
		_prefString[0] = 0;
	}
#endif

#if	AJAMac
	if (_prefID != NULL)
	{
		CFPreferencesSynchronize (_prefID, GetUser(), kCFPreferencesCurrentHost);
		CFRelease (_prefID);
		_prefID = NULL;
		_prefString[0] = 0;
	}
#endif
}


//---------------------------------------------------------------------------------
//	CopyString
//---------------------------------------------------------------------------------
// allocates memory via malloc

char *CNTV2Preferences::CopyString (const char *key, const char *defaultStr)
{
	const char *loadStr = LoadString(key, defaultStr);
	
	char *result = (char *) malloc (strlen (loadStr) + 1);
	strcpy (result, loadStr);
	return result;
}

//---------------------------------------------------------------------------------
//	GetString
//---------------------------------------------------------------------------------

void CNTV2Preferences::GetString(const char *key, const char *defaultStr, char *outStr, long size)
{
	const char *loadStr = LoadString(key, defaultStr);
	
	long loadSize = (long)strlen (loadStr) + 1;
	if (loadSize <= size)
	{
		memmove (outStr, loadStr, loadSize);
	}
	else
	{
		memmove (outStr, loadStr, loadSize);
		outStr[size-1] = 0;
	}
}


//---------------------------------------------------------------------------------
//	LoadString
//---------------------------------------------------------------------------------

const char *CNTV2Preferences::LoadString (const char *key, const char *defaultStr)
{
	_prefString[0] = 0;

#if	MSWindows
	HKEY baseKey;

	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) == ERROR_SUCCESS)
	{
		unsigned long type = REG_SZ;
		unsigned long size = PREF_STRING_SIZE;

		if (RegQueryValueEx (baseKey, key, NULL, &type, (unsigned char *)_prefString, &size) == ERROR_SUCCESS)
		{
			if (type == REG_SZ) 
				defaultStr = _prefString;
		}

		RegCloseKey (baseKey);
	}
	
	return defaultStr;
		
#endif

#if	AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFStringRef stringRef = (CFStringRef) CFPreferencesCopyValue (keyRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);

	if (stringRef != NULL)
	{
		CFStringGetCString (stringRef, _prefString, PREF_STRING_SIZE, kCFStringEncodingASCII);
		defaultStr = _prefString;
	}
	
	return defaultStr;
#endif

#if AJALinux
	return defaultStr;
#endif

}


//---------------------------------------------------------------------------------
//	SetString
//---------------------------------------------------------------------------------

void CNTV2Preferences::SetString (const char *key, char *defaultStr)
{
#if	MSWindows
	HKEY baseKey;
	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return;

	RegSetValueEx (baseKey, key, 0, REG_SZ, (unsigned char *)defaultStr, (DWORD)strlen(defaultStr)+1);
	RegCloseKey (baseKey);
#endif

#if AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFStringRef stringRef = CFStringCreateWithCString (kCFAllocatorDefault, defaultStr, kCFStringEncodingASCII);
	CFPreferencesSetValue (keyRef, stringRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
 	
	CFRelease (stringRef);
	CFRelease (keyRef); 	
#endif

#if AJALinux
	// TBD - do nothing
#endif
}


//---------------------------------------------------------------------------------
//	GetBool
//---------------------------------------------------------------------------------

bool CNTV2Preferences::GetBool (const char *key, bool defaultVal)
{
#if	MSWindows
	HKEY baseKey;

	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return defaultVal;
	
	unsigned long type = REG_SZ;
	unsigned long size = PREF_STRING_SIZE;

	if (RegQueryValueEx (baseKey, key, NULL, &type, (unsigned char *)_prefString, &size) == ERROR_SUCCESS)
	{
		if (type == REG_SZ && size > 0)
		{
			defaultVal = stricmp (_prefString, "true") == 0 || stricmp (_prefString, "1") == 0;
		}
	}
	
	RegCloseKey (baseKey);
	return defaultVal;
#endif

#if	AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFBooleanRef boolRef = (CFBooleanRef) CFPreferencesCopyValue (keyRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);

	if (boolRef != NULL)
	{
		defaultVal = CFBooleanGetValue (boolRef);
		CFRelease (boolRef);
	}
	
   	return defaultVal;
#endif

#if AJALinux
	return defaultVal;
#endif
}

//---------------------------------------------------------------------------------
//	SetBool
//---------------------------------------------------------------------------------

void CNTV2Preferences::SetBool (const char *key, bool defaultVal)
{
#if	MSWindows
	HKEY baseKey;

	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return;
	
	strcpy (_prefString, defaultVal ? "true" : "false");
	RegSetValueEx (baseKey, key, 0, REG_SZ, (unsigned char *)_prefString, (DWORD)strlen(_prefString)+1);
	RegCloseKey (baseKey);
#endif

#if AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFPreferencesSetValue(	keyRef, defaultVal ? kCFBooleanTrue : kCFBooleanFalse, _prefID, GetUser(), kCFPreferencesCurrentHost );
	CFRelease (keyRef);
#endif

#if AJALinux
	// TBD - do nothing
#endif
}

//---------------------------------------------------------------------------------
//	GetInt
//---------------------------------------------------------------------------------

int CNTV2Preferences::GetInt (const char *key, int defaultVal)
{
#if	MSWindows
	HKEY baseKey;

	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return defaultVal;
	
	unsigned long type = REG_SZ;
	unsigned long size = PREF_STRING_SIZE;

	if (RegQueryValueEx (baseKey, key, NULL, &type, (unsigned char *)_prefString, &size) == ERROR_SUCCESS)
	{
		if (type == REG_SZ && size > 0) 
			defaultVal = atoi (_prefString);
	}
	RegCloseKey (baseKey);
	return defaultVal;
#endif

#if	AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFNumberRef numberRef = (CFNumberRef) CFPreferencesCopyValue (keyRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);

	if (numberRef != NULL)
	{
		CFNumberGetValue (numberRef, kCFNumberSInt32Type, &defaultVal);
		CFRelease (numberRef);
	}
	
   	return defaultVal;
#endif

#if AJALinux
	return defaultVal;
#endif
}

//---------------------------------------------------------------------------------
//	SetInt
//---------------------------------------------------------------------------------

void CNTV2Preferences::SetInt (const char *key, int defaultVal)
{
#if	MSWindows
	HKEY baseKey;
	
	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return;
	
	itoa (defaultVal, _prefString, 10);
	RegSetValueEx (baseKey, key, 0, REG_SZ, (unsigned char *)_prefString, (DWORD)strlen(_prefString)+1);
	RegCloseKey (baseKey);
#endif

#if AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFNumberRef numRef = CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt32Type, &defaultVal);
	CFPreferencesSetValue (keyRef, numRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);
 	CFRelease (numRef);
#endif

#if AJALinux
	// TBD - do nothing
#endif
}


//---------------------------------------------------------------------------------
//	Remove
//---------------------------------------------------------------------------------

void CNTV2Preferences::Remove()
{
#if	MSWindows
	RegDeleteKey (GetUser(), _prefID);
#endif

#if AJAMac
	// do nothing
#endif

#if AJALinux
	// TBD - do nothing
#endif
}

//---------------------------------------------------------------------------------
//	GetDouble
//---------------------------------------------------------------------------------

double CNTV2Preferences::GetDouble (const char *key, double defaultVal)
{
#if	MSWindows
	double result = defaultVal;
	HKEY baseKey;

	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return result;
	
	unsigned long type = REG_SZ;
	unsigned long size = PREF_STRING_SIZE;

	if (RegQueryValueEx (baseKey, key, NULL, &type, (unsigned char *)_prefString, &size) == ERROR_SUCCESS)
	{
		if (type == REG_SZ && size > 0) 
		{
			char *ptr = _prefString;
			result = strtod (_prefString, &ptr);
		}
	}
	RegCloseKey (baseKey);
	
	return result;
#endif

#if	AJAMac
	double result = defaultVal;

	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFNumberRef numberRef = (CFNumberRef) CFPreferencesCopyValue (keyRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);

	if (numberRef != NULL)
	{
		CFNumberGetValue (numberRef, kCFNumberDoubleType, &result);
		CFRelease (numberRef);
	}
	
   	return result;
#endif

#if AJALinux
	return defaultVal;
#endif
}

//---------------------------------------------------------------------------------
//	SetDouble
//---------------------------------------------------------------------------------

void CNTV2Preferences::SetDouble (const char *key, double defaultVal)
{
#if	MSWindows
	HKEY baseKey;
	
	if (RegCreateKeyEx (GetUser(), _prefID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &baseKey, NULL) != ERROR_SUCCESS) 
		return;
	
	gcvt (defaultVal, 6, _prefString);
	RegSetValueEx (baseKey, key, 0, REG_SZ, (unsigned char *)_prefString, (DWORD)strlen(_prefString)+1);
	RegCloseKey (baseKey);
#endif

#if AJAMac
	CFStringRef keyRef = CFStringCreateWithCString (kCFAllocatorDefault, key, kCFStringEncodingASCII);
	CFNumberRef numRef = CFNumberCreate (kCFAllocatorDefault, kCFNumberDoubleType, &defaultVal);
	CFPreferencesSetValue (keyRef, numRef, _prefID, GetUser(), kCFPreferencesCurrentHost);
	CFRelease (keyRef);
 	CFRelease (numRef);
#endif

#if AJALinux
	// TBD - do nothing
#endif
}

