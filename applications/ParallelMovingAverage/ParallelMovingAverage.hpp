#ifndef _PARALLELMOVINGAVERAGE_H_
#define _PARALLELMOVINGAVERAGE_H_

void MovingAverageDFE(int width, int n, int *in, int* out,
		      int numEngines, char** dfeIds, bool useGroups);


#endif /* _PARALLELMOVINGAVERAGE_H_ */
