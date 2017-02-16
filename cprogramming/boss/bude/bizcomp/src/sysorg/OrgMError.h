/*组织管理错误*/

#ifndef	ORGMERROR_H
#define ORGMERROR_H

#include "base/Exception.h"

const string  INSERT("INSERT");

/*AREAMANAGER组件超出switch选择范围*/
const int AREA_COM_0 = 152000;
/*AREA查询DAO组件报错*/
const int	AREA_DAO_S = 152001;
/*AREA修改DAO组件报错*/
const int	AREA_DAO_U = 152002;
/*AREA插入DAO组件报错*/
const int	AREA_DAO_I = 152003;
/*AREA删除DAO组件报错*/
const int	AREA_DAO_D = 152004;
/*AREADAO组件查询结果赋值报错*/
const int	AREA_COM_1 = 152005;


/*AREAMANAGER组件超出switch选择范围*/
const int DEPART_COM_0 = 152100;
/*DEPART查询DAO组件报错*/
const int	DEPART_DAO_S = 152101;
/*DEPART修改DAO组件报错*/
const int	DEPART_DAO_U = 152102;
/*DEPART插入DAO组件报错*/
const int	DEPART_DAO_I = 152103;
/*DEPART删除DAO组件报错*/
const int	DEPART_DAO_D = 152104;
/*DEPARTDAO组件查询结果赋值报错*/
const int	DEPART_COM_1 = 152105;

/*STAFFMANAGER组件超出switch选择范围*/
const int STAFF_COM_0 = 152200;
/*STAFF查询DAO组件报错*/
const int	STAFF_DAO_S = 152201;
/*STAFF修改DAO组件报错*/
const int	STAFF_DAO_U = 152202;
/*STAFF插入DAO组件报错*/
const int	STAFF_DAO_I = 152203;
/*STAFF删除DAO组件报错*/
const int	STAFF_DAO_D = 152204;
/*STAFFDAO组件查询结果赋值报错*/
const int	STAFF_COM_1 = 152205;

/*OPERLOG查询DAO组件报错*/
const int	OPERLOG_COM_1 = 152305;
/*OPERLOG查询DAO组件报错*/
const int	OPERLOG_DAO_I = 152303;

#endif
