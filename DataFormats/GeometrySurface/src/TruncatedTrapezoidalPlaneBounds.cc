

#include "DataFormats/GeometrySurface/interface/TruncatedTrapezoidalPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/LocalError.h"
#include <cmath>

TruncatedTrapezoidalPlaneBounds::TruncatedTrapezoidalPlaneBounds(float be, float te, float a, float t, float rte)
    : hbotedge(be), htopedge(te), hapothem(a), hthickness(t), hrealtopedge(rte) {
  // pre-compute offset of triangle vertex and tg of (half) opening
  // angle of the trapezoid for faster inside() implementation.

  offset = a * (te + be) / (te - be);  // check sign if te < be !!!
  tan_a = te / std::abs(offset + a);
}

int TruncatedTrapezoidalPlaneBounds::yAxisOrientation() const { return (hbotedge > htopedge) ? -1 : 1; }

bool TruncatedTrapezoidalPlaneBounds::inside(const Local2DPoint& p) const {
  return (std::abs(p.y()) < hapothem) && (std::abs(p.x()) < hrealtopedge) && (std::abs(p.x()) < tan_a * std::abs(p.y() + offset));
}

bool TruncatedTrapezoidalPlaneBounds::inside(const Local3DPoint& p) const {
  return ((std::abs(p.y()) < hapothem) && (std::abs(p.z()) < hthickness)) &&
         (std::abs(p.x()) < hrealtopedge) && std::abs(p.x()) < tan_a * std::abs(p.y() + offset);
}

bool TruncatedTrapezoidalPlaneBounds::inside(const Local3DPoint& p, const LocalError& err, float scale) const {
  if (scale >= 0 && inside(p))
    return true;

  TruncatedTrapezoidalPlaneBounds tmp(hbotedge + std::sqrt(err.xx()) * scale,
                                   htopedge + std::sqrt(err.xx()) * scale,
                                   hapothem + std::sqrt(err.yy()) * scale,
                                   hthickness,
                                   hrealtopedge);
  return tmp.inside(p);
}

bool TruncatedTrapezoidalPlaneBounds::inside(const Local2DPoint& p, const LocalError& err, float scale) const {
  return Bounds::inside(p, err, scale);
}

float TruncatedTrapezoidalPlaneBounds::significanceInside(const Local3DPoint& p, const LocalError& err) const {
  return std::max((std::abs(p.y()) - hapothem) / std::sqrt(err.yy()),
                  (std::abs(p.x()) - tan_a * std::abs(p.y() + offset)) / std::sqrt(err.xx()));
}

Bounds* TruncatedTrapezoidalPlaneBounds::clone() const { return new TruncatedTrapezoidalPlaneBounds(*this); }

const std::array<const float, 5> TruncatedTrapezoidalPlaneBounds::parameters() const {
  // Same order as geant3 for constructor compatibility
  std::array<const float, 5> vec{{hbotedge, htopedge, hthickness, hapothem, hrealtopedge}};
  return vec;
}
