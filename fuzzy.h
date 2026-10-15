#ifndef FUZZY_H
#define FUZZY_H

/* Generic Mamdani fuzzy inference engine.
 *
 * This header declares the mathematical building blocks shared by all
 * inference stages: fuzzy set shapes (triangular/trapezoidal membership
 * functions), the fuzzy AND/OR operators, and centroid defuzzification,
 * which converts a fuzzy output set back into a single crisp number.
 * It contains no project-specific numbers; those live in params.h. */

/* Marks a rule antecedent that does not apply, e.g. a rule that depends
 * only on market value ignores the location input. */
#define RULE_ANY -1

/* Membership function shape tag. */
typedef enum {
  MF_TRI,  /* Triangular shape: rises to a peak, then falls. */
  MF_TRAP  /* Trapezoidal shape: rises, holds a flat top, then falls. */
} MfType;

/* Triangular membership function.
 * a: input where membership starts rising from 0.
 * b: input where membership reaches 1 (the peak).
 * c: input where membership has fallen back to 0. */
typedef struct {
  float a;
  float b;
  float c;
} Tri;

/* Trapezoidal membership function.
 * a: input where membership starts rising from 0.
 * b: input where membership reaches 1.
 * c: input where membership starts falling from 1.
 * d: input where membership has fallen back to 0.
 * A vertical edge is written by repeating a corner (a==b or c==d),
 * which models a shoulder that starts or ends at full membership. */
typedef struct {
  float a;
  float b;
  float c;
  float d;
} Trap;

/* A single fuzzy set. The 'type' field selects which member of the
 * 'data' union holds valid parameters: exactly one of them is used. */
typedef struct {
  MfType type;
  union {
    Tri tri;
    Trap trap;
  } data;
} MF;

/* Degree of membership of x in the triangle (a,b,c). Returns 0 outside
 * [a,c], 1 at the peak b, linear in between. */
float trimf(float x, float a, float b, float c);

/* Degree of membership of x in the trapezoid (a,b,c,d). Returns 0
 * outside [a,d], 1 on the flat top [b,c], linear on the slopes. */
float trapmf(float x, float a, float b, float c, float d);

/* Degree of membership of x in mf, dispatching on mf.type. */
float mf_eval(float x, MF mf);

/* Fuzzy AND: degree to which both a and b hold. */
float fuzzy_min(float a, float b);

/* Fuzzy OR: degree to which at least one of a and b holds. */
float fuzzy_max(float a, float b);

/* Centroid (center of gravity) of the sampled output set.
 * xs: sample inputs, mus: membership degree at each sample, n: count.
 * Returns sum(mu*x)/sum(mu). If no rule fired (all mus are 0),
 * returns the middle of the sampled universe as a neutral fallback. */
float centroid(const float *xs, const float *mus, int n);

/* Restricts x to [lo,hi]. Keeps out-of-range test inputs from
 * silently producing an empty (all-zero) fuzzy result. */
float clampf(float x, float lo, float hi);

#endif /* FUZZY_H */
