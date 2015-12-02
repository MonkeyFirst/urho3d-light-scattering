#include "Urho3DAll.h"
#include "DebugUI.h"


DebugUI::DebugUI(Context* context) : Object(context)
{
	

}
DebugUI::~DebugUI()
{

}

void DebugUI::CreateDebugUI() 
{

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	Engine* engine = GetSubsystem<Engine>();

	if (!engine) return;

	// Create console
	Console* console = engine->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);



	debugHud = engine->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
	//debugHud->SetMode(DEBUGHUD_SHOW_ALL | DEBUGHUD_SHOW_ALL_MEMORY);
}
void DebugUI::ToggleDebugUI()
{
	if (debugHud)
		debugHud->ToggleAll();
}