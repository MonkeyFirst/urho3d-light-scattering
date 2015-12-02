
class DebugUI : public Object
{
	URHO3D_OBJECT(DebugUI, Object)
public:
	DebugUI(Context* context);
	~DebugUI();

	void CreateDebugUI();
	void ToggleDebugUI();

	SharedPtr<DebugHud> debugHud;


};