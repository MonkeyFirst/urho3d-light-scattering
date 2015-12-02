#pragma once


class PlayerCamera: public LogicComponent 
{
	URHO3D_OBJECT(PlayerCamera, LogicComponent);
public:
	PlayerCamera(Context* context);

	static void RegisterObject(Context* context);

	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);

	void UpdateCamera(float timeStep);
	
	void SetTargetNode(Node* target);
	void SetMinMaxDistance(float minDistance, float maxDistance);

	void HandleShakeEvent(StringHash eventType, VariantMap& eventData);
	
	SharedPtr<Node> cameraNode;

private:

	SharedPtr<Node> target_;
	SharedPtr<Node> shakeNode_;
	SharedPtr<Node> angleNode_;
	
	SharedPtr<Camera> camera_;

	bool hasObstacle;

	int wheel_;
	float lastCollisionTime;

	float minFollow_;
	float maxFollow_;

	float pitch_;
	float yaw_;
	float follow_;
	float curFollow_;
	float followVel_;
	Vector3 pos;
	Vector3 newPos;
	Vector3 posVelocity;

	float shakeMagnitude;
	float shakeSpeed;
	float shakeTime;
	float shakeDamping;

	void SetShake(float magnitude, float speed, float damping);
	void SpringFollow(float timeStep);
	void SpringPosition(float timeStep);
	float CameraTestObstacles(float followDistance, bool& hasObstacle);


};