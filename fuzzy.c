#include "fuzzy.h"

/* Fuzzy AND: the weaker of the two claims limits the result. */
float fuzzy_min(float a, float b) {
  return (a < b) ? a : b;
}

/* Fuzzy OR: the stronger of the two claims wins. */
float fuzzy_max(float a, float b) {
  return (a > b) ? a : b;
}

/* Clamps x into [lo,hi]; values inside pass through unchanged. */
float clampf(float x, float lo, float hi) {
  if (x < lo)
    return lo;
  if (x > hi)
    return hi;
  return x;
}

/* A triangle is a trapezoid whose flat top has zero width (b==c),
 * so triangular evaluation reuses the trapezoidal formula. */
float trimf(float x, float a, float b, float c) {
  return trapmf(x, a, b, b, c);
}

/* Piecewise-linear membership curve. Because the bounds test uses strict
 * comparison, a vertical edge (a==b or c==d) needs no special case:
 * inputs on the edge fall through to the plateau and return 1. */
float trapmf(float x, float a, float b, float c, float d) {
  if (x < a || x > d)
    return 0.0f;
  if (x < b)
    return (x - a) / (b - a);
  if (x <= c)
    return 1.0f;
  return (d - x) / (d - c);
}

/* Evaluates the stored shape according to the MF's type tag. */
float mf_eval(float x, MF mf) {
  switch (mf.type) {
  case MF_TRI:
    return trimf(x, mf.data.tri.a, mf.data.tri.b, mf.data.tri.c);
  case MF_TRAP:
    return trapmf(x, mf.data.trap.a, mf.data.trap.b, mf.data.trap.c,
                  mf.data.trap.d);
  default:
    return 0.0f;
  }
}

/* Discrete centroid: the weighted average of the sample points, using
 * each point's membership degree as its weight. Division by zero is
 * guarded: an empty result set yields the universe midpoint. */
float centroid(const float *xs, const float *mus, int n) {
  float num = 0.0f;
  float den = 0.0f;
  int i;

  for (i = 0; i < n; i++) {
    num += xs[i] * mus[i];
    den += mus[i];
  }
  if (den == 0.0f) {
    if (n > 0)
      return (xs[0] + xs[n - 1]) * 0.5f;
    return 0.0f;
  }
  return num / den;
}
