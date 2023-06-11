#pragma once
#ifndef __HIT_RECORD_HPP__
#define __HIT_RECORD_HPP__

#include <optional>

#include "geometry/vec.hpp"

namespace Metropolis
{
    using namespace NRenderer;
    using namespace std;
    struct HitRecordBase
    {
        float t;
        Vec3 hitPoint;
        Vec3 normal;
        Handle material;
        int id; // 与哪个面片相交
    };
    using HitRecord = optional<HitRecordBase>;
    inline
    HitRecord getMissRecord() {
        return nullopt;
    }

    inline
    HitRecord getHitRecord(float t, const Vec3& hitPoint, const Vec3& normal, Handle material, int id) {
        return make_optional<HitRecordBase>(t, hitPoint, normal, material, id);
    }
}

#endif