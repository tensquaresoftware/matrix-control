#pragma once

#include "SkinValues.h"

namespace tss
{
    class ISkinValues
    {
    public:
        virtual ~ISkinValues() = default;

        virtual float getValue(SkinValueId valueId) const = 0;
        virtual void setValue(SkinValueId valueId, float value) = 0;
    };
}
