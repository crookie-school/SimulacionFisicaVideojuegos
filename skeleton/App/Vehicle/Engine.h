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
	Engine(const EngineDesc& desc) :
		differentialRatio(desc.differentialRatio),
		transmissionEfficieny(desc.transmissionEfficieny),
		gears(desc.gears),
		RPMtoTorqueCurve(desc.RPMtoTorqueCurve)
	{
		std::sort(RPMtoTorqueCurve.begin(), RPMtoTorqueCurve.end());
	}

	void attachWheel(Wheel* w) { wheelsAttached.push_back(w); }
	void detachWheel(Wheel* w) { wheelsAttached.erase(std::find(wheelsAttached.begin(), wheelsAttached.end(), w)); }

	void setThrottlePosition(float amount) { throttlePosition = amount; } // 1-0
	float getThrottlePosition() const { return throttlePosition; }

	void changeGear(int gear)
	{
		assert(gear >= 0 || gear < gears.size());

		currentGear = gear;
	}

	void update(float dt)
	{
		float wheelRotRateAvg = 0.f; // Hago average pq no se me ocurre otra cosa, aparte de pillar cualquier rueda o la que mas gire o algo asi
		for (Wheel* w : wheelsAttached)
			wheelRotRateAvg += w->getWheelAngularVelocity();
		if(wheelsAttached.size() != 0) wheelRotRateAvg /= wheelsAttached.size();

		// Calculate current RPM from wheel rotation
		currRPM = wheelRotRateAvg * gears[currentGear] * differentialRatio * 60 / physx::PxTwoPi;

		// Calculate torque produced by engine
		float maxTorque = getTorqueFromRPM(std::max(currRPM, idleRPM));
		float engineTorque = throttlePosition * maxTorque;
		float driveTorque = engineTorque * gears[currentGear] * differentialRatio * transmissionEfficieny / wheelsAttached.size();

		// Apply torque to wheels attached
		for (Wheel* w : wheelsAttached)
		{
			w->applyTorque(driveTorque);
		}
	}

	void drawGui()
	{
		ImGui::PushID(this);

		ImGui::PlotLines("Torque curve", 
			[](void* data, int i) { 
				return static_cast<Engine*>(data)->getTorqueFromRPM(i); 
			}, 
			this, 
			(int)RPMtoTorqueCurve[RPMtoTorqueCurve.size() - 1].first,
			0,
			"",
			0, 600,
			{200, 100});

		ImGui::Text("RPM: %f", currRPM);
		ImGui::Text("Gear: %d", currentGear);
		if (ImGui::BeginTable("Gear table", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Gear R");
			ImGui::TableNextColumn();
			ImGui::Text("%f", gears[0]);

			for (int r = 1; r < gears.size(); ++r)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Gear %d", r);
				ImGui::TableNextColumn();
				ImGui::Text("%f", gears[r]);
			}

			ImGui::EndTable();
		}

		ImGui::PopID();
	}

private:

	float getTorqueFromRPM(float rpm)
	{
		if (rpm >= RPMtoTorqueCurve.back().first)  return 0.f;               // rev limiter
		if (rpm <= RPMtoTorqueCurve.front().first) return RPMtoTorqueCurve.front().second;

		auto it = std::lower_bound(RPMtoTorqueCurve.begin(), RPMtoTorqueCurve.end(), rpm, 
			[](const std::pair<float, float>& p, float v) { return p.first < v; }
		);

		float torqueO = (it-1)->second, rpmO = (it - 1)->first;
		float torqueF = (it)->second,   rpmF = (it)->first;
		return torqueO + (torqueF - torqueO) * (rpm - rpmO) / (rpmF - rpmO); // Interpola linealmente entre puntos
	}

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

