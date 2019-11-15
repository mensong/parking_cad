#pragma once
#include <string>
#include <map>
#include "FileHelper.h"
#include <algorithm>

class ModulesManager
{
public:
	static ModulesManager& Instance();
	static void Relaese();
	
public:
	~ModulesManager();

public:
	template<typename FN>
	FN func(const std::string& moduleName, const std::string& fnName)
	{
		std::string upName = FileHelper::GetFileNameFromPath(moduleName);
		std::transform(upName.begin(), upName.end(), upName.begin(), ::toupper);
		if (m_handles.find(upName.c_str()) == m_handles.end())
		{
			if (!LoadModule(moduleName))
				return NULL;
		}

		HMODULE h = m_handles[upName.c_str()];
		return (FN)GetProcAddress(h, fnName.c_str());
	}

	template<typename FN>
	FN try_func(const std::string& moduleName, const std::string& fnName) 
	{
		std::string upName = FileHelper::GetFileNameFromPath(moduleName);
		std::transform(upName.begin(), upName.end(), upName.begin(), ::toupper);
		if (m_handles.find(upName.c_str()) == m_handles.end())
		{
			if (!LoadModule(moduleName))
				throw 0;
		}

		HMODULE h = m_handles[upName.c_str()];
		FN p = (FN)GetProcAddress(h, fnName.c_str());
		if (!p)
			throw 1;

		return p;
	}

	bool LoadModule(const std::string& path);

private:
	static ModulesManager* ms_instance;
	std::map<std::string, HMODULE> m_handles;
};

