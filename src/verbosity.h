#ifndef __HDR_verbosity_h
#define __HDR_verbosity_h


const int LEV_QUIET       = 0;
const int LEV_WARN        = 1;
const int LEV_INFO        = 2;
const int LEV_DBG         = 3;
const int LEV_VDBG        = 4;

#ifdef DISABLEMESSAGES
const int MSG_BIAS        = 9;
#else
const int MSG_BIAS        = 0;
#endif

const int MSG_VIDEO       = LEV_INFO   ;
const int MSG_AUDIO       = LEV_QUIET  ;
const int MSG_FONT        = LEV_DBG    ;
const int MSG_GL          = LEV_INFO   ;
const int MSG_JOYSTICK    = LEV_INFO   ;
const int MSG_CONFIG      = LEV_DBG    ;
const int MSG_STAGE       = LEV_DBG    ;
const int MSG_PNGREAD     = LEV_DBG    ;
const int MSG_SPRITES     = LEV_DBG    ;
const int MSG_SFX         = LEV_INFO   ;
const int MSG_LEVELSET    = LEV_DBG    ;
const int MSG_PLAYERSTATE = LEV_DBG    ;

#define INFO(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_INFO,__VA_ARGS__)
#define WARN(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_WARN,__VA_ARGS__)
#define DBG(tgt,...)     __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_DBG, __VA_ARGS__)
#define VDBG(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_VDBG, __VA_ARGS__)


#endif
