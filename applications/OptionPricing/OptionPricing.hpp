#ifndef _OPTIONPRICING_H_
#define _OPTIONPRICING_H_

double optionPricing(double strike,
		     double sigma,
		     double timestep,
		     int numMaturity,
                     int paraNode,
		     int numPathGroup,
		     double T,
                     double **out_rand1,
		     double **out_rand2,
		     int numDFEs);

double cpuOptionPricing(double strike,
			double sigma,
			double timestep,
			int numMaturity,
                        int paraNode,
			int numPathGroup,
			double T,
                        double *rand1,
			double *rand2);

#endif /* _OPTIONPRICING_H_ */
