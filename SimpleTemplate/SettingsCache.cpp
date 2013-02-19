#include "SettingsCache.h"
#include <iostream>
#include <fstream>

using namespace std;

SettingsCache::SettingsCache()
{
	ifstream settings(settingsFile);
	if (settings.is_open() == false)
	{
		return;
	}
	
	SaveableSetting* setting;
	char* name;
	char* type;
	
	int intVal;
	double doubleVal;
	char* charVal;
	
	while (settings)
	{
		settings >> name;
		setting = new SaveableSetting(name);
		
		settings >> type;
		
		if (type == "i")
		{
			settings >> intVal;
			setting->set(intVal);
		}
		else if (type == "d")
		{
			settings >> doubleVal;
			setting->set(doubleVal);
		}
		else if (type == "c")
		{
			settings >> charVal;
			setting->set(doubleVal);
		}
		
		values.push_back(setting);
	}
	
	instance = &this;
}

SettingsCache::~SettingsCache()
{
	ofstream settings(settingsFile);
	
	vector<SaveableSetting>::iterator i;
	int sz = values.size();
	for (int i = 0; i < sz; i++)
	{
		settings << values[i]->getName() << endl;
		settings << values[i]->getTypeMarker() << endl;
		settings << values[i]->getAsChar() << endl;
	}
}

SettingsCache* SettingsCache::getInstance()
{
	return instance;
}


void SettingsCache::setValue(char* name, int value)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{   
		store = new SaveableSetting(name);
		values.push_back(store);
	}
	store->set(value);
}

void SettingsCache::setValue(char* name, double value)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{   
		store = new SaveableSetting(name);
		values.push_back(store);
	}
	store->set(value);
}

void SettingsCache::setValue(char* name, char* value)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{   
		store = new SaveableSetting(name);
		values.push_back(store);
	}
	store->set(value);
}

void SettingsCache::getByReference(char* name, int& value, int defaultVal)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{
		value = defaultVal;
		return;
	}
	
	store->getByReference(value);
}

void SettingsCache::getByReference(char* name, double& value, double defaultVal)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{
		value = defaultVal;
		return;
	}
	
	store->getByReference(value);
}

void SettingsCache::getByReference(char* name, char*& value, char* defaultVal)
{
	SaveableSetting* store = getByName(name);
	
	if (store == (*0))
	{
		value = defaultVal;
		return;
	}
	
	store->getByReference(value);
}


SaveableSetting* SettingsCache::getByName(char* name)
{	
	vector<SaveableSetting>::iterator i;
	int sz = values.size();
	for (int i = 0; i < sz; i++)
	{
		if (values[i]->getName() == name)
		{
			return values[i];
		}
	}
	
	return (*0);
}
