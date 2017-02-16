#ifndef __LOCALCONSTDEF_010__
#define __LOCALCONSTDEF_010__

#include "ConstDef.h"

// 本地化的常量定义
// 注意：
//   1.如某些定义在公共的过程引用时请将其迁移至 ConstDef.h 中。
//   2.本地化扩充的CDR属性值也请在此定义。
//   3.对定义进行合理的分组和命名。


//add by dph
const string BILL_FLAG_6       ("32"); //主叫号码剔单

const string SOURCE_TYPE_FIXGSM_HUIJIE    ("71"); //汇接局
const string SOURCE_TYPE_FIXGSM_VILLAGE   ("73"); //农话
const string SOURCE_TYPE_FIXGSM_OFFLINE   ("74"); //脱机长途
const string SOURCE_TYPE_FIXGSM_METER     ("75"); //跳次
const string SOURCE_TYPE_FIXGSM_PHS       ("76"); //小灵通
const string SOURCE_TYPE_FIXGSM_800       ("77"); //800 4006业务
const string SOURCE_TYPE_FIXSM_29         ("29"); //短信点对点
const string SOURCE_TYPE_FIXSM_2A         ("2A"); //SP短信


#endif  /*__LOCALCONSTDEF_010__*/
