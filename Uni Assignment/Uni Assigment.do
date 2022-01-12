clear

** ssc install stddiff
* ssc install summtab
log using Appendix3.txt, text replace

cd "\\Client\C$\Users\georg\Downloads\"
import excel using en_audit.xls, firstrow

set seed 1511

** This sets up the basic data generation **
**<><><><><><><><>**
* This sets up the randomised data in the base year
gen expend0 = expendbase + 10*rnormal()
* It is reasonable to assume that the effect of area on expenditure is better described in percentage increases in area
* rather than linear terms. Each additional increase of are on expenditure should have decreasing impact
gen logarea = log(area)
* This sets up the DID regression
gen change_expend = expend - expend0
**<><><><><><><><>**


** This is the beginning of the data exploration**
**<><><><><><><><>**
* This clarifies the sample size of audit across districts
bysort treat district: summarize audit if audit == 1

* This creates a word file that contains the mean and standard deviations of all non-weather variables
summtab, by(treat) contvars(expend0 expend area pool aircon rc_aircon cheat dish mwave dryer ftype people inc hinc audit) mean landscape replace word wordname(summary_table5) title(Table 1: Balancing Treatment and Control Groups)

* This checks for imbalances within the audited households in the treatment and control group
preserve
drop if audit == 0
stddiff expend0 expend area pool aircon rc_aircon cheat dish mwave dryer ftype people inc hinc, by(treat)
restore

* This visualises the high degree of standard deviation in weather
stddiff expend0 expendbase expend area pool aircon rc_aircon cheat dish mwave dryer ftype people inc hinc audit max0_1 max0_2 max0_3 max0_4 max0_5 max0_6 max0_7 max0_8 max0_9 max0_10 max0_11 max0_12 max1_1 max1_2 max1_3 max1_4 max1_5 max1_6 max1_7 max1_8 max1_9 max1_10 max1_11 max1_12 min0_1 min0_2 min0_3 min0_4 min0_5 min0_6 min0_7 min0_8 min0_9 min0_10 min0_11 min0_12 min1_1 min1_2 min1_3 min1_4 min1_5 min1_6 min1_7 min1_8 min1_9 min1_10 min1_11 min1_12, by(district)

* Provides information of what is the main driver of changes in energy expenditure
bysort district: summarize change_expend
* These two graphs indicate that District 2 shifts their change in energy expenditure to the left
twoway kdensity change_expend if district == 1 & treat == 1 & change_expend < 400 || kdensity change_expend if district == 1 & treat == 0 & change_expend < 400, legend(order(1 "Treated" 2 "Control")) xtitle("Change in Energy Expenditure For District 1") ytitle("Density") graphregion(color(white)) bgcolor(white) 

twoway kdensity change_expend if district == 2 & treat == 1 & change_expend < 400|| kdensity change_expend if district == 2 & treat == 0 & change_expend < 400, legend(order(1 "Treated" 2 "Control")) xtitle("Change in Energy Expenditure For District 2") ytitle("Density") graphregion(color(white)) bgcolor(white)

* this is how the outlier amounts were determined
summarize area if treat == 1, detail
summarize area if treat == 0, detail

* This drops the three largest observations of area and finds checks that it is solely the
* three outliers that impact on such a large standard deviation
summarize area if treat == 1 & area < 4000, detail
**<><><><><><><><>**


** This is the beginning of the econometric modelling**
**<><><><><><><><>**
* These are the base controls. It includes all variables and interactions between income and household observable characteristics
global controls c.max0_1 c.max0_2 c.max0_3 c.max0_4 c.max0_5 c.max0_6 c.max0_7 c.max0_8 c.max0_9 c.max0_10 c.max0_11 c.max0_12 c.max1_1 c.max1_2 c.max1_3 c.max1_4 c.max1_5 c.max1_6 c.max1_7 c.max1_8 c.max1_9 c.max1_10 c.max1_11 c.max1_12 i.inc##(i.pool i.aircon i.rc_aircon i.cheat i.dish i.mwave i.dryer i.ftype i.people i.inc c.logarea)

global controls1 $controls i.district i.audit
global controls2 $controls audit#$controls
global controls3 $controls district#$controls

*regress change_expend treat c.pool_demeaned c.aircon_demeaned c.rc_aircon_demeaned c.cheat_demeaned c.dish_demeaned c.mwave_demeaned c.dryer_demeaned c.ftype_demeaned c.people_demeaned c.inc_demeaned c.area_demeaned c.district treat#(c.pool_demeaned c.aircon_demeaned c.rc_aircon_demeaned c.cheat_demeaned c.dish_demeaned c.mwave_demeaned c.dryer_demeaned c.ftype_demeaned c.people_demeaned c.inc_demeaned c.area_demeaned c.district), vce(r)

*regress change_expend treat  

*regress change_expend treat c.pool_demeaned c.aircon_demeaned c.rc_aircon_demeaned c.cheat_demeaned c.dish_demeaned c.mwave_demeaned c.dryer_demeaned c.ftype_demeaned c.people_demeaned c.inc_demeaned c.area_demeaned c.district_demeaned treat#(c.pool_demeaned c.aircon_demeaned c.rc_aircon_demeaned c.cheat_demeaned c.dish_demeaned c.mwave_demeaned c.dryer_demeaned c.ftype_demeaned c.people_demeaned c.inc_demeaned c.area_demeaned c.district_demeaned), vce(r)


* This checks the average treatment effect across a variety of specifications
* This includes no controls OLS, all controls OLS, and Double Selection.
* <><><> *
regress change_expend treat, vce(r)
estimates store m1

regress change_expend treat $controls1, vce(r)
estimates store m2

dsregress change_expend treat, rseed(10101) controls($controls1)
estimates store m3
lassocoef (., for(change_expend)), display(coef, postselection)
lassocoef (., for(treat)), display(coef, postselection)

esttab m1 m2 m3, label title("Table 2 Regression Results (Average Treatment Effects)") nonumbers mtitles("OLS (No controls)" "OLS (All controls)" "Double Selection") modelwidth(25) keep(treat) se replace
* <><><> *

* This checks the average treatment effect across districts across a variety of specifications
* This includes no controls SIM, Double Selection SIM, and Double Selection FMM.
* <><><> *
regress change_expend treat##district, vce(r)
estimates store m4

dsregress change_expend treat##district, rseed(10101) controls($controls i.audit)
estimates store m5
lassocoef (., for(change_expend)), display(coef, postselection)

dsregress change_expend treat##district, rseed(10101) controls($controls3)
estimates store m6

esttab m4 m5 m6, label title("Table 3 Regression Results (District Average Treatment Effects)") nonumbers mtitles("SIM OLS (No controls)" "SIM Double Selection" "FMM Double Selection") modelwidth(25) se replace drop(0.treat 1.district 0.treat#1.district 0.treat#2.district 1.treat#1.district)
* <><><> *

* This checks the average treatment effect across already-audited across a variety of specifications
* This includes no controls SIM, Double Selection SIM, and Double Selection FMM.
* <><><> *
regress change_expend treat##audit, vce(r)
estimates store m7

dsregress change_expend treat##audit, rseed(10101) controls($controls i.district)
estimates store m8

dsregress change_expend treat##audit, rseed(10101) controls($controls2)
estimates store m9

esttab m7 m8 m9, label title("Table 5 Regression Results (Average Treatment Effects On Already Audited Households)") nonumbers mtitles("SIM OLS (No controls)" "SIM Double Selection" "FMM Double Selection") modelwidth(25) se replace drop(0.treat 0.audit 0.treat#0.audit 0.treat#1.audit 1.treat#0.audit)
* <><><> *

* This checks whether weather has explanatory power for differing treatment effect across districts
* <><><> *
preserve
keep if max0_1 == 27.2 | max0_1 == 26
regress change_expend treat##district, vce(r)
estimates store m10
dsregress change_expend treat##district, rseed(10101) controls($controls i.audit)
estimates store m11
lassocoef (., for(change_expend)), display(coef, postselection)
dsregress change_expend treat##district, rseed(10101) controls($controls3)
estimates store m12
lassocoef (., for(change_expend)), display(coef, postselection)

esttab m10 m11 m12, label title("Table 4 Regression Results (Average Treatment Effects On Locations With Similar Weather Patterns)") nonumbers mtitles("SIM OLS (No controls)" "SIM Double Selection" "FMM Double Selection") modelwidth(25) se replace drop(0.treat 1.district 0.treat#1.district 0.treat#2.district 1.treat#1.district)
restore
* <><><> *

log close