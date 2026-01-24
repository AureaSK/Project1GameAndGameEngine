#pragma once
#include "CComponent.h"
class HPComponent : public CComponent
{
private:
	float maxHP;
	float currentHP;
	float valueToChange;

public:
	HPComponent(float setMaxHp);

	virtual ~HPComponent();

	void ChangeHP(float valueToChange);
	float GetCurrentHP() { return currentHP;};
	float GetMaxHP() { return maxHP; };
};

