#include "Urho3DAll.h"
#include "PlayerCamera.h"


PlayerCamera::PlayerCamera(Context* context) : LogicComponent(context)
{

	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);	
}

void PlayerCamera::RegisterObject(Context* context)
{
	context->RegisterFactory<PlayerCamera>();
}

void PlayerCamera::Start()
{
	// Настройки
	wheel_ = 0;
	minFollow_ = 2;
	maxFollow_ = 7;
	pitch_ = 45;
	yaw_ = 0;
	follow_ = 3;
	curFollow_ = follow_;
	followVel_ = 0;
	pos = Vector3::ZERO;
	newPos = Vector3::ZERO;
	posVelocity = Vector3::ZERO;
	hasObstacle = false;

	// болталка
	shakeMagnitude = 0.0f;
	shakeSpeed = 0.0f;
	shakeTime = 0.0f;
	shakeDamping = 0.0f;

	// ноды камеры
	shakeNode_ = GetNode()->GetChild("ShakeNode", true);
	angleNode_ = shakeNode_->GetChild("AngleNode", true);
	cameraNode = angleNode_->GetChild("CameraContainer", true);
	camera_ = cameraNode->GetComponent<Camera>(); // камера

	
	
	GetNode()->SetRotation(Quaternion(yaw_, Vector3(0,1,0)));
	cameraNode->SetPosition(Vector3(0,0, -follow_));
	angleNode_->SetRotation(Quaternion(pitch_, Vector3(1,0,0)));
	GetNode()->SetPosition(Vector3::ZERO);

	// цель - нода персонажа
	target_ = GetScene()->GetChild("PlayerNode", true);

	// подписаться на "болталку"
	SubscribeToEvent(StringHash("SHAKE"), URHO3D_HANDLER(PlayerCamera, HandleShakeEvent));

}

void PlayerCamera::Update(float timeStep)
{
	UpdateCamera(timeStep);
}

void PlayerCamera::UpdateCamera(float timeStep) 
{
	Input* input = GetSubsystem<Input>();
	const float MOVE_SPEED = 800.0f;
	const float MOUSE_SENSITIVITY = 0.1f;
	IntVector2 mouseMove = input->GetMouseMove();

	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	if (wheel_) follow_ -= wheel_ * timeStep * 200;
	
	if (follow_ < minFollow_) follow_ = minFollow_;
	if (follow_ > maxFollow_) follow_ = maxFollow_;



	
	// Аммортизация зума/дальности расположения камеры
	SpringFollow(timeStep);
	//curFollow_ = CameraTestObstacles(curFollow_, hasObstacle);
	// Тест на наличие преград перед камерой

	if (hasObstacle) 
	{
		follow_ = curFollow_;
		hasObstacle = false;
	}

	// Просчет болтания камеры (эффекты дрожания камеры от: ветра, взрыва)
	const float xOffsetFactor = 3.0;
	const float yOffsetFactor = 1.0;
	shakeTime = shakeTime + timeStep * shakeSpeed;
	float magnitudeForce = sin(shakeTime) * shakeMagnitude; 
	Vector3 shakePos = Vector3(sin(shakeTime*xOffsetFactor)*magnitudeForce, cos(shakeTime*yOffsetFactor)*magnitudeForce, 0.0f);
	shakeMagnitude -= shakeDamping * timeStep;
	if (shakeMagnitude < 0.0f) shakeMagnitude = 0.0f;

	// Зададим текущее смещение для болталки
	shakeNode_->SetPosition(shakePos);

	// Амортизация позиции для root node
	SpringPosition(timeStep);
	GetNode()->SetPosition(pos);

	GetNode()->SetRotation(Quaternion(yaw_, Vector3::UP));
	cameraNode->SetPosition(Vector3(0.0f, 0.0f, -curFollow_));
	angleNode_->SetRotation(Quaternion(pitch_, Vector3::RIGHT));
}

float PlayerCamera::CameraTestObstacles ( float followDistance, bool& hasObstacle )
{
	hasObstacle = false;
	Quaternion cd = cameraNode->GetWorldRotation();
	Vector3 dir = cameraNode->GetWorldPosition() - target_->GetWorldPosition();
	dir.Normalize();
	//Ray fromTargetToCam = Ray(target_->GetWorldPosition(), cd*-Vector3::ONE );
	Ray fromTargetToCam = Ray(target_->GetWorldPosition(), dir);
	
	
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, fromTargetToCam, RAY_TRIANGLE, followDistance, DRAWABLE_GEOMETRY, -1);

	Octree* octree = GetNode()->GetScene()->GetComponent<Octree>();
	octree->RaycastSingle(query);
	
	if (results.Size())
	{
		RayQueryResult& result = results[0];

		Variant v = result.node_->GetVar("noclip");
		if (v.GetInt() == 1) // if this some fx not clip follow 
		{
	
		}
		else 
		{
			if (result.distance_ >= 0) 
			{
				hasObstacle = true;
				return min(result.distance_, followDistance)-0.5f;
			}
		}
	}
	
	return followDistance;
}


void PlayerCamera::SpringFollow(float timeStep) 
{
	float deltaFollow = follow_ - curFollow_;
	float af = 9*deltaFollow - 6 * followVel_;
	//float af = 18 * deltaFollow - 12 * followVel_;
	
	followVel_ = followVel_ + timeStep* af;
	curFollow_ = curFollow_ + timeStep * followVel_;
}

void PlayerCamera::SpringPosition(float timeStep) 
{
	Vector3 d = newPos - pos;
	//Vector3 a = d * Vector3(8, 8, 8) - posVelocity * Vector3(1, 1, 1);
	Vector3 a = d * Vector3(16,16,16) - posVelocity * Vector3(8,8,8);
	//Vector3 a = d * Vector3(8, 8, 8) - posVelocity * Vector3(10, 10, 10);
	posVelocity = posVelocity + a * Vector3(timeStep, timeStep, timeStep);
	pos = pos + posVelocity * Vector3(timeStep, timeStep, timeStep);
	
}

void PlayerCamera::SetShake(float magnitude, float speed, float damping)
{
	shakeMagnitude = magnitude;
	shakeSpeed = speed;
	shakeDamping = damping;
}

void PlayerCamera::FixedUpdate(float timeStep)
{
	newPos = target_->GetWorldPosition();
	Input* input = GetSubsystem<Input>();
	
	wheel_ = input->GetMouseMoveWheel();

	curFollow_ = CameraTestObstacles(curFollow_, hasObstacle);
	
}

void PlayerCamera::SetTargetNode(Node* target) 
{
	target_ = target;	
}

void PlayerCamera::SetMinMaxDistance(float minDistance, float maxDistance)
{
	minFollow_ = minDistance;
	maxFollow_ = maxDistance;
}

/// Реакция на событие взрыв
void PlayerCamera::HandleShakeEvent(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;

	Vector3 BlastPos = eventData[P_DATA].GetVector3();

	float distanceTo = (GetNode()->GetWorldPosition() - BlastPos).Length();

	if (distanceTo < 30.0f) // если взрыв достаточно близко 
	{
		SetShake(0.7f, 5.0f, 2.0f);	
		
	}
}