/**\file */
#ifndef SLIC_DECLARATIONS_OptionPricing_H
#define SLIC_DECLARATIONS_OptionPricing_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*----------------------------------------------------------------------------*/
/*---------------------------- Interface default -----------------------------*/
/*----------------------------------------------------------------------------*/



/**
 * \brief Basic static function for the interface 'default'.
 * 
 * \param [in] param_initsize Interface Parameter "initsize".
 * \param [in] param_nodesize Interface Parameter "nodesize".
 * \param [in] param_pathsize Interface Parameter "pathsize".
 * \param [in] param_seedsize Interface Parameter "seedsize".
 * \param [in] ticks_OptionPricingKernel The number of ticks for which kernel "OptionPricingKernel" will run.
 * \param [in] inscalar_OptionPricingKernel_T Input scalar parameter "OptionPricingKernel.T".
 * \param [in] inscalar_OptionPricingKernel_discount Input scalar parameter "OptionPricingKernel.discount".
 * \param [in] inscalar_OptionPricingKernel_numMaturity Input scalar parameter "OptionPricingKernel.numMaturity".
 * \param [in] inscalar_OptionPricingKernel_numPath Input scalar parameter "OptionPricingKernel.numPath".
 * \param [in] inscalar_OptionPricingKernel_outputRand Input scalar parameter "OptionPricingKernel.outputRand".
 * \param [in] inscalar_OptionPricingKernel_sigma Input scalar parameter "OptionPricingKernel.sigma".
 * \param [in] inscalar_OptionPricingKernel_sqrt_t Input scalar parameter "OptionPricingKernel.sqrt_t".
 * \param [in] inscalar_OptionPricingKernel_strike Input scalar parameter "OptionPricingKernel.strike".
 * \param [in] instream_fin The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_maturity The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_maturity_diff The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_seed The stream should be of size (param_seedsize * 4) bytes.
 * \param [in] instream_seed2 The stream should be of size (param_seedsize * 4) bytes.
 * \param [out] outstream_randOut The stream should be of size (param_pathsize * 8) bytes.
 * \param [out] outstream_randOut2 The stream should be of size (param_pathsize * 8) bytes.
 * \param [out] outstream_result The stream should be of size (param_nodesize * 8) bytes.
 */
void OptionPricing(
	int64_t param_initsize,
	int64_t param_nodesize,
	int64_t param_pathsize,
	int64_t param_seedsize,
	uint64_t ticks_OptionPricingKernel,
	double inscalar_OptionPricingKernel_T,
	double inscalar_OptionPricingKernel_discount,
	uint64_t inscalar_OptionPricingKernel_numMaturity,
	uint64_t inscalar_OptionPricingKernel_numPath,
	uint64_t inscalar_OptionPricingKernel_outputRand,
	double inscalar_OptionPricingKernel_sigma,
	double inscalar_OptionPricingKernel_sqrt_t,
	double inscalar_OptionPricingKernel_strike,
	const double *instream_fin,
	const double *instream_maturity,
	const double *instream_maturity_diff,
	const uint32_t *instream_seed,
	const uint32_t *instream_seed2,
	double *outstream_randOut,
	double *outstream_randOut2,
	double *outstream_result);

/**
 * \brief Basic static non-blocking function for the interface 'default'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_initsize Interface Parameter "initsize".
 * \param [in] param_nodesize Interface Parameter "nodesize".
 * \param [in] param_pathsize Interface Parameter "pathsize".
 * \param [in] param_seedsize Interface Parameter "seedsize".
 * \param [in] ticks_OptionPricingKernel The number of ticks for which kernel "OptionPricingKernel" will run.
 * \param [in] inscalar_OptionPricingKernel_T Input scalar parameter "OptionPricingKernel.T".
 * \param [in] inscalar_OptionPricingKernel_discount Input scalar parameter "OptionPricingKernel.discount".
 * \param [in] inscalar_OptionPricingKernel_numMaturity Input scalar parameter "OptionPricingKernel.numMaturity".
 * \param [in] inscalar_OptionPricingKernel_numPath Input scalar parameter "OptionPricingKernel.numPath".
 * \param [in] inscalar_OptionPricingKernel_outputRand Input scalar parameter "OptionPricingKernel.outputRand".
 * \param [in] inscalar_OptionPricingKernel_sigma Input scalar parameter "OptionPricingKernel.sigma".
 * \param [in] inscalar_OptionPricingKernel_sqrt_t Input scalar parameter "OptionPricingKernel.sqrt_t".
 * \param [in] inscalar_OptionPricingKernel_strike Input scalar parameter "OptionPricingKernel.strike".
 * \param [in] instream_fin The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_maturity The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_maturity_diff The stream should be of size (param_initsize * 8) bytes.
 * \param [in] instream_seed The stream should be of size (param_seedsize * 4) bytes.
 * \param [in] instream_seed2 The stream should be of size (param_seedsize * 4) bytes.
 * \param [out] outstream_randOut The stream should be of size (param_pathsize * 8) bytes.
 * \param [out] outstream_randOut2 The stream should be of size (param_pathsize * 8) bytes.
 * \param [out] outstream_result The stream should be of size (param_nodesize * 8) bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *OptionPricing_nonblock(
	int64_t param_initsize,
	int64_t param_nodesize,
	int64_t param_pathsize,
	int64_t param_seedsize,
	uint64_t ticks_OptionPricingKernel,
	double inscalar_OptionPricingKernel_T,
	double inscalar_OptionPricingKernel_discount,
	uint64_t inscalar_OptionPricingKernel_numMaturity,
	uint64_t inscalar_OptionPricingKernel_numPath,
	uint64_t inscalar_OptionPricingKernel_outputRand,
	double inscalar_OptionPricingKernel_sigma,
	double inscalar_OptionPricingKernel_sqrt_t,
	double inscalar_OptionPricingKernel_strike,
	const double *instream_fin,
	const double *instream_maturity,
	const double *instream_maturity_diff,
	const uint32_t *instream_seed,
	const uint32_t *instream_seed2,
	double *outstream_randOut,
	double *outstream_randOut2,
	double *outstream_result);

/**
 * \brief Advanced static interface, structure for the engine interface 'default'
 * 
 */
typedef struct { 
	int64_t param_initsize; /**<  [in] Interface Parameter "initsize". */
	int64_t param_nodesize; /**<  [in] Interface Parameter "nodesize". */
	int64_t param_pathsize; /**<  [in] Interface Parameter "pathsize". */
	int64_t param_seedsize; /**<  [in] Interface Parameter "seedsize". */
	uint64_t ticks_OptionPricingKernel; /**<  [in] The number of ticks for which kernel "OptionPricingKernel" will run. */
	double inscalar_OptionPricingKernel_T; /**<  [in] Input scalar parameter "OptionPricingKernel.T". */
	double inscalar_OptionPricingKernel_discount; /**<  [in] Input scalar parameter "OptionPricingKernel.discount". */
	uint64_t inscalar_OptionPricingKernel_numMaturity; /**<  [in] Input scalar parameter "OptionPricingKernel.numMaturity". */
	uint64_t inscalar_OptionPricingKernel_numPath; /**<  [in] Input scalar parameter "OptionPricingKernel.numPath". */
	uint64_t inscalar_OptionPricingKernel_outputRand; /**<  [in] Input scalar parameter "OptionPricingKernel.outputRand". */
	double inscalar_OptionPricingKernel_sigma; /**<  [in] Input scalar parameter "OptionPricingKernel.sigma". */
	double inscalar_OptionPricingKernel_sqrt_t; /**<  [in] Input scalar parameter "OptionPricingKernel.sqrt_t". */
	double inscalar_OptionPricingKernel_strike; /**<  [in] Input scalar parameter "OptionPricingKernel.strike". */
	const double *instream_fin; /**<  [in] The stream should be of size (param_initsize * 8) bytes. */
	const double *instream_maturity; /**<  [in] The stream should be of size (param_initsize * 8) bytes. */
	const double *instream_maturity_diff; /**<  [in] The stream should be of size (param_initsize * 8) bytes. */
	const uint32_t *instream_seed; /**<  [in] The stream should be of size (param_seedsize * 4) bytes. */
	const uint32_t *instream_seed2; /**<  [in] The stream should be of size (param_seedsize * 4) bytes. */
	double *outstream_randOut; /**<  [out] The stream should be of size (param_pathsize * 8) bytes. */
	double *outstream_randOut2; /**<  [out] The stream should be of size (param_pathsize * 8) bytes. */
	double *outstream_result; /**<  [out] The stream should be of size (param_nodesize * 8) bytes. */
} OptionPricing_actions_t;

/**
 * \brief Advanced static function for the interface 'default'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void OptionPricing_run(
	max_engine_t *engine,
	OptionPricing_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'default'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *OptionPricing_run_nonblock(
	max_engine_t *engine,
	OptionPricing_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'default'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void OptionPricing_run_group(max_group_t *group, OptionPricing_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *OptionPricing_run_group_nonblock(max_group_t *group, OptionPricing_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'default'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void OptionPricing_run_array(max_engarray_t *engarray, OptionPricing_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *OptionPricing_run_array_nonblock(max_engarray_t *engarray, OptionPricing_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* OptionPricing_convert(max_file_t *maxfile, OptionPricing_actions_t *interface_actions);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* OptionPricing_init(void);

/* Error handling functions */
int OptionPricing_has_errors(void);
const char* OptionPricing_get_errors(void);
void OptionPricing_clear_errors(void);
/* Free statically allocated maxfile data */
void OptionPricing_free(void);
/* These are dummy functions for hardware builds. */
int OptionPricing_simulator_start(void);
int OptionPricing_simulator_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_OptionPricing_H */

