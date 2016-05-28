#include "PrecompiledHeader.hpp"
#include "Application\Application.hpp"

namespace
{
	class SourceDemoRenderPlugin final : public IServerPluginCallbacks
	{
	public:
		virtual bool Load(CreateInterfaceFn interfacefactory, CreateInterfaceFn gameserverfactory) override;
		virtual void Unload() override;

		virtual void Pause() override
		{
			
		}

		virtual void UnPause() override
		{
			
		}

		virtual const char* GetPluginDescription() override
		{
			return "Source Demo Render";
		}

		virtual void LevelInit(char const* mapname) override
		{
			
		}

		virtual void ServerActivate(edict_t* edictlist, int edictcount, int maxclients) override
		{
			
		}

		virtual void GameFrame(bool simulating) override
		{
			
		}

		virtual void LevelShutdown() override
		{
			
		}

		virtual void ClientActive(edict_t* entity) override
		{
			
		}

		virtual void ClientDisconnect(edict_t* entity) override
		{
			
		}

		virtual void ClientPutInServer(edict_t* entity, char const* playername) override
		{
			
		}

		virtual void SetCommandClient(int index) override
		{
			
		}

		virtual void ClientSettingsChanged(edict_t* entity) override
		{
			
		}

		virtual PLUGIN_RESULT ClientConnect(bool* allowconnect,
											edict_t* entity,
											const char* name,
											const char* address,
											char* rejectreason,
											int maxrejectlen) override
		{
			return PLUGIN_CONTINUE;
		}

		virtual PLUGIN_RESULT ClientCommand(edict_t* entity, const CCommand& args) override
		{
			return PLUGIN_CONTINUE;
		}

		virtual PLUGIN_RESULT NetworkIDValidated(const char* username, const char* networkid) override
		{
			return PLUGIN_CONTINUE;
		}

		virtual void OnQueryCvarValueFinished(QueryCvarCookie_t cookie,
											  edict_t* playerentity,
											  EQueryCvarValueStatus status,
											  const char *cvarname,
											  const char *cvarvalue) override
		{
			
		}

		virtual void OnEdictAllocated(edict_t* entity) override
		{
			
		}

		virtual void OnEdictFreed(const edict_t* entity) override
		{
			
		}
	};

	SourceDemoRenderPlugin ThisPlugin;
	SDR::EngineInterfaces Interfaces;

	class InterfaceBase
	{
	public:
		InterfaceBase(const char* interfacename) :
			InterfaceName(interfacename)
		{
			
		}

		virtual bool Create(CreateInterfaceFn interfacefactory, CreateInterfaceFn gameserverfactory) = 0;

		const char* GetInterfaceName() const
		{
			return InterfaceName;
		}

	protected:
		const char* InterfaceName;
	};

	template <typename T>
	class InterfaceFactoryLocal final : public InterfaceBase
	{
	public:
		InterfaceFactoryLocal(std::vector<InterfaceBase*>& container, T** targetinterface, const char* interfacename) :
			InterfaceBase(interfacename),
			TargetInterface(targetinterface)
		{
			container.push_back(this);
		}

		virtual bool Create(CreateInterfaceFn interfacefactory, CreateInterfaceFn gameserverfactory) override
		{
			*TargetInterface = static_cast<T*>(interfacefactory(InterfaceName, nullptr));

			return TargetInterface != nullptr;
		}

	private:
		T** TargetInterface;
	};

	template <typename T>
	class ServerFactoryLocal final : public InterfaceBase
	{
	public:
		ServerFactoryLocal(std::vector<InterfaceBase*>& container, T** targetinterface, const char* interfacename) :
			InterfaceBase(interfacename),
			TargetInterface(targetinterface)
		{
			container.push_back(this);
		}

		virtual bool Create(CreateInterfaceFn interfacefactory, CreateInterfaceFn gameserverfactory) override
		{
			*TargetInterface = static_cast<T*>(gameserverfactory(InterfaceName, nullptr));

			return TargetInterface != nullptr;
		}

	private:
		T** TargetInterface;
	};

	bool SourceDemoRenderPlugin::Load(CreateInterfaceFn interfacefactory, CreateInterfaceFn gameserverfactory)
	{
		ConnectTier1Libraries(&interfacefactory, 1);
		ConVar_Register();

		std::vector<InterfaceBase*> addedinterfaces;

		ServerFactoryLocal<IPlayerInfoManager> T1(addedinterfaces, &Interfaces.PlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);

		for (auto ptr : addedinterfaces)
		{
			auto res = ptr->Create(interfacefactory, gameserverfactory);

			if (!res)
			{
				Warning("SDR: Failed to get the \"%s\" interface\n", ptr->GetInterfaceName());
				return false;
			}
		}

		Interfaces.Globals = Interfaces.PlayerInfoManager->GetGlobalVars();

		auto res = SDR::Setup();

		if (!res)
		{
			return false;
		}

		Msg("SDR: Source Demo Render loaded\n");

		return true;
	}

	void SourceDemoRenderPlugin::Unload()
	{
		ConVar_Unregister();
		DisconnectTier1Libraries();

		SDR::Close();
	}
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR
(
	SourceDemoRenderPlugin,
	IServerPluginCallbacks,
	INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
	ThisPlugin
);

const SDR::EngineInterfaces& SDR::GetEngineInterfaces()
{
	return Interfaces;
}