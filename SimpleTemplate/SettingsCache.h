#ifndef SETTINGS_CACHE_H
#define SETTINGS_CACHE_H

#define SETTINGS_FILE "robot_settings.txt"

#include "SaveableSetting.h"
#include <vector>

class SettingsCache {
public:

	SettingsCache();
	~SettingsCache();
	
	static SettingsCache* getInstance();
	
	static void setValue(char* name, int value);
	static void setValue(char* name, double value);
	static void setValue(char* name, char* value);
	
	static void getByReference(char* name, int& value, int defaultVal);
	static void getByReference(char* name, double& value, double defaultVal);
	static void getByReference(char* name, char*& value, char* defaultVal);
	
protected:
	
	static SaveableSetting* getByName(char* name);

	static vector<SaveableSetting*> values;
	
	static SettingsCache* instance;
	
};

#endif
