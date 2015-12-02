#include "Urho3DAll.h"
#include "GameScene.h"


GameScene::GameScene(Context* context) : Object(context), isActive(false), isLoaded(false)
{

}

GameScene::~GameScene()
{

}

void GameScene::Create(String sceneName) 
{
	scene = SharedPtr<Scene>(new Scene(context_));
	File sceneFile(context_, context_->GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/" + sceneName, FILE_READ);
	isLoaded = scene->LoadXML(sceneFile);

	//scene->SetTimeScale(2.0f);
}
