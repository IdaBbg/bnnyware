#pragma once

#include <Roblox/EncryptionsHelper.hpp>

#define LUAU_COMMA_SEP ,
#define LUAU_SEMICOLON_SEP ;

#define LUAU_SHUFFLE2(s, a1, a2) a1 s a2
#define LUAU_SHUFFLE3(s, a1, a2, a3) a1 s a2 s a3
#define LUAU_SHUFFLE4(s, a1, a2, a3, a4) a1 s a2 s a3 s a4
#define LUAU_SHUFFLE5(s, a1, a2, a3, a4, a5) a1 s a2 s a3 s a4 s a5
#define LUAU_SHUFFLE6(s, a1, a2, a3, a4, a5, a6) a1 s a2 s a3 s a4 s a5 s a6
#define LUAU_SHUFFLE7(s, a1, a2, a3, a4, a5, a6, a7) a1 s a2 s a3 s a4 s a5 s a6 s a7
#define LUAU_SHUFFLE8(s, a1, a2, a3, a4, a5, a6, a7, a8) a1 s a2 s a3 s a4 s a5 s a6 s a7 s a8
#define LUAU_SHUFFLE9(s, a1, a2, a3, a4, a5, a6, a7, a8, a9) a1 s a2 s a3 s a4 s a5 s a6 s a7 s a8 s a9

#define PROTO_LINEINFO_ENC VMValue3
#define PROTO_ABSLINEINFO_ENC VMValue4
#define PROTO_LOCVARS_ENC VMValue2
#define PROTO_UPVALUES_ENC VMValue1
#define PROTO_SOURCE_ENC VMValue3

#define PROTO_DEBUGINSN_ENC VMValue1
#define PROTO_DEBUGNAME_ENC VMValue2
#define PROTO_TYPEINFO_ENC VMValue4
#define PROTO_USERDATA_ENC VMValue1

#define LSTATE_STACKSIZE_ENC VMValue1

#define CLOSURE_CONT_ENC VMValue2
#define CLOSURE_DEBUGNAME_ENC VMValue1

#define UDATA_META_ENC VMValue1

#define TSTRING_HASH_ENC VMValue2
