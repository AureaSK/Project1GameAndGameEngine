#pragma once
#include "CComponent.h"
class HPComponent : public CComponent
{
private:
	

public:
	float maxHP;
	float currentHP;
	float valueToChange;

	HPComponent(CActor* owner, float setMaxHp) : CComponent(owner), maxHP(setMaxHp), currentHP(setMaxHp), valueToChange(0.0f) {}

	virtual ~HPComponent();

	void ChangeHP(float valueToChange);
	float GetCurrentHP() { return currentHP;};
	float GetMaxHP() { return maxHP; };
};

