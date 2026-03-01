
// dummy methods for simables

#include "ISimable.h"
#include <cstdio>

void ISimable::OnCollide(ISimable* other, const tVector3 &contactPoint)
{
}

void ISimable::Render(vView* view)
{
}

void ISimable::AddFuel(float amount)
{
}


ISimable::~ISimable()
{
}
