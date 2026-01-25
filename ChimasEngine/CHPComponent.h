#pragma once
#include "CComponent.h"
class CHPComponent : public CComponent
{
private:
	float maxHP;
	float currentHP;
	float valueToChange;

public:

	CHPComponent(CActor* owner);
	virtual ~CHPComponent();

	void ChangeHP(float value);
	float GetCurrentHP() const { return currentHP; }
	float GetMaxHP() const { return maxHP; }
	void SetMaxHP(float value);
};

