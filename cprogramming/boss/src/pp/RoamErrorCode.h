// ##########################################
// Source file : RoamErrorCode.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef ROAMERRORCODE_H_HEADER_INCLUDED_7DA25B6E
#define ROAMERRORCODE_H_HEADER_INCLUDED_7DA25B6E

// 本文件定义了漫游出访原始话单文件字段错误代码

// G网文件错误

const int R_FILE_FORMAT_ERROR           = 271           // 文件格式错误
const int R_FILE_NAME_ERROR             = 273           // 文件名错误
const int R_FILE_NAME_PCODE_ERROR       = 274           // 文件名中分公司代码错误
const int R_FILE_NAME_NO_ERROR          = 276           // 文件序列号错误
const int R_RECORD_TYPE_ERROR           = 400           // 记录类型错误
const int R_PCODE_ERROR                 = 181           // 与文件名中的分公司代码不同
const int R_FILE_SIGN_ERROR             = 401           // 错误的文件标志
const int R_FILE_NO_ERROR               = 182           // 与文件名中的序列号不同
const int R_FILE_VERSION_ERROR          = 402           // 文件版本号错误
const int R_FILE_DATE_ERROR             = 403           // 文件产生日期不符合年/月/日的规定
const int R_FIRST_CALL_DATE_ERROR       = 404           // 首次通话日期不符合年/月/日的规定
const int R_FIRST_CALL_TIME_ERROR       = 405           // 首次通话时间不符合时/秒/分的要求
const int R_LAST_CALL_DATE_ERROR        = 406           // 末次通话日期不符合年/月/日的规定
const int R_LAST_CALL_TIME_ERROR        = 407           // 末次通话时间不符合时/秒/分的要求
const int R_TOTAL_CDRS_ERROR            = 183           // 总记录数与详细记录数不符
const int R_TOTAL_FEE_ERROR             = 184           // 总话费与详细记录话费不符
const int R_TOTAL_LONGFEE1_ERROR        = 151           // 总长途1与详细记录不符
const int R_TOTAL_LONGFEE2_ERROR        = 152           // 总长途2与详细记录不符
const int R_TOTAL_LONGFEE3_ERROR        = 153           // 总长途3与详细记录不符
const int R_TOTAL_LONGFEE4_ERROR        = 154           // 总长途4与详细记录不符
const int R_TOTAL_OUTFEE_ERROR          = 155           // 总出网市话与详细记录不符
const int R_TOTAL_LONGFEE5_ERROR        = 156           // 总长途5与详细记录不符

// C网文件错误 文件拒绝原因码
// 01记录＝首记录 98记录＝尾记录

const int R_C_FILE_ERROR_BASE           = 300           // 首次发送的或没有被拒收的文件

const int R_FILE_HEAD_LOSE              = R_C_FILE_ERROR_BASE + 1           // 文件首记录丢失
const int R_FILE_CREATE_TIME_ERROR      = R_C_FILE_ERROR_BASE + 2           // 文件创建时间不符和时间格式
const int R_FILE_CREATE_TIME_LATE       = R_C_FILE_ERROR_BASE + 3           // 文件创建时间大于系统时间
const int R_FILE_PROV_CODE_ERROR        = R_C_FILE_ERROR_BASE + 4           // 错误的省分公司代码
const int R_RESERVE1                    = R_C_FILE_ERROR_BASE + 5           // 系统保留
const int R_FILE_VERSION_NOT_NUMBER     = R_C_FILE_ERROR_BASE + 6           // 话单版本号不是数字
const int R_RESERVE2                    = R_C_FILE_ERROR_BASE + 7           // 保留
const int R_FILE_NO_BEYOND_RANGE        = R_C_FILE_ERROR_BASE + 8           // 文件序列号超出值域范围
const int R_FILE_REFUSE_ERROR           = R_C_FILE_ERROR_BASE + 9           // 错误的文件拒绝码
const int R_FILE_RECEIPT_ERROR          = R_C_FILE_ERROR_BASE + 10          // 错误的回执标识
const int R_FILE_TYPE_ERROR             = R_C_FILE_ERROR_BASE + 11          // 首记录中的文件类型非法
const int R_RESERVE3                    = R_C_FILE_ERROR_BASE + 12          // 系统保留
const int R_RESERVE4                    = R_C_FILE_ERROR_BASE + 13          // 系统保留
const int R_RESERVE5                    = R_C_FILE_ERROR_BASE + 14          // 系统保留
const int R_FILE_CONTENT_ERROR          = R_C_FILE_ERROR_BASE + 15          // 错误的文件内容
const int R_FILE_NO_TAIL_IN_HEAD        = R_C_FILE_ERROR_BASE + 16          // 文件首记录没有相关的文件尾记录
const int R_FILE_REPEAT_UPLOAD          = R_C_FILE_ERROR_BASE + 17          // 文件重复上传
const int R_RESERVE6                    = R_C_FILE_ERROR_BASE + 18          // 系统保留
const int R_RESERVE7                    = R_C_FILE_ERROR_BASE + 19          // 系统保留
const int R_RESERVE8                    = R_C_FILE_ERROR_BASE + 20          // 系统保留
const int R_FILE_TAIL_LOSE              = R_C_FILE_ERROR_BASE + 21          // 尾记录丢失


#endif /* ROAMERRORCODE_H_HEADER_INCLUDED_7DA25B6E */
