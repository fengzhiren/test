#include "proto_cpp.h"
#include <iostream>

const string strReqName = "REQ_NAME";
const string strAckName = "ACK_NAME";
const string strFuncName = "FUNC_NAME";
const string strProtoId = "PROTO_ID";
const string strModuleName = "MODULE_NAME";

static const string strGame2GateTemplateDeclare = 
"	bool FUNC_NAME(const char* pszData, uint32 nLen, CGame2GateSession* pSession, uint64 nId);\n";
static const string strGame2GateTemplateDefine =
"bool CGame2GateHandler::FUNC_NAME(const char* pszData, uint32 nLen, CGame2GateSession* pSession, uint64 nId)\n"
"{\n"
"	REQ_NAME oReq;\n"
"	CRole* pRole = nullptr;\n"
"	PARSE_CLIENT_PACKET(pRole, pSession, REQ_NAME, oReq, pszData, nLen, nId);\n"
"	if (pRole == nullptr)\n"
"	{\n"
"		return false;\n"
"	}\n"
"	pRole->ModuleName().FUNC_NAME(oReq);\n"
"	return true;\n"
"}\n";

static const string strGame2GateTemplateRegister =
"	RegisterDealer(SProtoSpace::PROTO_ID, &CGame2GateHandler::FUNC_NAME) &&\n";

static const string strModuleFuncDeclare =
"	void   FUNC_NAME(const REQ_NAME& oReq);\n";
static const string strModuleFuncDefine =
"void  MODULE_NAME::FUNC_NAME(const REQ_NAME& oReq)\n"
"{\n"
"	ACK_NAME oAck;\n"
"	const auto funcSend = [this, &oAck](MsgErrorType eRet)\n"
"	{\n"
"		oAck.set_ret(eRet);\n"
"		m_pOwner->Send(oAck.proto_id(), oAck, EPACKET_ACK);\n"
"	};\n\n\n"
"	funcSend(RES_ERROR_None);\n"
"}\n";

bool CProtoCpp::GenGoCode(vector<CProtoMsg>& vecProtoMsg, const string & strDstFile, const string & strMoudle)
{
	_Clear();

	m_strDstFile = strDstFile;
	m_strModuleName = strMoudle;

	if (_OpenFile() == false)
	{
		cout << " open file " << m_strDstFile << " failed!" << endl;
		return false;
	}

	for (auto & it : vecProtoMsg)
	{
		if (it.GetMsgType() == MSG_REQ)
		{
			_DealReqMsg(it);
		}
	}

	_Write2File();
	_CloseFile();
	return true;
}

bool CProtoCpp::_DealReqMsg(CProtoMsg & oProtoMsg)
{
	m_strReqName = oProtoMsg.GetMsgName();
	if (_Req2Ack(m_strReqName, m_strAckName) == false)
	{
		cout << "req 2 ack failed. req name : " << m_strReqName << endl;
		oProtoMsg.PrintMsg();
		return false;
	}

	_Underline2Hump(m_strReqName, m_strFuncName);
	if (_GenProtoId(oProtoMsg, m_strProtoId) == false)
	{
		cout << "get proto id failed" << endl;
		oProtoMsg.PrintMsg();
		return false;
	}

	_GenRegister();
	_GenPipeDeclare();
	_GenPipeDefine();
	_GenModuleDeclare();
	_GenModuleDefine();
}

bool CProtoCpp::_GenRegister()
{
	string strRegister = strGame2GateTemplateRegister;
	_ReplaceStr(strRegister);
	m_vecRegister.push_back(strRegister);
	return true;
}

bool CProtoCpp::_GenPipeDeclare()
{
	string strPipeDeclare = strGame2GateTemplateDeclare;
	_ReplaceStr(strPipeDeclare);
	m_vecPipeDeclare.push_back(strPipeDeclare);
	return true;
}

bool CProtoCpp::_GenPipeDefine()
{
	string strPipeDefine = strGame2GateTemplateDefine;
	_ReplaceStr(strPipeDefine);
	m_vecPipeDefine.push_back(strPipeDefine);
	return true;
}

bool CProtoCpp::_GenModuleDeclare()
{
	string strModuleDeclare = strModuleFuncDeclare;
	_ReplaceStr(strModuleDeclare);
	m_vecModuleDeclare.push_back(strModuleDeclare);
	return true;
}

bool CProtoCpp::_GenModuleDefine()
{
	string strModuleDefine = strModuleFuncDefine;
	_ReplaceStr(strModuleDefine);
	m_vecModuleDefine.push_back(strModuleDefine);
	return true;
}

void CProtoCpp::_ReplaceStr(string & strSrc)
{
	_ReplaceStr(strSrc, strReqName, m_strReqName);
	_ReplaceStr(strSrc, strAckName, m_strAckName);
	_ReplaceStr(strSrc, strModuleName, m_strModuleName);
	_ReplaceStr(strSrc, strFuncName, m_strFuncName);
	_ReplaceStr(strSrc, strProtoId, m_strProtoId);
}

void CProtoCpp::_ReplaceStr(string & strSrc, const string & strSub, const string & strDst)
{
	size_t nPos = strSrc.find(strSub);
	for (;nPos != string::npos;)
	{
		strSrc = strSrc.substr(0, nPos) + strDst + strSrc.substr(nPos + strSub.size());
		nPos = strSrc.find(strSub);
	}
}

void CProtoCpp::_Clear()
{
	m_vecRegister.clear();
	m_vecPipeDeclare.clear();
	m_vecPipeDefine.clear();
	m_vecModuleDeclare.clear();
	m_vecModuleDefine.clear();
}

void CProtoCpp::_Write2File()
{
	m_fStream << "\n\n=================== register =========================" << endl;
	_Write2File(m_vecRegister);

	m_fStream << "\n\n=================== pipe declare =========================" << endl;
	_Write2File(m_vecPipeDeclare);

	m_fStream << "\n\n=================== pipe define =========================" << endl;
	_Write2File(m_vecPipeDefine);

	m_fStream << "\n\n=================== module declare =========================" << endl;
	_Write2File(m_vecModuleDeclare);

	m_fStream << "\n\n=================== module define =========================" << endl;
	_Write2File(m_vecModuleDefine);
}

void CProtoCpp::_Write2File(std::vector<std::string>& vecOutput)
{
	for (auto str : vecOutput)
	{
		m_fStream << str << endl;
	}
}
