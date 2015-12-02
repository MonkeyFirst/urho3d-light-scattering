#include "Urho3DAll.h"
#include "GameScene.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "DebugUI.h"

using namespace Urho3D;

class MyApp : public Application
{
public:
    SharedPtr<GameScene> gameScene;
    SharedPtr<Player> player;
    SharedPtr<DebugUI> debugUI;
    SharedPtr<Node> SunNode;

    MyApp(Context* context) :
        Application(context)
    {
        PlayerCamera::RegisterObject(context);
    }

	virtual void Setup()
	{
		// Called before engine initialization. engineParameters_ member variable can be modified here
		engineParameters_["WindowTitle"] = "Light scattering";
		engineParameters_["FullScreen"] = false;
		engineParameters_["Headless"] = false;
		engineParameters_["WindowWidth"] = 1280;
		engineParameters_["WindowHeight"] = 768;
		engineParameters_["LogName"] = GetTypeName() + ".log";
		engineParameters_["VSync"] = false;
		engineParameters_["FrameLimiter"] = false;
		engineParameters_["WorkerThreads"] = true;
		engineParameters_["RenderPath"] = "CoreData/RenderPaths/ForwardSun.xml";
		//engineParameters_["RenderPath"] = "CoreData/RenderPaths/Forward.xml";
		engineParameters_["Sound"] = false;
    }

	virtual void Start()
	{
		// Called after engine initialization. Setup application & subscribe to events here
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));
		SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(MyApp, HandleFixedUpdate));
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(MyApp, HandleMouseButtonDown));
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));
		//SubscribeToEvent(E_RENDERPATHPASS, URHO3D_HANDLER(MyApp, HandleRenderPass));
        

		ResourceCache* cache = context_->GetSubsystem<ResourceCache>();

		gameScene = new GameScene(context_);
		gameScene->Create("Scene1.xml");

		player = new Player(context_);
		player->Create(gameScene, "PlayerNode");

		debugUI = new DebugUI(context_);
		debugUI->CreateDebugUI();

        SunNode = gameScene->scene->GetChild("SUN", true);

        debugUI->ToggleDebugUI();
        
    }

    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated

    }

	void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace MouseButtonDown;
		int button = eventData[P_BUTTON].GetInt();

		if (button == MOUSEB_LEFT) 
		{
		}
	}

    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
		ResourceCache* cache = context_->GetSubsystem<ResourceCache>();

        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESC)
            engine_->Exit();

        if (key == KEY_F2)
            debugUI->ToggleDebugUI();

        if (key == KEY_F3)
        {
            static bool isSunnyDay = true;
            static ResourceCache* cache = GetSubsystem<ResourceCache>();

            if (isSunnyDay) 
                player->vp->SetRenderPath(cache->GetResource<XMLFile>("CoreData/RenderPaths/ForwardSunFast.xml"));
            else
                player->vp->SetRenderPath(cache->GetResource<XMLFile>("CoreData/RenderPaths/ForwardSun.xml"));

            isSunnyDay = !isSunnyDay;            
        }

        if (key == KEY_F4)
        {
            static ResourceCache* cache = GetSubsystem<ResourceCache>();
            player->vp->SetRenderPath(cache->GetResource<XMLFile>("CoreData/RenderPaths/Forward.xml"));
       }    

    }

	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;
		float timeStep = eventData[P_TIMESTEP].GetFloat();
	}

	void HandleFixedUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;
		float timeStep = eventData[P_TIMESTEP].GetFloat();	
	}

	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
        Viewport* vp = player->vp;
        if (!vp) return;
        
        RenderPath* rp = vp->GetRenderPath();
        Vector2 sunPos = player->camera->WorldToScreenPoint(SunNode->GetWorldPosition());
        sunPos.y_ = 1.0 - sunPos.y_; //GL have revert Y screen coords

        // Send current sun position on screen to post-process shader
        rp->SetShaderParameter("LightPositionOnScreen", Variant(sunPos));

        debugUI->debugHud->SetAppStats(" Sun screen position ", String(Vector2(sunPos)));
        debugUI->debugHud->SetAppStats(" Change Light scattering RenderPath (Std/Lowres) Key =", String("F3"));
        debugUI->debugHud->SetAppStats(" Change RenderPath to Std Forward Key = ", String("F4"));

	}
};

URHO3D_DEFINE_APPLICATION_MAIN(MyApp)