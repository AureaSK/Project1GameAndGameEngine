#include "HPComponent.h"

HPComponent::HPComponent(float setMaxHp): CComponent(owner), maxHP(setMaxHp)
, currentHP(setMaxHp), valueToChange(0.0f)
{
}

HPComponent::~HPComponent()
{
}

void HPComponent::ChangeHP(float valueToChange)
{
	currentHP += (valueToChange);
}
