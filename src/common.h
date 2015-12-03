#ifndef _COMMON_FGRM
#define _COMMON_FGRM

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include <omp.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multimin.h>

#ifdef _LONGIDS
typedef long lint;
#else //_LONGIDS
typedef int lint;
#endif //_LONGIDS

#ifdef _SPREC
typedef float flouble;
typedef float complex fcomplex;
#else //_SPREC
typedef double flouble;
typedef double complex fcomplex;
#endif //_SPREC

typedef struct {
  int nside;
  int nside_spec;
  int n_side_sub;
  int n_sub;
  int n_pix;
  int n_pix_spec;

  char input_data_prefix[256];
  flouble *maps_data;

  char input_noise_prefix[256];
  flouble *maps_noise_weight;

  char output_prefix[256];
  flouble *map_components_mean;
  flouble *map_components_covar;
  flouble *map_indices_mean;
  flouble *map_indices_covar;
  flouble *map_chi2;

  char fname_nulist[256];
  int n_nu;
  flouble *freqs;

  int flag_include_polarization;
  int n_pol;

  int flag_include_cmb;
  int flag_include_synchrotron;
  int flag_include_dust;
  int n_comp;
  int index_cmb;
  int index_synchrotron;
  int index_dust;

  int flag_independent_polarization;
  int flag_beta_s_free;
  int flag_beta_d_free;
  int flag_temp_d_free;
  int n_param_max;
  int n_spec_vary;
  int n_dof_pix;
  int index_beta_s_t;
  int index_beta_s_p;
  int index_beta_d_t;
  int index_beta_d_p;
  int index_temp_d_t;
  int index_temp_d_p;
  flouble beta_s_0;
  flouble beta_d_0;
  flouble temp_d_0;
  flouble sigma_beta_s;
  flouble sigma_beta_d;
  flouble sigma_temp_d;
  flouble nu0_s;
  flouble nu0_d;

  unsigned long seed;
  int n_samples;
  flouble frac_samples_burn;
  int n_update_covar;
  int n_samples_burn;

  int dbg_ipix;
  flouble *dbg_extra;
} ParamFGRM;

//Defined in common.c
int my_linecount(FILE *f);
void report_error(int level,char *fmt,...);
void *my_malloc(size_t size);
void *my_calloc(size_t nmemb,size_t size);
FILE *my_fopen(const char *path,const char *mode);
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
void param_fgrm_free(ParamFGRM *par);
ParamFGRM *read_params(char *fname);
void write_output(ParamFGRM *par);
void write_debug_info(ParamFGRM *par);
void dbg_printf(char *fmt,...);

//Defined in healpix_extra.c
long he_nalms(int lmax);
long he_indexlm(int l,int m,int lmax);
void he_alm2map(int nside,int lmax,int ntrans,flouble **maps,fcomplex **alms);
void he_map2alm(int nside,int lmax,int ntrans,flouble **maps,fcomplex **alms);
void he_anafast(flouble **maps_1,flouble **maps_2,
		int nmaps_1,int nmaps_2,
		int pol_1,int pol_2,
		flouble **cls,int nside,int lmax);
void he_write_healpix_map(flouble **tmap,int nfields,long nside,char *fname);
flouble *he_read_healpix_map(char *fname,long *nside,int nfield);
int he_ring_num(long nside,double z);
void he_ring2nest_inplace(flouble *map_in,long nside);
void he_nest2ring_inplace(flouble *map_in,long nside);
void he_udgrade(flouble *map_in,long nside_in,
		flouble *map_out,long nside_out,
		int nest);
double *he_generate_beam_window(int lmax,double fwhm_amin);
void he_alter_alm(int lmax,double fwhm_amin,fcomplex *alms,double *window);

//Defined in rng.c
#define RNG_NRAN 624
#define RNG_MRAN 397
#define RNG_MATRIX_A 0x9908b0df
#define RNG_UPPER_MASK 0x80000000UL
#define RNG_LOWER_MASK 0x7fffffffUL
typedef struct {
  unsigned long mt[RNG_NRAN];
  int mti;
  int calc_gauss;
  double u;
  double phi;
} Rng;
Rng *init_rng(unsigned long seed);
void end_rng(Rng *rng);
unsigned long rand_ulong(Rng *rng);
double rand_real01(Rng *rng);
double rand_gauss(Rng *rng);

//Defined in fgrm.c
#define N_CHECK 1000
#define N_BURN_FRAC 0.2
typedef struct {
  flouble *f_matrix;
  gsl_matrix **cov_inv;
  gsl_vector **vec_mean;
  Rng *rng;
  flouble *rand_spec;
} PixelState;
PixelState *pixel_state_new(ParamFGRM *par,unsigned long seed);
void pixel_state_free(PixelState *pst,ParamFGRM *par);
void clean_pixel(ParamFGRM *par,PixelState *pst,int ipix_big);

#endif //_COMMON_FGRM
