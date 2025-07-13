#ifndef _UPDATE_H_
#define _UPDATE_H_


//升级的参数/类别
#define UPDATE_ASSERT 	"assert="	
#define UPDATE_APP		"app="
//#define UPDATE_


//升级方式
typedef enum {
    UPDATE_MODE_0 	= 0, 	//不升级
    UPDATE_MODE_1	= 1,    //只增加新增文件
    UPDATE_MODE_2	= 2,    //删除减少的文件增加新增的文件
	UPDATE_MODE_3	= 3,    //增加新增的文件替换已有的文件
	UPDATE_MODE_4	= 4    	//全部替换
} UpdateMode;



#endif
