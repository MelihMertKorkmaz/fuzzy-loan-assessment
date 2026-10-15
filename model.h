#ifndef MODEL_H
#define MODEL_H

/* Number of elements in a statically sized array. */
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

/* Public interface of the loan assessment model.
 *
 * The assessment runs in three hierarchical Mamdani stages with crisp
 * handoff: the house and applicant stages each produce a 0-10 score,
 * and the credit stage consumes those scores (plus income and interest)
 * to produce the loan amount. All functions are pure: their outputs
 * depend only on their arguments. Numbers live in params.h.
 *
 * Units: market/asset x10^3 $, income x10^3 $, interest %, house/applicant
 * scores 0-10, credit x10^3 $. */

/* Stage 1: property score (0-10) from market value and location. */
float infer_house(float market, float loc);

/* Stage 2: applicant score (0-10) from assets and income. */
float infer_applicant(float asset, float income);

/* Stage 3: loan amount (x10^3 $) from the two stage scores plus the
 * applicant's income and the offered interest rate. */
float infer_credit(float house, float applicant, float income, float interest);

#endif /* MODEL_H */
