#ifndef SAVEABLE_SETTING_H
#define SAVEABLE_SETTING_H

class SaveableSetting {
public:
	
	SaveableSetting(char* name);
	~SaveableSetting();
	
	void set(int value);
	void set(double value);
	void set(char* value);
	
	void getByReference(int& reference);
	void getByReference(double& reference);
	void getByReference(char* &reference);
	
	char* getName();
	char* getTypeMarker();
	char* getAsChar();
	
protected:
	
	typedef enum ValueType {
		integerType,
		doubleType,
		charType
	};

	int intValue;
	double doubleValue;
	char* charValue;  
	
	ValueType type;
	char* name;
};

#endif
