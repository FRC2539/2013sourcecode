#include "SaveableSetting.h"
#include <cstdio>

SaveableSetting::SaveableSetting(char* n)
{
	name = n;
}

SaveableSetting::~SaveableSetting()
{

}

void SaveableSetting::set(int value)
{
	if (type == doubleType)
	{
		set(static_cast<double>(value));
		return;
	}
	intValue = value;
	type = integerType;
}

void SaveableSetting::set(double value)
{
	doubleValue = value;
	type = doubleType;
}

void SaveableSetting::set(char* value)
{
	charValue = value;
	type = charType;
}

void SaveableSetting::getByReference(int& reference)
{
	if (type != integerType)
	{
		throw sprintf("%s is not an integer", name);
	}
	
	reference = intValue;
}

void SaveableSetting::getByReference(double& reference)
{
	if (type != doubleType)
	{
		throw sprintf("%s is not a double", name);
	}
	
	reference = doubleValue;
}

void SaveableSetting::getByReference(char*& reference)
{
	if (type != charType)
	{
		throw sprintf("%s is not a char", name);
	}
	
	reference = charValue;
}

char* SaveableSetting::getName()
{
	return name;
}

char* SaveableSetting::getTypeMarker()
{
	if (type == integerType)
	{
		return "i";
	}
	else if (type == doubleType)
	{
		return "d";
	}
	else if (type == charType)
	{
		return "c";
	}
	
	return "";
}


char* SaveableSetting::getAsChar()
{
	char* stringValue = new char[32];
	if (type == integerType)
	{
		sprintf(stringValue, "%i", intValue);
	}
	else if (type == doubleType)
	{
		sprintf(stringValue, "%f", doubleValue);
	}
	else if (type == charType)
	{
		sprintf(stringValue, "%s", charValue);
	}
	
	return stringValue;
}
