#include "stdafx.h"
#include "ModulesManager.h"
#include "FileHelper.h"
#include "Convertor.h"


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
	std::string ex = FileHelper::GetFileExtensionFromPath(path);
	std::transform(ex.begin(), ex.end(), ex.begin(), ::toupper);
	bool b = false;
	if (ex == "DLL")
		b = loadDllModule(path);
	else
		b = loadArxModule(path);

	return b;
}

bool ModulesManager::addDir(const std::string& sDir)
{
	if (sDir.size() < 1)
		return false;

	for (int i=0; i<m_dirs.size(); ++i)
	{
		if (FileHelper::IsSamePathA(m_dirs[i].c_str(), sDir.c_str()))
			return false;
	}

	char end = sDir[sDir.size() - 1];
	if (end != '\\' && end != '/')
		m_dirs.push_back(sDir + '/');
	m_dirs.push_back(sDir);

	return true;
}

bool ModulesManager::loadDllModule(const std::string& path)
{
	HMODULE h = LoadLibraryA(path.c_str());
	if (!h)
	{
		for (int i=0; i<m_dirs.size(); ++i)
		{
			std::string sTestPath = (m_dirs[i] + path);
			h = LoadLibraryA(sTestPath.c_str());
			if (h)
				break;
		}
	}

	if (!h)
		return false;

	std::string fileName = FileHelper::GetFileNameFromPath(path);
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);
	m_handles[fileName.c_str()] = h;

	return true;
}

bool ModulesManager::loadArxModule(const std::string& path)
{
	AcString sPath = 
#ifdef _UNICODE
		GL::Ansi2WideByte(path.c_str()).c_str()
#else
		path.c_str()
#endif
		;

	int res = acedArxLoad(sPath);
	for (int i=0; i<m_dirs.size() && res != RTNORM; ++i)
	{
		std::string sTestPath = (m_dirs[i] + path);
		sPath = 
#ifdef _UNICODE
			GL::Ansi2WideByte(sTestPath.c_str()).c_str()
#else
			sTestPath.c_str()
#endif
			;
		res = acedArxLoad(sPath);
	}

	std::string fileName = FileHelper::GetFileNameFromPath(path);
	HMODULE h = GetModuleHandleA(fileName.c_str());
	if (!h)
		return false;

	std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);
	m_handles[fileName.c_str()] = h;

	return true;
}
