#ifndef _NUWRO_SYSTEMATIC_PARAM_H_
#define _NUWRO_SYSTEMATIC_PARAM_H_

#include <string>

namespace nuwro {
namespace rew {

typedef enum ENuWroSyst {
  kNullSystematic = 0,
  kNuwro_Ma_CCQE,
  kNuwro_Ma_NCEL,
  kNuwro_SMa_NCEL,
  kNuwro_DeltaS_NCEL,

  // RES
  kNuwro_MaRES,
  kNuwro_CA5,
  kNuwro_SPPBkgScale,

  // Flag Norm
  kNuwro_QELNorm,
  kNuwro_RESNorm,
  kNuwro_DISNorm,
  kNuwro_COHNorm,
  kNuwro_MECNorm,
  kNuwro_CCNorm,
  kNuwro_NCNorm,
  kNuwro_AntiNuNorm,

  // DYN Norm
  kNuwro_DYNNorm_0,
  kNuwro_DYNNorm_1,
  kNuwro_DYNNorm_2,
  kNuwro_DYNNorm_3,
  kNuwro_DYNNorm_4,
  kNuwro_DYNNorm_5,
  kNuwro_DYNNorm_6,
  kNuwro_DYNNorm_7,
  kNuwro_DYNNorm_8,
  kNuwro_DYNNorm_9,

  // BBBA07 FF Pars
  kNuwro_BBBA07_AEp1,
  kNuwro_BBBA07_AEp2,
  kNuwro_BBBA07_AEp3,
  kNuwro_BBBA07_AEp4,
  kNuwro_BBBA07_AEp5,
  kNuwro_BBBA07_AEp6,
  kNuwro_BBBA07_AEp7,
  
  kNuwro_BBBA07_AMp1,
  kNuwro_BBBA07_AMp2,
  kNuwro_BBBA07_AMp3,
  kNuwro_BBBA07_AMp4,
  kNuwro_BBBA07_AMp5,
  kNuwro_BBBA07_AMp6,
  kNuwro_BBBA07_AMp7,

  kNuwro_BBBA07_AEn1,
  kNuwro_BBBA07_AEn2,
  kNuwro_BBBA07_AEn3,
  kNuwro_BBBA07_AEn4,
  kNuwro_BBBA07_AEn5,
  kNuwro_BBBA07_AEn6,
  kNuwro_BBBA07_AEn7,

  kNuwro_BBBA07_AMn1,
  kNuwro_BBBA07_AMn2,
  kNuwro_BBBA07_AMn3,
  kNuwro_BBBA07_AMn4,
  kNuwro_BBBA07_AMn5,
  kNuwro_BBBA07_AMn6,
  kNuwro_BBBA07_AMn7,

  kNuwro_BBBA07_AAx1,
  kNuwro_BBBA07_AAx2,
  kNuwro_BBBA07_AAx3,
  kNuwro_BBBA07_AAx4,
  kNuwro_BBBA07_AAx5,
  kNuwro_BBBA07_AAx6,
  kNuwro_BBBA07_AAx7,

  kNuwro_AxlFFQEL,
  
  kNuwro_Axl2comp_gamma,
  kNuwro_Axl2comp_alpha,
  kNuwro_Axl3comp_beta,
  kNuwro_Axl3comp_theta,

  kNuwro_AxlZexp_TC,
  kNuwro_AxlZexp_T0,
  kNuwro_AxlZexp_NTerms,
  kNuwro_AxlZexp_Q4Limit,
  kNuwro_AxlZexp_A0,
  kNuwro_AxlZexp_A1,
  kNuwro_AxlZexp_A2,
  kNuwro_AxlZexp_A3,
  kNuwro_AxlZexp_A4,
  kNuwro_AxlZexp_A5,
  kNuwro_AxlZexp_A6,
  kNuwro_AxlZexp_A7,
  kNuwro_AxlZexp_A8,
  kNuwro_AxlZexp_A9,
  
  kNNuWroSystematics
} NuwroSyst_t;

class NuwroSyst {
public:

  static std::string AsString(NuwroSyst_t syst);
  static NuwroSyst_t FromString(std::string syst);
  static NuwroSyst_t Next(NuwroSyst_t const &syst);
  static NuwroSyst_t Prev(NuwroSyst_t const &syst);
  static void Adv(NuwroSyst_t &syst);
  static void Red(NuwroSyst_t &syst);
};

}
}

std::ostream& operator<<(std::ostream &os, nuwro::rew::ENuWroSyst const & syst);

#endif

