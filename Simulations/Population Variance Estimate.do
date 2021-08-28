clear
set seed 1000

*WARNING: THIS IS VERY BAD HARDCODED CODE

* The point of the program is to test the result in my business
* statistics textbook that population variance can be estimated
* as variance < (n-1)(sample variance)/(chi squared critical value with n - 1 degrees of freedom at a certain 1 - alpha/2)
capture program drop pop_variance_estimate
program define pop_variance_estimate
    local N = `1'
	
	set obs `N'

    gen data = rnormal(0, 5)

end

capture program drop variance_test
program define variance_test
    * Generate 1001 pieces of normal data
    pop_variance_estimate 1001
	summarize data
	
	* Use the business statistics formula for capturing the true variance
	* Within a confidence interval
	* Theoretically, this should be 95% of the time since this is a 95%
	* Confidence interval
	local max = (1000)*(r(Var))/invchi2tail(1000, 0.975)
	local min = (1000)*(r(Var))/invchi2tail(1000, 0.025)
	
	gen true = 0
	replace true = 1 if (`min' < 25 & 25 < `max') 
	summarize true
	
end

set obs 1001
gen estimates = .
forvalues i = 1(1)1001 {
    preserve
    quietly variance_test 1
	restore
    quietly replace estimates = r(mean) in `i'
}

summarize estimates