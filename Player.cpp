#include "Urho3DAll.h"
#include "Player.h"
#include "GameScene.h"
#include "PlayerCamera.h"

Player::Player(Context* context) : Object(context), isActive(false), isLoaded(false), gameScene(0)
{

}

Player::~Player()
{

}


bool Player::Create(GameScene* gameScene_, String nodeName)
{
	gameScene = SharedPtr<GameScene>(gameScene_);
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	cameraNode = gameScene->scene->GetChild("CameraContainer", true);
	if (cameraNode)
	{
		camera = cameraNode->GetComponent<Camera>();
		if (camera)
		{
			vp = SharedPtr<Viewport>(new Viewport(context_, gameScene->scene, camera));
			//vp->SetRenderPath(cache->GetResource<XMLFile>("CoreData/RenderPaths/ForwardHWDepthDOF.xml"));
			Renderer* renderer = GetSubsystem<Renderer>();
			renderer->SetViewport(0, vp);
		}
	}

	playerNode = gameScene->scene->GetChild("PlayerNode", true);
	body = playerNode->GetComponent<RigidBody>();
	Node* cam = gameScene->scene->GetChild("CameraNode", true);
	tpCamera = cam->CreateComponent<PlayerCamera>();
	tpCamera->SetTargetNode(playerNode);
	characterNode = playerNode->GetChild("CharacterNode", true);

	

	//body->SetMass(0);

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Player, HandleKeyDown));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Player, HandleUpdate));
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(Player, HandleFixedUpdate));
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Player, HandleMouseButton));

	// Collect all helpers
	helper.Resize(H_LAST);
	for (int i = 0; i < H_LAST; i++) 
	{
		helper[i] = playerNode->GetChild(helpersNames[i], true);
	}
	

	return true;
}

void Player::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

}
void Player::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
	float timeStep = eventData[P_TIMESTEP].GetFloat();
	
}

void Player::HandleFixedUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	//MoveCamera(timeStep);
	MovePlayer(timeStep);
	//MovePlayerByJoystick(timeStep);
}

void Player::HandleMouseButton(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;

}

void Player::MovePlayer(float timeStep)
{

	Input* input = GetSubsystem<Input>();

	Quaternion camRotation = tpCamera->cameraNode->GetWorldRotation();
	Vector3 campos = tpCamera->cameraNode->GetWorldPosition();
	Vector3 playerpos = playerNode->GetWorldPosition();
	Vector3 camXZpos = Vector3(campos.x_, playerpos.y_, campos.z_);

	Vector3 diff = playerpos - camXZpos;
	diff.Normalize();
	Quaternion q;
	q.FromLookRotation(diff, Vector3::UP);
	

	//campos.y_ = 0.0f;
	//playerpos.y_ = 0.0f;
	//Vector3 diff = playerpos - campos;
	//camRotation.FromLookRotation(diff, Vector3::UP);
	

	Vector3 dir = Vector3::ZERO;


	JoystickState* js = input->GetJoystick(1);

	
	if (input->GetKeyDown('W'))
		dir += Vector3::FORWARD;

	
	if (input->GetKeyDown('S'))
		dir += Vector3::BACK;

	if (input->GetKeyDown('A'))
		dir += Vector3::LEFT;

	if (input->GetKeyDown('D'))
		dir += Vector3::RIGHT;

	if (input->GetKeyDown(KEY_SPACE))
	{
		dir += Vector3::UP;
	}

	if (dir.Length() > 0)
	{
		Vector3 curVel = body->GetLinearVelocity();
		if (curVel.Length() < 5.0f)
		{
			float multipler = 1.0f;
			//if (input->GetQualifierDown(KEY_SHIFT)) 
			if (input->GetKeyDown(KEY_LSHIFT))
			{
				multipler = 2.0f;
			}

			Vector3 offset = q * dir;
			/*offset.y_ = 0.0f;
			Quaternion q = characterNode->GetWorldRotation();
			q.FromLookRotation(-dir, Vector3::UP);*/
			
			//Vector3 fw = playerNode->GetWorldPosition() + playerNode->GetWorldRotation() * Vector3::FORWARD;
			//characterNode->LookAt(helper[H_FORWARD]->GetWorldPosition(), Vector3::UP, TS_WORLD);
			characterNode->SetWorldRotation(q);
			body->SetLinearVelocity(multipler * 300.0f * (offset) * timeStep);
			//playerNode->Translate(camRotation * dir * timeStep * 10.0f);
		}
	}
}

void Player::MovePlayerByJoystick(float timeStep)
{

	Input* input = GetSubsystem<Input>();

	Quaternion camRotation = tpCamera->cameraNode->GetWorldRotation();
	Vector3 campos = tpCamera->cameraNode->GetWorldPosition();
	Vector3 playerpos = playerNode->GetWorldPosition();
	Vector3 camXZpos = Vector3(campos.x_, playerpos.y_, campos.z_);

	Vector3 diff = playerpos - camXZpos;
	diff.Normalize();
	Quaternion q;
	q.FromLookRotation(diff, Vector3::UP);


	//campos.y_ = 0.0f;
	//playerpos.y_ = 0.0f;
	//Vector3 diff = playerpos - campos;
	//camRotation.FromLookRotation(diff, Vector3::UP);


	Vector3 dir = Vector3::ZERO;
	if (input->GetNumJoysticks() > 0)
	{

		JoystickState* js = input->GetJoystickByIndex(0);
	
		//js->hats_[0] == 1

		float mx = js->axes_[0];
		float my = -js->axes_[1];

		//if (js->axes_[1] < -0.01f)
		//	dir += Vector3::FORWARD;

		//if (js->axes_[1] > 0.01f)
		//	dir += Vector3::BACK;

		//if (js->axes_[0] < -0.01f)
		//	dir += Vector3::LEFT;

		//if (js->axes_[0] > 0.01f)
		//	dir += Vector3::RIGHT;

		dir = Vector3(mx, 0.0f, my);

		if (js->buttons_[0] == true)
		{
			dir += Vector3::UP;
			
		}

		if (dir.Length() > 0)
		{
			Vector3 curVel = body->GetLinearVelocity();
			if (curVel.Length() < 5.0f)
			{
				float multipler = 1.0f;
				//if (input->GetQualifierDown(KEY_SHIFT)) 
				if (input->GetKeyDown(KEY_LSHIFT))
				{
					multipler = 2.0f;
				}

				Vector3 offset = q * dir;
				/*offset.y_ = 0.0f;
				Quaternion q = characterNode->GetWorldRotation();
				q.FromLookRotation(-dir, Vector3::UP);*/

				//Vector3 fw = playerNode->GetWorldPosition() + playerNode->GetWorldRotation() * Vector3::FORWARD;
				//characterNode->LookAt(helper[H_FORWARD]->GetWorldPosition(), Vector3::UP, TS_WORLD);
				//characterNode->SetWorldRotation(q);
				body->SetLinearVelocity(multipler * 300.0f * (offset)* timeStep);
				//playerNode->Translate(camRotation * dir * timeStep * 10.0f);
			}
		}
	}
}