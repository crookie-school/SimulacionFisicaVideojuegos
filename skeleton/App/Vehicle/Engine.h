#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include "../../ImGui/imgui.h"
#include "Wheel.h"

struct EngineDesc
{
	float differentialRatio;
	float transmissionEfficieny = 0.7f;
	float idleRPM = 1000.f;
	std::vector<float> gears;
	std::vector<std::pair<float, float>> RPMtoTorqueCurve; // <RPM, torque> pairs
};

class Engine
{
public:
	Engine(const EngineDesc& desc);

	void attachWheel(Wheel* w) { wheelsAttached.push_back(w); }
	void detachWheel(Wheel* w) { wheelsAttached.erase(std::find(wheelsAttached.begin(), wheelsAttached.end(), w)); }
	bool isWheelAttached(Wheel* w) { std::find(wheelsAttached.begin(), wheelsAttached.end(), w) != wheelsAttached.end(); }

	void setThrottlePosition(float amount) { throttlePosition = amount; } // 1-0
	float getThrottlePosition() const { return throttlePosition; }

	int getCurrentGear() const { return currentGear; }
	void changeGear(int gear)
	{
		assert(gear >= 0 || gear < gears.size());
		currentGear = gear;
	}

	void update(float dt);

	void drawGui();

private:

	float getTorqueFromRPM(float rpm);

	float throttlePosition = 0.f;
	float currRPM = 0.f;
	float idleRPM;
	float differentialRatio;
	float transmissionEfficieny;
	int currentGear = 0;
	std::vector<float> gears;
	std::vector<std::pair<float, float>> RPMtoTorqueCurve;
	std::vector<Wheel*> wheelsAttached;
};

