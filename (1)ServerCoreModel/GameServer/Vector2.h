#pragma once
struct Vector2
{
public:
	Vector2(float assignedX, float assignedY) : x(assignedX), y(assignedY) {}
	Vector2() {}

	inline Vector2 operator + (const Vector2& rhs) const noexcept
	{
		float returnX = x + rhs.x;
		float returnY = y + rhs.y;
		return Vector2(returnX, returnY);
	}

	inline Vector2 operator - (const Vector2& rhs) const noexcept
	{
		float returnX = x - rhs.x;
		float returnY = y - rhs.y;
		return Vector2(returnX, returnY);
	}

	inline static float Distance(Vector2 firstVec, Vector2 secondVec)
	{
		float dx = firstVec.x - secondVec.x;
		float dy = firstVec.y - secondVec.y;

		return std::sqrt(dx * dx + dy * dy);
	}

	inline static Vector2 BetweenPoint(Vector2 firstVec, Vector2 secondVec, UINT8 percentage)
	{
		if (percentage > 100)
		{
			CRASH("NOT_BETWEEN_0_100");
		}

		float secondWeight = static_cast<float>(percentage) * 0.01f;
		float firstWeight = 1.0f - secondWeight;


		float x = firstVec.x * firstWeight + secondVec.x * secondWeight;
		float y = firstVec.y * firstWeight + secondVec.y * secondWeight;

		return Vector2(x, y);
	}

public:
	float x;
	float y;
};