#pragma once
class GameScene : public Object
{
	URHO3D_OBJECT(GameScene, Object)
public:
	GameScene(Context* context);
	~GameScene();

	void Create(String sceneName);
	
	
	bool isActive;
	bool isLoaded;
	SharedPtr<Scene> scene;
	SharedPtr<Context> context;

};

