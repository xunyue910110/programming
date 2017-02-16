/*登陆认证错误代码*/

#ifndef __AUTHSECUERROR_H_
#define __AUTHSECUERROR_H_

#include "base/Exception.h"

/*登陆员工身份认证错误*/
const int VALIDSTAFF_ERR = 310001;
/*登陆员工密码认证错误*/
const int VALIDPASSWD_ERR = 310002;
/*登陆员工IP认证错误*/
const int VALIDIP_ERR = 310003;
/*登陆员工登陆时间认证错误*/
const int VALIDDATE_ERR = 310004;
/*登陆员工地洲校验错误*/
const int GETSTAFFINFO_ERR = 310005;

/*员工密码修改错误*/
const int CHGPASSWD_ERR = 310006;

/*登陆日志记录错误*/
const int SYSLOG_ERR = 310007;

/*获取用户权限错误*/
const int GETSTAFFRIGHT_ERR = 310008;

#endif
