#ifndef __TASK_OBJECT_DETECT_CFG__
#define __TASK_OBJECT_DETECT_CFG__

// config
#define EYE_DETECT_EN			1
#define MOUTH_NOSE_DETECT_EN	1
#define FACE_RECOGNIZE_EN		1
#define GESTURE_DETECT_EN		1
#define OBJECT_RECOGNIZE_EN		1

// define
#define C_FACE_DETECT			0
#define C_FACE_EYE_DETECT		1
#define C_FACE_RECOGNIZE		2
#define C_GESTURE_DETECT		3
#define C_OBJECT_RECOGNIZE		4
#define C_USER_MODE_DETECT		5

typedef enum
{
	MSG_OBJ_DET_INIT = 0x1000,
	MSG_OBJ_DET_POST_PROC_ENABLE,
	MSG_OBJ_DET_POST_PROC_DISABLE,
	MSG_OBJ_DET_EXIT 
} MSG_OBJ_DET_ENUM;
#endif