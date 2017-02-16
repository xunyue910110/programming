#ifndef BUSI_API_H
#define BUSI_API_H

#include <string>

typedef int(* StartContact)(string eparchyCode, string cityCode, string contactStartTime, string contactMode, string inMode, string channelID, string contacterID, string custID, string contactDesc);
typedef int(* AppendTrade2Contact)(string eparchyCode, string cityCode, string contactTime, string contactMode, string inMode, string channelID, string contacterID, string custID, string contactTradeType, string contactTradeID, string contactDesc);
typedef int(* FinishContact)(string eparchyCode, string cityCode, string contactFinishTime, string channelID, string contacterID, string custID);


#endif //BUSI_API_H