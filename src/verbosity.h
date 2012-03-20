/**
 * @file verbosity.h Define how much debug output should be produced
 * by various subsystems
 */
#ifndef __HDR_verbosity_h
#define __HDR_verbosity_h

/** the verbosity level that disables all output */
const int LEV_QUIET       = 0;
/** print only warnings */
const int LEV_WARN        = 1;
/** print status messages in addition to warnings
 * for example indicate when a certain actions are started or stopped*/
const int LEV_INFO        = 2;
/** print debug messages in addition to warning and status information
 * these messages usually contain variable dumps */
const int LEV_DBG         = 3;
/** print verbose debug, this may include array dumps and very large
 * or frequent debug messages
 * (more than an operator can observe in real-time)*/
const int LEV_VDBG        = 4;

#ifdef DISABLEMESSAGES
/**
 * macro to disable the output of messages by adding a bias to the
 * required verbosity levels
 */
const int MSG_BIAS        = 9;
#else
const int MSG_BIAS        = 0;
#endif

/** verbosity level of messages from the video subsystem
 * (resolution changes, video initialization, video operations) */
const int MSG_VIDEO       = LEV_INFO   ;
/**
 * verbosity level of messages from the audio subsystem
 * (audio initialization, playback) */
const int MSG_AUDIO       = LEV_QUIET  ;
/**
 * verbosity level of font engine messages */
const int MSG_FONT        = LEV_DBG    ;
/**
 * verbosity level of OpenGL related messages */
const int MSG_GL          = LEV_INFO   ;
/**
 * verbosity level of Joystick subsystem messages */
const int MSG_JOYSTICK    = LEV_INFO   ;
/**
 * verbosity level of messages related to reading and writing
 * of the configuration files and overall configuration initialization*/
const int MSG_CONFIG      = LEV_DBG    ;
/**
 * verbosity level of messages related to stage rendering and
 * theme processing*/
const int MSG_STAGE       = LEV_DBG    ;
/**
 * verbosity level of messages related to loading PNG images */
const int MSG_PNGREAD     = LEV_DBG    ;
/**
 * verbosity level of messages regarding sound effects */
const int MSG_SFX         = LEV_INFO   ;
/**
 * verbosity level of messages related to loading or using a level set*/
const int MSG_LEVELSET    = LEV_DBG    ;
/**
 * verbosity level of messages regarding the player status, for example
 * list of levels completed and statistics */
const int MSG_PLAYERSTATE = LEV_DBG    ;

/**
 * macro to conditionally produce an informational message
 * if the verbosity level is sufficiently high
 */
#define INFO(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_INFO,__VA_ARGS__)

/**
 * macro to conditionally produce a warning message
 * if the verbosity level is sufficiently high
 */
#define WARN(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_WARN,__VA_ARGS__)

/**
 * macro to conditionally produce a debug message
 * if the verbosity level is sufficiently high
 */
#define DBG(tgt,...)     __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_DBG, __VA_ARGS__)

/**
 * macro to conditionally produce a verbose debug message
 * if the verbosity level is sufficiently high
 */
#define VDBG(tgt,...)    __COND_DBG_OUT(MSG_##tgt >= MSG_BIAS+LEV_VDBG, __VA_ARGS__)


#endif
