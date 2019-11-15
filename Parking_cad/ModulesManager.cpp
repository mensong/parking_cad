#include "stdafx.h"
#include "ModulesManager.h"
#include "FileHelper.h"


ModulesManager* ModulesManager::ms_instance = NULL;


ModulesManager& ModulesManager::Instance()
{
	if (!ms_instance)
		ms_instance = new ModulesManager;
	return *ms_instance;
}

void ModulesManager::Relaese()
{
	if (ms_instance)
	{
		delete ms_instance;
		ms_instance = NULL;
	}
}

ModulesManager::~ModulesManager()
{
	for (std::map<std::string, HMODULE>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
	{
		FreeLibrary(it->second);
	}
	m_handles.clear();
}

bool ModulesManager::LoadModule(const std::string& path)
{
	HMODULE h = LoadLibraryA(path.c_str());
	if (!h)
		return false;

	std::string fileName = FileHelper::GetFileNameFromPath(path);
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);

	m_handles[fileName.c_str()] = h;

	return true;
}
