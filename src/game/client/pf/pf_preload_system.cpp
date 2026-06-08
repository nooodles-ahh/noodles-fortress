#include "cbase.h"
#include "pf_preload_system.h"
#include <tier3/mdlutils.h>
#include <filesystem.h>
#include <filesystem/IQueuedLoader.h>
#include <tier2/tier2.h>

static CPFPreloadSystem s_pfPreloadSystem("PreloadSystem");

CPFPreloadSystem::CPFPreloadSystem(const char* name) : CAutoGameSystem( name )
{
}

CPFPreloadSystem &CPFPreloadSystem::GetInstance()
{
	return s_pfPreloadSystem;
}

void CPFPreloadSystem::RegisterMDLPath( const char *path )
{
	if( m_stringMap.Find( path ) == UTL_INVAL_SYMBOL )
		m_stringMap[path] = MDLHANDLE_INVALID;
}

void CPFPreloadSystem::PostInit()
{
	bool bAsyncAnims = mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, false );
	bool bAsyncVModel = mdlcache->SetAsyncLoad( MDLCACHE_VIRTUALMODEL, false );
	bool bAsyncVertex = mdlcache->SetAsyncLoad( MDLCACHE_VERTEXES, false );
	bool bAsyncDecodedAnims = mdlcache->SetAsyncLoad( MDLCACHE_DECODEDANIMBLOCK, false );
	bool bAsyncVCollide = mdlcache->SetAsyncLoad( MDLCACHE_VCOLLIDE, false );

	int count = m_stringMap.GetNumStrings();
	for ( int i = 0; i < count; i++ )
	{
		m_stringMap[i] = PreloadMDL( m_stringMap.String( i ) );
	}

	PreloadClientPrecache();

	mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, bAsyncAnims );
	mdlcache->SetAsyncLoad( MDLCACHE_VIRTUALMODEL, bAsyncVModel );
	mdlcache->SetAsyncLoad( MDLCACHE_VERTEXES, bAsyncVertex );
	mdlcache->SetAsyncLoad( MDLCACHE_DECODEDANIMBLOCK, bAsyncDecodedAnims );
	mdlcache->SetAsyncLoad( MDLCACHE_VCOLLIDE, bAsyncVCollide );
}

void CPFPreloadSystem::Shutdown()
{
	int count = m_stringMap.GetNumStrings();
	for(int i = 0; i < count; i++)
	{
		mdlcache->Release( m_stringMap[i] );
	}
}

void CPFPreloadSystem::PreloadClientPrecache()
{
	const char *pFilename = "scripts/client_precache.txt";
	KeyValues *pValues = new KeyValues( "ClientPrecache" );

	if ( !pValues->LoadFromFile( filesystem, pFilename, "GAME" ) )
	{
		Error( "Can't open %s for client precache info.", pFilename );
		pValues->deleteThis();
		return;
	}

	for ( KeyValues *pData = pValues->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey() )
	{
		const char *pszType = pData->GetName();
		const char *pszFile = pData->GetString();

		if ( Q_strlen( pszType ) > 0 &&
			Q_strlen( pszFile ) > 0 )
		{
			if ( !Q_stricmp( pData->GetName(), "model" ) )
			{
				PreloadMDL( pszFile );
			}
		}
	}

	pValues->deleteThis();
}

MDLHandle_t CPFPreloadSystem::PreloadMDL( const char *path )
{
	MDLHandle_t hMDLFindResult = MDLHANDLE_INVALID;
	if ( path && path[0] != '\0' )
	{
		hMDLFindResult = mdlcache->FindMDL( path );
		if ( mdlcache->IsErrorModel( hMDLFindResult ) )
			hMDLFindResult = MDLHANDLE_INVALID;

		if ( hMDLFindResult != MDLHANDLE_INVALID )
		{
			CMDL mdl;
			mdl.SetMDL( hMDLFindResult );
			VMatrix mat;
			MatrixSetIdentity( mat );
			mdl.Draw( mat.As3x4() );
		}
	}
	if( hMDLFindResult == MDLHANDLE_INVALID)
		DevMsg( 2, "Failed to preload %s\n", path );
	else
		DevMsg( 2, "Preloaded %s\n", path );

	return hMDLFindResult;
}