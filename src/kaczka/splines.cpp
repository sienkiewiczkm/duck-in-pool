#include "splines.hpp"
#include <cmath>
#include <iostream>

using namespace std;

vector<float> buildEquidistantKnotVector(int numControlPoints, 
    int degree) {
  auto intervals = numControlPoints + degree;
  vector<float> knots(intervals + 1);
  for (auto i = 0; i < intervals + 1; ++i) {
    knots[i] = ((float)i)/intervals;
  }
  return knots;
}

float bsplineBasis(int i, int degree, const vector<float> &knots, 
    float t) {
  if (degree == 0) {
    if (knots[i] <= t && t < knots[i+1])
      return 1.0f;
    return 0.0f;
  }

  float left = 0.0f, right = 0.0f;
  const float zeroEpsilon = 0.00001f;

  float leftDenominator = knots[i+degree] - knots[i];
  if (fabs(leftDenominator) > zeroEpsilon) {
    float leftCoefficient = (t - knots[i]) / leftDenominator;
    left = bsplineBasis(i, degree - 1, knots, t) * leftCoefficient;
  }

  float rightDenominator = knots[i+degree+1] - knots[i+1];
  if (fabs(rightDenominator) > zeroEpsilon) {
    float rightCoefficient = (knots[i+degree+1] - t) / rightDenominator;
    right = bsplineBasis(i + 1, degree - 1, knots, t) * rightCoefficient;
  }

  return left + right;
}

BSpline2D::BSpline2D() : _degree(3) {
}

BSpline2D::~BSpline2D() {
}

void BSpline2D::setControlPoints(const vector<glm::vec2> &controlPoints) {
  assert(controlPoints.size() >= 4);
  _controlPoints = controlPoints;
  _knots = buildEquidistantKnotVector(controlPoints.size(), 3);
  calculateDerivativeControlPoints();
}

void BSpline2D::setLoopedControlPoints(const vector<glm::vec2> &controlPoints) {
  assert(controlPoints.size() >= 3);
  vector<glm::vec2> loopedControlPoints(controlPoints);
  for (auto i = 0; i < 3; ++i) {
    loopedControlPoints.push_back(loopedControlPoints[i]);
  }
  setControlPoints(loopedControlPoints);
}

glm::vec2 BSpline2D::evaluate(float t) {
  t = nonVanishingIntervalCorrection(t);
  glm::vec2 evaluated(0.0f, 0.0f);
  for (auto i = 0; i < _controlPoints.size(); ++i) {
    float baseFunctionWeight = bsplineBasis(i, _degree, _knots, t);
    evaluated += _controlPoints[i] * baseFunctionWeight;
  }
  return evaluated;
}

glm::vec2 BSpline2D::derivative(float t) {
  t = nonVanishingIntervalCorrection(t);
  glm::vec2 evaluated(0.0f, 0.0f);
  for (auto i = 0; i < _derivativeControlPoints.size(); ++i) {
    float baseFunctionWeight = bsplineBasis(i + 1, _degree - 1, _knots, t);
    evaluated += _derivativeControlPoints[i] * baseFunctionWeight;
  }
  return evaluated;
}

void BSpline2D::calculateDerivativeControlPoints() {
  _derivativeControlPoints.clear();
  for (auto i = 0; i < _controlPoints.size() - 1; ++i) {
    float factor = _degree / (_knots[i+_degree+1] - _knots[i+1]);
    auto adjustedControlPoint = factor * 
      (_controlPoints[i+1] - _controlPoints[i]);
    _derivativeControlPoints.push_back(adjustedControlPoint);
  }
}

float BSpline2D::nonVanishingIntervalCorrection(float t) {
  auto m = _degree + _controlPoints.size();
  auto fixedIntervalLength = _knots[_controlPoints.size()] - _knots[_degree];
  return (double)_degree/m + t*fixedIntervalLength;
}
