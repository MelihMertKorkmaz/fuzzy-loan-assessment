#ifndef PARAMS_H
#define PARAMS_H

/* All project numbers in one place; no logic here.
 *
 * Each membership function is listed with its corner points on the input
 * axis (see fuzzy.h for the meaning of a/b/c/d). Approximate corners that
 * were read from the graphs in Project_1.pdf are marked as such.
 *
 * Units: market value, assets, income and credit are in thousands of USD;
 * interest rate is in percent; house and applicant scores run from 0 to 10. */

/* ---------- 1. Market value of the house (x10^3 $, 0-1000) ---------- */
#define MARKET_LOW_A 0.0f
#define MARKET_LOW_B 0.0f
#define MARKET_LOW_C 75.0f /* approx: read from graph, +-5 on 0-1000 scale */
#define MARKET_LOW_D 100.0f
#define MARKET_MED_A 50.0f /* approx: read from graph, +-5 on 0-1000 scale */
#define MARKET_MED_B 100.0f
#define MARKET_MED_C 200.0f
#define MARKET_MED_D 250.0f
#define MARKET_HIGH_A 200.0f
#define MARKET_HIGH_B 300.0f
#define MARKET_HIGH_C 650.0f
#define MARKET_HIGH_D 850.0f
#define MARKET_VHIGH_A 650.0f
#define MARKET_VHIGH_B 850.0f
#define MARKET_VHIGH_C 1000.0f
#define MARKET_VHIGH_D 1000.0f

/* ---------- 2. Location of the house (score 0-10) ---------- */
#define LOC_BAD_A 0.0f
#define LOC_BAD_B 0.0f
#define LOC_BAD_C 1.5f /* approx: read from graph, +-0.2 on 0-10 scale */
#define LOC_BAD_D 4.0f
#define LOC_FAIR_A 2.5f
#define LOC_FAIR_B 5.0f
#define LOC_FAIR_C 6.0f
#define LOC_FAIR_D 8.5f
#define LOC_EXC_A 6.0f
#define LOC_EXC_B 8.5f
#define LOC_EXC_C 10.0f
#define LOC_EXC_D 10.0f

/* ---------- 3. Applicant's assets (x10^3 $, 0-1000) ---------- */
#define ASSET_LOW_A 0.0f
#define ASSET_LOW_B 0.0f
#define ASSET_LOW_C 150.0f
#define ASSET_MED_A 50.0f
#define ASSET_MED_B 250.0f
#define ASSET_MED_C 450.0f /* approx: read from graph, +-5 on 0-1000 scale */
#define ASSET_MED_D 650.0f
#define ASSET_HIGH_A 500.0f
#define ASSET_HIGH_B 700.0f
#define ASSET_HIGH_C 1000.0f
#define ASSET_HIGH_D 1000.0f

/* ---------- 4. Applicant's income (x10^3 $, 0-100) ---------- */
#define INC_LOW_A 0.0f
#define INC_LOW_B 0.0f
#define INC_LOW_C 10.0f
#define INC_LOW_D 25.0f
#define INC_MED_A 15.0f
#define INC_MED_B 35.0f
#define INC_MED_C 55.0f
#define INC_HIGH_A 40.0f
#define INC_HIGH_B 60.0f
#define INC_HIGH_C 80.0f
#define INC_VHIGH_A 60.0f
#define INC_VHIGH_B 80.0f
#define INC_VHIGH_C 100.0f
#define INC_VHIGH_D 100.0f

/* ---------- 5. Interest rate (percent, 0-10) ---------- */
#define INTEREST_LOW_A 0.0f
#define INTEREST_LOW_B 0.0f
#define INTEREST_LOW_C 2.0f
#define INTEREST_LOW_D 5.0f
#define INTEREST_MED_A 2.0f
#define INTEREST_MED_B 4.0f
#define INTEREST_MED_C 6.0f
#define INTEREST_MED_D 8.0f
#define INTEREST_HIGH_A 6.0f
#define INTEREST_HIGH_B 8.5f
#define INTEREST_HIGH_C 10.0f
#define INTEREST_HIGH_D 10.0f

/* ---------- 6. House evaluation (score 0-10) ---------- */
#define HOUSE_VLOW_A 0.0f
#define HOUSE_VLOW_B 0.0f
#define HOUSE_VLOW_C 3.0f
#define HOUSE_LOW_A 0.0f
#define HOUSE_LOW_B 3.0f
#define HOUSE_LOW_C 6.0f
#define HOUSE_MED_A 2.0f
#define HOUSE_MED_B 5.0f
#define HOUSE_MED_C 8.0f
#define HOUSE_HIGH_A 4.0f
#define HOUSE_HIGH_B 7.0f
#define HOUSE_HIGH_C 10.0f
#define HOUSE_VHIGH_A 7.0f
#define HOUSE_VHIGH_B 10.0f
#define HOUSE_VHIGH_C 10.0f

/* ---------- 7. Applicant evaluation (score 0-10) ---------- */
#define APPL_LOW_A 0.0f
#define APPL_LOW_B 0.0f
#define APPL_LOW_C 2.0f
#define APPL_LOW_D 4.0f
#define APPL_MED_A 2.0f
#define APPL_MED_B 5.0f
#define APPL_MED_C 8.0f
#define APPL_HIGH_A 6.0f
#define APPL_HIGH_B 8.0f
#define APPL_HIGH_C 10.0f
#define APPL_HIGH_D 10.0f

/* ---------- 8. Loan credit amount (x10^3 $, 0-500) ---------- */
#define CREDIT_VLOW_A 0.0f
#define CREDIT_VLOW_B 0.0f
#define CREDIT_VLOW_C 125.0f
#define CREDIT_LOW_A 0.0f
#define CREDIT_LOW_B 125.0f
#define CREDIT_LOW_C 250.0f
#define CREDIT_MED_A 125.0f
#define CREDIT_MED_B 250.0f
#define CREDIT_MED_C 375.0f
#define CREDIT_HIGH_A 250.0f
#define CREDIT_HIGH_B 375.0f
#define CREDIT_HIGH_C 500.0f
#define CREDIT_VHIGH_A 375.0f
#define CREDIT_VHIGH_B 500.0f
#define CREDIT_VHIGH_C 500.0f

/* Valid input ranges. Inputs outside these bounds are clamped to them. */
#define MARKET_MIN 0.0f
#define MARKET_MAX 1000.0f
#define LOC_MIN 0.0f
#define LOC_MAX 10.0f
#define ASSET_MIN 0.0f
#define ASSET_MAX 1000.0f
#define INC_MIN 0.0f
#define INC_MAX 100.0f
#define INTEREST_MIN 0.0f
#define INTEREST_MAX 10.0f
#define HOUSE_MIN 0.0f
#define HOUSE_MAX 10.0f
#define APPL_MIN 0.0f
#define APPL_MAX 10.0f
#define CREDIT_MIN 0.0f
#define CREDIT_MAX 500.0f

/* Number of evenly spaced samples used to approximate the continuous
 * centroid integral. 1001 samples give a step of 0.01 on a 0-10 universe
 * and 0.5 (thousand USD) on the 0-500 credit universe. */
#define N_SAMPLES 1001

#endif /* PARAMS_H */
