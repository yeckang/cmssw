#ifndef Geom_TruncatedTrapezoidalPlaneBounds_H
#define Geom_TruncatedTrapezoidalPlaneBounds_H

#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometrySurface/interface/Bounds.h"
#include <array>

/** \class TruncatedTrapezoidalPlaneBounds
 *  TruncatedTrapezoidal plane bounds.
 *  Local Coordinate system coincides with center of the box
 *  with Y axis being the symmetry axis along the height
 *  and pointing in the direction of top_edge.
 */

class TruncatedTrapezoidalPlaneBounds /* final */ : public Bounds {
public:
  /** constructed from:
   *    half bottom edge (smaller side width)
   *    half top edge    (larger side width)
   *    half apothem (distance from top to bottom sides, 
   *                  measured perpendicularly to them)
   *    half thickness
   */
  TruncatedTrapezoidalPlaneBounds(float be, float te, float a, float t, float rte);

  /** apothem (full, not half)*/
  float length() const override { return 2 * hapothem; }

  /** largest width (full, not half)*/
  float width() const override { return 2 * std::max(hbotedge, htopedge); }

  /** thickness (full, not half)*/
  float thickness() const override { return 2 * hthickness; }

  /** Width at half length. Useful for e.g. pitch definition.
   */
  float widthAtHalfLength() const override { return hbotedge + htopedge; }

  virtual int yAxisOrientation() const;

  using Bounds::inside;

  bool inside(const Local2DPoint& p) const override;

  bool inside(const Local3DPoint& p) const override;

  bool inside(const Local3DPoint& p, const LocalError& err, float scale) const override;

  bool inside(const Local2DPoint& p, const LocalError& err, float scale) const override;

  float significanceInside(const Local3DPoint&, const LocalError&) const override;

  /** returns the 5 parameters needed for construction, in the order
   * ( half bottom edge, half top edge, half thickness, half apothem).
   * Beware! This order is different from the one in the constructor!
   */
  virtual const std::array<const float, 5> parameters() const;

  Bounds* clone() const override;

private:
  // persistent part
  float hbotedge;
  float htopedge;
  float hapothem;
  float hthickness;
  float hrealtopedge;

  // transient part
  float offset;
  float tan_a;
};

#endif  // Geom_TruncatedTrapezoidalPlaneBounds_H
