#pragma once

class GameScene;
class PlayerCamera;

enum PlayerActions 
{
	PA_IDLE = 0,
	PA_WALK,
	PA_JUMP,
	PA_ATTACK,
	PA_LAST
};

enum PlayerHelpers
{
	H_FORWARD = 0,
	H_BACK,
	H_RIGHT,
	H_LEFT,
	H_UP,
	H_LAST
};

const String helpersNames[H_LAST] = { "Forward", "Back", "Right", "Left", "Up" };

class Player : public Object
{
	URHO3D_OBJECT(Player, Object)
public:
	Player(Context* context);
	~Player();
	bool Create(GameScene* gameScene_, String nodeName);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);
	void HandleMouseButton(StringHash eventType, VariantMap& eventData);
	void MovePlayer(float timeStep);
	void MovePlayerByJoystick(float timeStep);


	PlayerActions currentAction;
	float speed;
	SharedPtr<Node> playerNode;
	SharedPtr<Node> characterNode;
	SharedPtr<Node> cameraNode;
	// helpers
	Vector<SharedPtr<Node>> helper;

	SharedPtr<RigidBody> body;
	SharedPtr<Camera> camera;
	SharedPtr<Viewport> vp;
	SharedPtr<PlayerCamera> tpCamera;
	
	bool isActive;
	bool isLoaded;
	SharedPtr<GameScene> gameScene;


	

};

