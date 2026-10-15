#include "model.h"
#include <stdio.h>

/* One loan application to assess. Monetary values are in thousands of USD
 * (matching params.h); location is a 0-10 score, interest is a percent. */
typedef struct {
  const char *name;
  float market;
  float loc;
  float asset;
  float income;
  float interest;
} Scenario;

/* Runs the full three-stage assessment for one scenario and prints the
 * intermediate house/applicant scores together with the final loan. */
static void run_scenario(const Scenario *s) {
  /* Crisp handoff between stages: each stage's numeric score is fed back
   * in as the next stage's input. */
  float house = infer_house(s->market, s->loc);
  float applicant = infer_applicant(s->asset, s->income);
  float credit = infer_credit(house, applicant, s->income, s->interest);

  printf("%s\n", s->name);
  printf(
      "  in:  market=%.1fk loc=%.1f asset=%.1fk income=%.1fk interest=%.1f%%\n",
      s->market, s->loc, s->asset, s->income, s->interest);
  printf("  out: house=%.2f/10 applicant=%.2f/10 credit=%.1fk $ (%.0f $)\n\n",
         house, applicant, credit, credit * 1000.0f);
}

int main(void) {
  /* Five typical applications plus the one extreme corners (all
   *  maximum inputs) to show the system stays stable there. */
  static const Scenario cases[] = {
      {"1) Poor, cheap house, bad location", 80.0f, 1.0f, 20.0f, 12.0f, 7.5f},
      {"2) Rich, excellent location, big house", 800.0f, 9.0f, 800.0f, 90.0f,
       3.0f},
      {"3) Medium everything", 300.0f, 5.5f, 300.0f, 45.0f, 5.0f},
      {"4) High income but bad house + high interest", 120.0f, 2.0f, 600.0f,
       75.0f, 8.0f},
      {"5) Low income, good collateral, medium interest", 650.0f, 7.0f, 100.0f,
       20.0f, 4.5f},
      {"6) Absolute max", 1000.0f, 10.0f, 1000.0f, 100.0f, 10.0f},
  };

  printf("Fuzzy loan assessment (Mamdani, hierarchical, crisp handoff)\n\n");
  for (size_t i = 0; i < ARRAY_LEN(cases); i++) {
    run_scenario(&cases[i]);
  }
  return 0;
}
