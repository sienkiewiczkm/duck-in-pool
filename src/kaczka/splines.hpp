#ifndef __SPLINES_HPP__
#define __SPLINES_HPP__

#include <vector>
#include <glm/glm.hpp>

std::vector<float> buildEquidistantKnotVector(int numControlPoints, int degree);
float bsplineBasis(int i, int degree, const std::vector<float> &knots, float t);

class BSpline2D {
public:
  BSpline2D();
  ~BSpline2D();

  void setControlPoints(const std::vector<glm::vec2> &controlPoints);
  void setLoopedControlPoints(const std::vector<glm::vec2> &controlPoints);
  glm::vec2 evaluate(float t);
  glm::vec2 derivative(float t);

protected:
  void calculateDerivativeControlPoints();
  float nonVanishingIntervalCorrection(float t);

private:
  int _degree;
  std::vector<glm::vec2> _controlPoints;
  std::vector<glm::vec2> _derivativeControlPoints;
  std::vector<float> _knots;
};

#endif
