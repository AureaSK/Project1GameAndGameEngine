#include "pch.h"
#include "CHPComponent.h"

CHPComponent::CHPComponent(CActor* owner)
	: CComponent(owner), maxHP(100.0f), currentHP(100.0f), valueToChange(0.0f)
{
}

CHPComponent::~CHPComponent()
{
}

void CHPComponent::ChangeHP(float value)
{
	currentHP += value;
	if (currentHP > maxHP) currentHP = maxHP;
	if (currentHP < 0.0f) currentHP = 0.0f;
}

void CHPComponent::SetMaxHP(float value)
{
	maxHP = value;
	currentHP = value;
}