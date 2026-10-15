#include "fuzzy.h"
#include "model.h"
#include "params.h"

#include <assert.h>

/* Linguistic terms of every variable. Each enum both names the terms in
 * the project manual and indexes its membership-function table below, so
 * a table entry can never drift out of sync with its term. The trailing
 * *_TERM_COUNT gives the table size. In rule tables, first_idx means
 * location (house rules) or assets (applicant rules), and second_idx means
 * market value (house rules) or income (applicant rules); RULE_ANY marks
 * a rule with only one condition, where the other input is ignored. */
typedef enum {
  MARKET_LOW,
  MARKET_MEDIUM,
  MARKET_HIGH,
  MARKET_VERY_HIGH,
  MARKET_TERM_COUNT
} MarketTerm;

typedef enum {
  LOC_BAD,
  LOC_FAIR,
  LOC_EXCELLENT,
  LOC_TERM_COUNT
} LocTerm;

typedef enum {
  HOUSE_VERY_LOW,
  HOUSE_LOW,
  HOUSE_MEDIUM,
  HOUSE_HIGH,
  HOUSE_VERY_HIGH,
  HOUSE_TERM_COUNT
} HouseTerm;

typedef enum {
  ASSET_LOW,
  ASSET_MEDIUM,
  ASSET_HIGH,
  ASSET_TERM_COUNT
} AssetTerm;

typedef enum {
  INCOME_LOW,
  INCOME_MEDIUM,
  INCOME_HIGH,
  INCOME_VERY_HIGH,
  INCOME_TERM_COUNT
} IncomeTerm;

typedef enum {
  INTEREST_LOW,
  INTEREST_MEDIUM,
  INTEREST_HIGH,
  INTEREST_TERM_COUNT
} InterestTerm;

typedef enum {
  APPL_LOW,
  APPL_MEDIUM,
  APPL_HIGH,
  APPL_TERM_COUNT
} ApplTerm;

typedef enum {
  CREDIT_VERY_LOW,
  CREDIT_LOW,
  CREDIT_MEDIUM,
  CREDIT_HIGH,
  CREDIT_VERY_HIGH,
  CREDIT_TERM_COUNT
} CreditTerm;

/* Fuzzified input shapes, one table per variable. Designated initializers
 * ([TERM] = ...) bind each entry to its enum value by name. */
static const MF MARKET_MFS[MARKET_TERM_COUNT] = {
  [MARKET_LOW] = {MF_TRAP,
                  {.trap = {MARKET_LOW_A, MARKET_LOW_B, MARKET_LOW_C,
                            MARKET_LOW_D}}},
  [MARKET_MEDIUM] = {MF_TRAP,
                     {.trap = {MARKET_MED_A, MARKET_MED_B, MARKET_MED_C,
                               MARKET_MED_D}}},
  [MARKET_HIGH] = {MF_TRAP,
                   {.trap = {MARKET_HIGH_A, MARKET_HIGH_B, MARKET_HIGH_C,
                             MARKET_HIGH_D}}},
  [MARKET_VERY_HIGH] = {MF_TRAP,
                        {.trap = {MARKET_VHIGH_A, MARKET_VHIGH_B,
                                  MARKET_VHIGH_C, MARKET_VHIGH_D}}},
};

static const MF LOC_MFS[LOC_TERM_COUNT] = {
  [LOC_BAD] = {MF_TRAP,
               {.trap = {LOC_BAD_A, LOC_BAD_B, LOC_BAD_C, LOC_BAD_D}}},
  [LOC_FAIR] = {MF_TRAP,
                {.trap = {LOC_FAIR_A, LOC_FAIR_B, LOC_FAIR_C, LOC_FAIR_D}}},
  [LOC_EXCELLENT] = {MF_TRAP,
                     {.trap = {LOC_EXC_A, LOC_EXC_B, LOC_EXC_C, LOC_EXC_D}}},
};

static const MF ASSET_MFS[ASSET_TERM_COUNT] = {
  [ASSET_LOW] = {MF_TRI, {.tri = {ASSET_LOW_A, ASSET_LOW_B, ASSET_LOW_C}}},
  [ASSET_MEDIUM] = {MF_TRAP,
                    {.trap = {ASSET_MED_A, ASSET_MED_B, ASSET_MED_C,
                              ASSET_MED_D}}},
  [ASSET_HIGH] = {MF_TRAP,
                  {.trap = {ASSET_HIGH_A, ASSET_HIGH_B, ASSET_HIGH_C,
                            ASSET_HIGH_D}}},
};

static const MF INC_MFS[INCOME_TERM_COUNT] = {
  [INCOME_LOW] = {MF_TRAP,
                  {.trap = {INC_LOW_A, INC_LOW_B, INC_LOW_C, INC_LOW_D}}},
  [INCOME_MEDIUM] = {MF_TRI, {.tri = {INC_MED_A, INC_MED_B, INC_MED_C}}},
  [INCOME_HIGH] = {MF_TRI, {.tri = {INC_HIGH_A, INC_HIGH_B, INC_HIGH_C}}},
  [INCOME_VERY_HIGH] = {MF_TRAP,
                        {.trap = {INC_VHIGH_A, INC_VHIGH_B, INC_VHIGH_C,
                                  INC_VHIGH_D}}},
};

static const MF INTEREST_MFS[INTEREST_TERM_COUNT] = {
  [INTEREST_LOW] = {MF_TRAP,
                    {.trap = {INTEREST_LOW_A, INTEREST_LOW_B, INTEREST_LOW_C,
                              INTEREST_LOW_D}}},
  [INTEREST_MEDIUM] = {MF_TRAP,
                       {.trap = {INTEREST_MED_A, INTEREST_MED_B,
                                 INTEREST_MED_C, INTEREST_MED_D}}},
  [INTEREST_HIGH] = {MF_TRAP,
                     {.trap = {INTEREST_HIGH_A, INTEREST_HIGH_B,
                               INTEREST_HIGH_C, INTEREST_HIGH_D}}},
};

/* Output shapes that Mamdani inference clips and merges. */
static const MF HOUSE_OUT_MFS[HOUSE_TERM_COUNT] = {
  [HOUSE_VERY_LOW] = {MF_TRI,
                      {.tri = {HOUSE_VLOW_A, HOUSE_VLOW_B, HOUSE_VLOW_C}}},
  [HOUSE_LOW] = {MF_TRI, {.tri = {HOUSE_LOW_A, HOUSE_LOW_B, HOUSE_LOW_C}}},
  [HOUSE_MEDIUM] = {MF_TRI,
                    {.tri = {HOUSE_MED_A, HOUSE_MED_B, HOUSE_MED_C}}},
  [HOUSE_HIGH] = {MF_TRI,
                  {.tri = {HOUSE_HIGH_A, HOUSE_HIGH_B, HOUSE_HIGH_C}}},
  [HOUSE_VERY_HIGH] = {MF_TRI,
                       {.tri = {HOUSE_VHIGH_A, HOUSE_VHIGH_B, HOUSE_VHIGH_C}}},
};

static const MF APPL_OUT_MFS[APPL_TERM_COUNT] = {
  [APPL_LOW] = {MF_TRAP,
                {.trap = {APPL_LOW_A, APPL_LOW_B, APPL_LOW_C, APPL_LOW_D}}},
  [APPL_MEDIUM] = {MF_TRI, {.tri = {APPL_MED_A, APPL_MED_B, APPL_MED_C}}},
  [APPL_HIGH] = {MF_TRAP,
                 {.trap = {APPL_HIGH_A, APPL_HIGH_B, APPL_HIGH_C,
                           APPL_HIGH_D}}},
};

static const MF CREDIT_OUT_MFS[CREDIT_TERM_COUNT] = {
  [CREDIT_VERY_LOW] = {MF_TRI,
                       {.tri = {CREDIT_VLOW_A, CREDIT_VLOW_B, CREDIT_VLOW_C}}},
  [CREDIT_LOW] = {MF_TRI,
                  {.tri = {CREDIT_LOW_A, CREDIT_LOW_B, CREDIT_LOW_C}}},
  [CREDIT_MEDIUM] = {MF_TRI,
                     {.tri = {CREDIT_MED_A, CREDIT_MED_B, CREDIT_MED_C}}},
  [CREDIT_HIGH] = {MF_TRI,
                   {.tri = {CREDIT_HIGH_A, CREDIT_HIGH_B, CREDIT_HIGH_C}}},
  [CREDIT_VERY_HIGH] = {MF_TRI,
                        {.tri = {CREDIT_VHIGH_A, CREDIT_VHIGH_B,
                                 CREDIT_VHIGH_C}}},
};

/* One if-then rule with two (possibly optional) conditions and one
 * conclusion, e.g. "if location is Bad and market value is Low then the
 * house evaluation is Very_low". A rule with a single condition stores
 * RULE_ANY in the unused field. */
typedef struct {
  int first_idx;
  int second_idx;
  int out_idx;
} PairRule;

/* The 14 house rules from the project manual, in manual order. */
static const PairRule HOUSE_RULES[] = {
  {RULE_ANY, MARKET_LOW, HOUSE_LOW}, /* 1: market Low -> House Low */
  {LOC_BAD, RULE_ANY, HOUSE_LOW},    /* 2: loc Bad -> House Low */
  {LOC_BAD, MARKET_LOW, HOUSE_VERY_LOW},      /* 3: Bad + Low -> Very_low */
  {LOC_BAD, MARKET_MEDIUM, HOUSE_LOW},        /* 4: Bad + Medium -> Low */
  {LOC_BAD, MARKET_HIGH, HOUSE_MEDIUM},       /* 5: Bad + High -> Medium */
  {LOC_BAD, MARKET_VERY_HIGH, HOUSE_HIGH},    /* 6: Bad + Very_high -> High */
  {LOC_FAIR, MARKET_LOW, HOUSE_LOW},          /* 7: Fair + Low -> Low */
  {LOC_FAIR, MARKET_MEDIUM, HOUSE_MEDIUM},    /* 8: Fair + Medium -> Medium */
  {LOC_FAIR, MARKET_HIGH, HOUSE_HIGH},        /* 9: Fair + High -> High */
  {LOC_FAIR, MARKET_VERY_HIGH, HOUSE_VERY_HIGH}, /* 10: Fair + Very_high -> Very_high */
  {LOC_EXCELLENT, MARKET_LOW, HOUSE_MEDIUM}, /* 11: Excellent + Low -> Medium */
  {LOC_EXCELLENT, MARKET_MEDIUM, HOUSE_HIGH}, /* 12: Excellent + Medium -> High */
  {LOC_EXCELLENT, MARKET_HIGH, HOUSE_VERY_HIGH}, /* 13: Excellent + High -> Very_high */
  {LOC_EXCELLENT, MARKET_VERY_HIGH, HOUSE_VERY_HIGH}, /* 14: Excellent + Very_high -> Very_high */
};

/* The 12 applicant rules from the project manual: every combination of
 * assets (Low/Medium/High) with income (Low/Medium/High/Very_high). */
static const PairRule APPL_RULES[] = {
  {ASSET_LOW, INCOME_LOW, APPL_LOW},       /* Low + Low -> Low */
  {ASSET_LOW, INCOME_MEDIUM, APPL_LOW},    /* Low + Medium -> Low */
  {ASSET_LOW, INCOME_HIGH, APPL_MEDIUM},   /* Low + High -> Medium */
  {ASSET_LOW, INCOME_VERY_HIGH, APPL_HIGH}, /* Low + Very_high -> High */
  {ASSET_MEDIUM, INCOME_LOW, APPL_LOW},    /* Medium + Low -> Low */
  {ASSET_MEDIUM, INCOME_MEDIUM, APPL_MEDIUM}, /* Medium + Medium -> Medium */
  {ASSET_MEDIUM, INCOME_HIGH, APPL_HIGH}, /* Medium + High -> High */
  {ASSET_MEDIUM, INCOME_VERY_HIGH, APPL_HIGH}, /* Medium + Very_high -> High */
  {ASSET_HIGH, INCOME_LOW, APPL_MEDIUM},   /* High + Low -> Medium */
  {ASSET_HIGH, INCOME_MEDIUM, APPL_MEDIUM}, /* High + Medium -> Medium */
  {ASSET_HIGH, INCOME_HIGH, APPL_HIGH},    /* High + High -> High */
  {ASSET_HIGH, INCOME_VERY_HIGH, APPL_HIGH}, /* High + Very_high -> High */
};

/* Fuzzification: converts one crisp input into a membership degree per
 * term by evaluating every shape in the table. */
static void fuzzify_all(float x, const MF *table, float *mus, int n) {
  int i;
  for (i = 0; i < n; i++) {
    mus[i] = mf_eval(x, table[i]);
  }
}

/* Rule evaluation: computes how strongly each rule fires (AND of its
 * conditions via fuzzy_min, or the single condition for RULE_ANY rules)
 * and merges rules that share a conclusion with fuzzy_max, yielding one
 * strength per output term in str[]. */
static void reduce_pair_rules(const float *mu_first, const float *mu_second,
                              const PairRule *rules, int n_rules, float *str,
                              int n_out) {
  int i;
  for (i = 0; i < n_out; i++) {
    str[i] = 0.0f;
  }
  for (i = 0; i < n_rules; i++) {
    float strength;
    int first = rules[i].first_idx;
    int second = rules[i].second_idx;
    assert(!(first == RULE_ANY && second == RULE_ANY));
    if (first == RULE_ANY) {
      strength = mu_second[second];
    } else if (second == RULE_ANY) {
      strength = mu_first[first];
    } else {
      strength = fuzzy_min(mu_first[first], mu_second[second]);
    }
    str[rules[i].out_idx] = fuzzy_max(str[rules[i].out_idx], strength);
  }
}

/* Mamdani aggregation and defuzzification, shared by all three stages.
 * Each output shape is clipped at its rule strength (fuzzy_min), the
 * clipped shapes are merged by taking their pointwise maximum
 * (fuzzy_max), and the resulting set is reduced to a crisp value with
 * the centroid of N_SAMPLES evenly spaced points. */
static float aggregate_and_defuzz(const float *str, const MF *out_table,
                                  int n_out, float out_min, float out_max) {
  float xs[N_SAMPLES];
  float mus[N_SAMPLES];
  int i;
  int k;

  for (i = 0; i < N_SAMPLES; i++) {
    float x = out_min + (out_max - out_min) * (float)i / (float)(N_SAMPLES - 1);
    float m = 0.0f;
    xs[i] = x;
    for (k = 0; k < n_out; k++) {
      float clipped = fuzzy_min(str[k], mf_eval(x, out_table[k]));
      m = fuzzy_max(m, clipped);
    }
    mus[i] = m;
  }
  return centroid(xs, mus, N_SAMPLES);
}

/* Stage 1: scores the property from its market value and location. */
float infer_house(float market, float loc) {
  float mu_market[MARKET_TERM_COUNT];
  float mu_loc[LOC_TERM_COUNT];
  float str[HOUSE_TERM_COUNT];

  market = clampf(market, MARKET_MIN, MARKET_MAX);
  loc = clampf(loc, LOC_MIN, LOC_MAX);

  fuzzify_all(market, MARKET_MFS, mu_market, MARKET_TERM_COUNT);
  fuzzify_all(loc, LOC_MFS, mu_loc, LOC_TERM_COUNT);
  reduce_pair_rules(mu_loc, mu_market, HOUSE_RULES, ARRAY_LEN(HOUSE_RULES), str,
                    HOUSE_TERM_COUNT);

  return aggregate_and_defuzz(str, HOUSE_OUT_MFS, HOUSE_TERM_COUNT, HOUSE_MIN,
                              HOUSE_MAX);
}

/* Stage 2: scores the applicant from assets and income. */
float infer_applicant(float asset, float income) {
  float mu_asset[ASSET_TERM_COUNT];
  float mu_inc[INCOME_TERM_COUNT];
  float str[APPL_TERM_COUNT];

  asset = clampf(asset, ASSET_MIN, ASSET_MAX);
  income = clampf(income, INC_MIN, INC_MAX);

  fuzzify_all(asset, ASSET_MFS, mu_asset, ASSET_TERM_COUNT);
  fuzzify_all(income, INC_MFS, mu_inc, INCOME_TERM_COUNT);
  reduce_pair_rules(mu_asset, mu_inc, APPL_RULES, ARRAY_LEN(APPL_RULES), str,
                    APPL_TERM_COUNT);

  return aggregate_and_defuzz(str, APPL_OUT_MFS, APPL_TERM_COUNT, APPL_MIN,
                              APPL_MAX);
}

/* Stage 3: sets the loan amount from the house and applicant scores plus
 * income and interest. Its 15 rules mix different input pairs (plus two
 * single-condition safety rules that force Very_low when the applicant or
 * the house is weakest), so they are evaluated explicitly rather than
 * through the shared pair-rule table. */
float infer_credit(float house, float applicant, float income, float interest) {
  float mu_house[HOUSE_TERM_COUNT];
  float mu_appl[APPL_TERM_COUNT];
  float mu_inc[INCOME_TERM_COUNT];
  float mu_int[INTEREST_TERM_COUNT];
  float str[CREDIT_TERM_COUNT];
  int i;

  house = clampf(house, HOUSE_MIN, HOUSE_MAX);
  applicant = clampf(applicant, APPL_MIN, APPL_MAX);
  income = clampf(income, INC_MIN, INC_MAX);
  interest = clampf(interest, INTEREST_MIN, INTEREST_MAX);

  fuzzify_all(house, HOUSE_OUT_MFS, mu_house, HOUSE_TERM_COUNT);
  fuzzify_all(applicant, APPL_OUT_MFS, mu_appl, APPL_TERM_COUNT);
  fuzzify_all(income, INC_MFS, mu_inc, INCOME_TERM_COUNT);
  fuzzify_all(interest, INTEREST_MFS, mu_int, INTEREST_TERM_COUNT);

  for (i = 0; i < CREDIT_TERM_COUNT; i++) {
    str[i] = 0.0f;
  }

  /* 1: Income Low + Interest Medium -> Very_low */
  str[CREDIT_VERY_LOW] = fuzzy_max(
    str[CREDIT_VERY_LOW],
    fuzzy_min(mu_inc[INCOME_LOW], mu_int[INTEREST_MEDIUM]));

  /* 2: Income Low + Interest High -> Very_low */
  str[CREDIT_VERY_LOW] = fuzzy_max(
    str[CREDIT_VERY_LOW],
    fuzzy_min(mu_inc[INCOME_LOW], mu_int[INTEREST_HIGH]));

  /* 3: Income Medium + Interest High -> Low */
  str[CREDIT_LOW] = fuzzy_max(
    str[CREDIT_LOW],
    fuzzy_min(mu_inc[INCOME_MEDIUM], mu_int[INTEREST_HIGH]));

  /* 4: Applicant Low -> Very_low */
  str[CREDIT_VERY_LOW] = fuzzy_max(str[CREDIT_VERY_LOW], mu_appl[APPL_LOW]);

  /* 5: House Very_low -> Very_low */
  str[CREDIT_VERY_LOW] =
    fuzzy_max(str[CREDIT_VERY_LOW], mu_house[HOUSE_VERY_LOW]);

  /* 6: Applicant Medium + House Very_low -> Low */
  str[CREDIT_LOW] = fuzzy_max(
    str[CREDIT_LOW], fuzzy_min(mu_appl[APPL_MEDIUM], mu_house[HOUSE_VERY_LOW]));

  /* 7: Applicant Medium + House Low -> Low */
  str[CREDIT_LOW] = fuzzy_max(
    str[CREDIT_LOW], fuzzy_min(mu_appl[APPL_MEDIUM], mu_house[HOUSE_LOW]));

  /* 8: Applicant Medium + House Medium -> Medium */
  str[CREDIT_MEDIUM] = fuzzy_max(
    str[CREDIT_MEDIUM],
    fuzzy_min(mu_appl[APPL_MEDIUM], mu_house[HOUSE_MEDIUM]));

  /* 9: Applicant Medium + House High -> High */
  str[CREDIT_HIGH] = fuzzy_max(
    str[CREDIT_HIGH], fuzzy_min(mu_appl[APPL_MEDIUM], mu_house[HOUSE_HIGH]));

  /* 10: Applicant Medium + House Very_high -> High */
  str[CREDIT_HIGH] = fuzzy_max(
    str[CREDIT_HIGH],
    fuzzy_min(mu_appl[APPL_MEDIUM], mu_house[HOUSE_VERY_HIGH]));

  /* 11: Applicant High + House Very_low -> Low */
  str[CREDIT_LOW] = fuzzy_max(
    str[CREDIT_LOW], fuzzy_min(mu_appl[APPL_HIGH], mu_house[HOUSE_VERY_LOW]));

  /* 12: Applicant High + House Low -> Medium */
  str[CREDIT_MEDIUM] = fuzzy_max(
    str[CREDIT_MEDIUM], fuzzy_min(mu_appl[APPL_HIGH], mu_house[HOUSE_LOW]));

  /* 13: Applicant High + House Medium -> High */
  str[CREDIT_HIGH] = fuzzy_max(
    str[CREDIT_HIGH], fuzzy_min(mu_appl[APPL_HIGH], mu_house[HOUSE_MEDIUM]));

  /* 14: Applicant High + House High -> High */
  str[CREDIT_HIGH] = fuzzy_max(
    str[CREDIT_HIGH], fuzzy_min(mu_appl[APPL_HIGH], mu_house[HOUSE_HIGH]));

  /* 15: Applicant High + House Very_high -> Very_high */
  str[CREDIT_VERY_HIGH] = fuzzy_max(
    str[CREDIT_VERY_HIGH],
    fuzzy_min(mu_appl[APPL_HIGH], mu_house[HOUSE_VERY_HIGH]));

  return aggregate_and_defuzz(str, CREDIT_OUT_MFS, CREDIT_TERM_COUNT,
                              CREDIT_MIN, CREDIT_MAX);
}
