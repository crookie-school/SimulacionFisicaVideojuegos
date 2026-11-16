#include "Engine.h"

Engine::Engine(const EngineDesc& desc) :
	differentialRatio(desc.differentialRatio),
	transmissionEfficieny(desc.transmissionEfficieny),
	gears(desc.gears),
	RPMtoTorqueCurve(desc.RPMtoTorqueCurve)
{
	std::sort(RPMtoTorqueCurve.begin(), RPMtoTorqueCurve.end());
}

void Engine::update(float dt)
{
	float wheelRotRateAvg = 0.f; // Hago average pq no se me ocurre otra cosa, aparte de pillar cualquier rueda o la que mas gire o algo asi
	for (Wheel* w : wheelsAttached)
		wheelRotRateAvg += w->getWheelAngularVelocity();
	if (wheelsAttached.size() != 0) wheelRotRateAvg /= wheelsAttached.size();

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

void Engine::drawGui()
{
	ImGui::PushID(this);

	// Un poco caro, pero bueno es para GUI, si afecta al rendimiento lo cacheare
	float min, max; min = max = RPMtoTorqueCurve[0].second;
	for (int i = 1; i < RPMtoTorqueCurve.size(); ++i)
	{
		if (RPMtoTorqueCurve[i].second < min) min = RPMtoTorqueCurve[i].second;
		if (RPMtoTorqueCurve[i].second > max) max = RPMtoTorqueCurve[i].second;
	}
	float currTorque = getTorqueFromRPM(currRPM);
	char overlayText[64];
	sprintf(overlayText, "Torque curve: %f", currTorque);
	ImGui::PlotLines("##TorqueCurve",
		[](void* data, int i)
		{
			return static_cast<Engine*>(data)->getTorqueFromRPM(i);
		},
		this,
		(int)RPMtoTorqueCurve[RPMtoTorqueCurve.size() - 1].first,
		0,
		overlayText,
		min, max,
		{ 400, 200 });

	ImGui::Text("RPM: %f", currRPM);
	ImGui::Text("Gear: %d", currentGear);
	if (ImGui::BeginTable("Gear table", 2))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (currentGear == 0) ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Gear R");
		else ImGui::Text("Gear R");
		ImGui::TableNextColumn();
		ImGui::Text("%f", gears[0]);

		for (int r = 1; r < gears.size(); ++r)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (currentGear == r) ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Gear %d", r);
			else ImGui::Text("Gear %d", r);
			ImGui::TableNextColumn();
			ImGui::Text("%f", gears[r]);
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
}

float Engine::getTorqueFromRPM(float rpm)
{
	if (rpm >= RPMtoTorqueCurve.back().first)  return 0.f;               // rev limiter
	if (rpm <= RPMtoTorqueCurve.front().first) return RPMtoTorqueCurve.front().second;

	auto it = std::lower_bound(RPMtoTorqueCurve.begin(), RPMtoTorqueCurve.end(), rpm,
		[](const std::pair<float, float>& p, float v) { return p.first < v; }
	);

	float torqueO = (it - 1)->second, rpmO = (it - 1)->first;
	float torqueF = (it)->second, rpmF = (it)->first;
	return torqueO + (torqueF - torqueO) * (rpm - rpmO) / (rpmF - rpmO); // Interpola linealmente entre puntos
}
