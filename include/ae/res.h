#ifndef _AE_RES_H
#define _AE_RES_H

typedef enum ae_res
{
	AE_RES_OK = 0,
	AE_RES_ERR = (1 << 0),
	AE_RES_WARN = (1 << 1),	
	AE_RES_INFO = (1 << 2),
} ae_res_t;

#define AE_RES_TRY(expr)                        \
     {ae_res_t r = (expr);                      \
          if(r & AE_RES_ERR) return r;          \
     }

#define AE_RES_INFO(res) ((res) & AE_RES_INFO)
#define AE_RES_WARN(res) ((res) & AE_RES_WARN)
#define AE_RES_ERR(res) ((res) & AE_RES_ERR)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif
