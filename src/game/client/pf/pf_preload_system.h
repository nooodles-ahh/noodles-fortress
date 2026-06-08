#ifndef PF_PRELOAD_SYSTEM_H
#define PF_PRELOAD_SYSTEM_H
#pragma once

#include <igamesystem.h>
#include <tier1/UtlStringMap.h>
#include <engine/ivmodelinfo.h>

// Preload models on the client. Models still need to be pre-cached from the server,
// but this will reduce stuttering in the main menu and lessen pop-in if the player
// is using any mod_* convars for async loading
class CPFPreloadSystem : public CAutoGameSystem
{
public:
	CPFPreloadSystem( const char* name );
	static CPFPreloadSystem& GetInstance();
	void RegisterMDLPath(const char* path);

	virtual void PostInit();
	virtual void Shutdown();

private:
	void PreloadClientPrecache();
	MDLHandle_t PreloadMDL(const char* path);
private:
	CUtlStringMap<MDLHandle_t> m_stringMap;
	int m_oldStringMapCount;
};

#endif