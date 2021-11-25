﻿/*
	* mcfilter :  该程序负责r3的规则逻辑处理
*/
#include "HlprMiniCom.h"
#include "../lib_json/json/json.h"

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fltuser.h>

#include "nfevents.h"
#include "devctrl.h"
#include "sysinfo.h"
#include "grpc.h"

#include <stdlib.h>

using namespace std;

const char devSyLinkName[] = "\\??\\KernelDark";
const int max_size = MAX_PATH * 3;

typedef struct _PE_CONTROL{
	string name;
	unsigned int type;
	unsigned int Permissions;
}PE_CONTROL, *PPE_CONTROL;

typedef struct _RULE_NODE {
	// rule public
	unsigned int module;
	string processname;

	// mode 1
	unsigned int redirectflag;
	string redirectdirectory;
	string filewhitelist;


	// mode 2
	vector<PE_CONTROL> pecontrol;

	// mode 3
	// ......

}RULE_NODE,*PRULE_NODE;

vector<RULE_NODE> g_ruleNode;

void helpPrintf() 
{
	string helpinfo = "支持功能规则如下\n";
	helpinfo += "1. 进程文件读写访问重定向\n";
	helpinfo += "2. 进程文件读写访问访问控制\n";
}

void charTowchar(const char* chr, wchar_t* wchar, int size)
{
	MultiByteToWideChar(CP_ACP, 0, chr,
		strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}

// 内核上抛数据
class EventHandler : public NF_EventHandler
{
	void processPacket(const char* buf, int len) override
	{
		PROCESSINFO processinfo;
		RtlSecureZeroMemory(&processinfo, sizeof(PROCESSINFO));
		RtlCopyMemory(&processinfo, buf, len);

		wstring wstr;
		WCHAR info[max_size] = { 0, };

		swprintf(info, max_size, L"[Process_Log] Pid: %d\t", processinfo.processid);
		if (processinfo.endprocess)
		{
			wstr = info;
			if (lstrlenW(processinfo.queryprocesspath))
			{
				wstr += processinfo.queryprocesspath;
				wstr += '\r\n';
			}
			if (lstrlenW(processinfo.processpath))
			{
				wstr += processinfo.processpath;
				wstr += '\t';
			}
			if (lstrlenW(processinfo.commandLine))
			{
				wstr += processinfo.commandLine;
			}
		}
		else
		{
			// 进程退出
			wstr = info;
			if (lstrlenW(processinfo.queryprocesspath))
				wstr += processinfo.queryprocesspath;
		}

		OutputDebugString(wstr.data());
	}

	void threadPacket(const char* buf, int len) override
	{
		THREADINFO threadinfo;
		RtlSecureZeroMemory(&threadinfo, sizeof(THREADINFO));
		RtlCopyMemory(&threadinfo, buf, len);

		WCHAR info[max_size] = { 0, };
		swprintf(info, max_size, L"[Thread_Log] Pid: %d Threadid: %d ExitStatus: %d", threadinfo.processid, threadinfo.threadid, threadinfo.createid);
		OutputDebugString(info);
	}

	void imagemodPacket(const char* buf, int len) override
	{
		IMAGEMODINFO imageinfo;
		RtlSecureZeroMemory(&imageinfo, sizeof(IMAGEMODINFO));
		RtlCopyMemory(&imageinfo, buf, len);

		WCHAR info[max_size] = { 0, };
		swprintf(info, max_size, L"[Image_Log] Pid: %d ImageBase: %p ImageSize: %d \n ImageBasePath: %s", imageinfo.processid, imageinfo.imagebase, imageinfo.imagesize, imageinfo.imagename);
		OutputDebugString(info);
	}

	void registerPacket(const char* buf, int len) override
	{
		REGISTERINFO registerinfo;
		RtlSecureZeroMemory(&registerinfo, sizeof(REGISTERINFO));
		RtlCopyMemory(&registerinfo, buf, len);

		wstring opearestring;
		switch (registerinfo.opeararg)
		{
			// 创建Key
			case RegNtPreCreateKey:
			{
				opearestring = L"Register - Create Key";
			}
			break;
			// 打开Key
			case RegNtPreOpenKey:
			{
				opearestring = L"Register - Open Key";
			}
			break;

			// 修改Key
			case RegNtSetValueKey:
			{

			}
			// 删除Key
			case RegNtPreDeleteKey:
			{
				opearestring = L"Register - Modify Key";
			}
			break;

			// 枚举Key
			case RegNtEnumerateKey:
			{
				opearestring = L"Register - Emun Key";
			}
			break;

			// 重命名注册表
			case RegNtPostRenameKey:
			{
				opearestring = L"Register - Rename Key";
			}
			break;
		}
		if (opearestring.size())
		{
			WCHAR info[max_size] = { 0, };
			swprintf(info, max_size, L"[Register_Log] Pid: %d Threadid:%d\t%s", registerinfo.processid, registerinfo.threadid, opearestring.data());
			OutputDebugString(info);
		}
	}

	void filePacket(const char* buf, int len) override
	{
		FILEINFO fileinfo;
		RtlSecureZeroMemory(&fileinfo, sizeof(FILEINFO));
		RtlCopyMemory(&fileinfo, buf, len);

		WCHAR info[max_size] = { 0, };
		swprintf(info, max_size, L"[File_Log]: Pid %d Threadid:%d\nDosPath: %s - FileName: %s", fileinfo.processid, fileinfo.threadid, fileinfo.DosName, fileinfo.FileName);
		OutputDebugString(info);
	}

	void sessionPacket(const char* buf, int len) override
	{
		SESSIONINFO sessioninfo;
		RtlSecureZeroMemory(&sessioninfo, sizeof(SESSIONINFO));
		RtlCopyMemory(&sessioninfo, buf, len);

		IO_SESSION_STATE_INFORMATION iosession;
		RtlSecureZeroMemory(&iosession, sizeof(IO_SESSION_STATE_INFORMATION));
		RtlCopyMemory(&iosession, sessioninfo.iosessioninfo, sizeof(IO_SESSION_STATE_INFORMATION));

		wstring events;
		switch (sessioninfo.evens)
		{
		case IoSessionStateCreated:
		{
			events = L"Session Create";
		}
		break;
		case IoSessionStateConnected:
		{
			events = L"Session Connect, But User NotLogin";
		}
		break;
		case IoSessionStateLoggedOn:
		{
			events = L"Session Login";
		}
		break;
		case IoSessionStateLoggedOff:
		{
			events = L"Session ExitLogin";
		}
		break;
		}
		if (events.size() == 0)
			return;

		if (iosession.LocalSession)
			events += L" - User Local Login";
		else
			events += L" - User Remote Login";

		WCHAR info[max_size] = { 0, };
		swprintf(info, max_size, L"[Session_Log]: Pid %d Threadid:%d\n%s - EventId: %d", sessioninfo.processid, sessioninfo.threadid, events.data(), iosession.SessionId);
		OutputDebugString(info);
	}
};


static DevctrlIoct			devobj;
static EventHandler			eventobj;

bool gethostip(RawData* ip_liststr)
{
	WSAData data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		return false;

	char host[255] = { 0, };
	do {

		if (gethostname(host, sizeof(host)) == SOCKET_ERROR)
			break;

		auto p = gethostbyname(host);
		if (p == 0)
			break;
		else
		{
			for (int i = 0; p->h_addr_list[i] != 0; i++)
			{
				struct in_addr in;
				memcpy(&in, p->h_addr_list[i], sizeof(struct in_addr));
				ip_liststr->set_intranetipv4(i, inet_ntoa(in));
			}
		}
	
	} while (false);

	WSACleanup();
}
bool SysNodeOnlineData(RawData* sysinfobuffer)
{
	
	sysinfobuffer->mutable_pkg();

}
typedef struct _SYSTEMONLIENNODE
{
	wchar_t platform[260];
	int id;
	__int64 id64;
}SYSTEMONLIENNODE, * PSYSTEMONLIENNODE;

DWORD pthread_grpread(
	LPVOID lpThreadParameter
)
{
	Grpc* greeter = (Grpc*)lpThreadParameter;
	greeter->Grpc_ReadC2Thread(NULL);
	return 1;
}

int main(int argc, char* argv[])
{
	getchar();
	// 
	// @ Grpc Active Online Send to  Server Msg
	// SSL
	// auto rootcert = get_file_contents(rootcrt_path);
	// auto clientkey = get_file_contents(clientkey_path);
	// auto clientcert = get_file_contents(clientcrt_path);
	// grpc::SslCredentialsOptions ssl_opts;
	// ssl_opts.pem_root_certs = rootcert;
	// ssl_opts.pem_private_key = clientkey;
	// ssl_opts.pem_cert_chain = clientcert;
	// std::shared_ptr<grpc::ChannelCredentials> channel_creds = grpc::SslCredentials(ssl_opts);
	
	//  grpc::InsecureChannelCredentials()
	static Grpc greeter(
		grpc::CreateChannel("10.128.128.23:8888", grpc::InsecureChannelCredentials()));
	
	proto::RawData rawData;
	DWORD ComUserLen = MAX_PATH;
	CHAR ComUserName[MAX_PATH] = { 0, };
	GetComputerNameA(ComUserName, &ComUserLen);
	// Send Agent
	rawData.set_hostname(ComUserName);
	rawData.set_version("0.1");
	rawData.set_agentid("123");
	rawData.set_timestamp(GetCurrentTime());
	greeter.Grpc_Transfer(rawData);

	// Send System Onliy Buffer 
	rawData.Clear();
	rawData.set_hostname(ComUserName);
	rawData.set_version("0.1");
	rawData.set_agentid("123");
	rawData.set_timestamp(GetCurrentTime());
	::proto::Record* pkg_re = rawData.add_pkg();
	auto MapMessage = pkg_re->mutable_message();
	(*MapMessage)["platform"] =  "windows";
	(*MapMessage)["agent_id"] = "123";
	(*MapMessage)["timestamp"] = "1111";
	(*MapMessage)["hostname"] = ComUserName;
	(*MapMessage)["version"] = "0.1";
	(*MapMessage)["in_ipv4_list"] = "localhost";
	(*MapMessage)["in_ipv6_list"] = "localhost";
	(*MapMessage)["data_type"] = "1";
	greeter.Grpc_Transfer(rawData);

	// start grpc read thread (Wait server Data)
	DWORD threadid = 0;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pthread_grpread, &greeter, 0, &threadid);

	int status = 0;

	// Init devctrl
	status = devobj.devctrl_init();
	if (0 > status)
	{
		cout << "devctrl_init error: main.c --> lines: 342" << endl;
		return -1;
	}

	do
	{
		// Open driver
		status = devobj.devctrl_opendeviceSylink(devSyLinkName);
		if (0 > status)
		{
			cout << "devctrl_opendeviceSylink error: main.c --> lines: 352" << endl;
			break;
		}

		// Init share Mem
		status = devobj.devctrl_InitshareMem();
		if (0 > status)
		{
			cout << "devctrl_InitshareMem error: main.c --> lines: 360" << endl;
			break;
		}

		//status = devobj.devctrl_workthread();
		//if (0 > status)
		//{
		//	cout << "devctrl_workthread error: main.c --> lines: 367" << endl;
		//	break;
		//}

		// Enable try Network packte Monitor
		//status = devobj.devctrl_OnMonitor();
		//if (0 > status)
		//{
		//	cout << "devctrl_InitshareMem error: main.c --> lines: 375" << endl;
		//	break;
		//}

		// Enable Event
		devobj.nf_setEventHandler((PVOID)&eventobj);

		status = 1;

	} while (false);

	if (!status)
	{
		OutputDebugString(L"Init Driver Failuer");
		return -1;
	}

	Command cmd;
	cmd.set_agentctrl(100);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(101);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(103);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(108);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(109);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(110);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(111);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(113);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cmd.Clear();
	cmd.set_agentctrl(115);
	greeter.Grpc_ReadDispatchHandle(cmd);

	cout << "输入回车结束进程" << endl;
	getchar();

	devobj.devctrl_free();
	exit(0);

	/*
	*   未开放
		Json Config Alay
			Json::FastWriter writer;
			Json::Value	Jsontestnode;
			Jsontestnode["platform"] = "windows";
			Jsontestnode["agent_id"] = "123";
			Jsontestnode["timestamp"] = "1111";
			Jsontestnode["hostname"] = ComUserName;
			Jsontestnode["version"] = "0.1";
			Jsontestnode["in_ipv4_list"] = "localhost";
			Jsontestnode["in_ipv6_list"] = "localhost";
			Jsontestnode["data_type"] = "1";
			// 序列化
			string jsonreq = writer.write(Jsontestnode);
	*/
	OutputDebugString(L"Init Driver Success. Json Rule Init Wait.......");
	bool nstatus = false;
	// read rule to mcrule.json
	Json::Value root;
	ifstream ifs;
	// debug
	// ifs.open("../rule_config/mcrule.json");
	// release
	ifs.open("mcrule.json");
	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		cout << errs << endl;
		return EXIT_FAILURE;
	}
	std::cout << root << std::endl;
	
	// 规则初始化
	PE_CONTROL rulecontrol;
	RULE_NODE rulenode;
	unsigned int processNameLen = 0;
	WCHAR processNameArray[MAX_PATH][MAX_PATH] = { 0, };
	WCHAR towchar[MAX_PATH] = { 0, };
	auto size = root.size();
	for (int i = 0; i < size; ++i)
	{
		try {
			auto rule_list = root["mcrule"];
			for (int j = 0; j < rule_list.size(); ++j) {
				memset(&rulenode, 0, sizeof(RULE_NODE));
				auto c_strbuf = rule_list[j]["processname"].asString().c_str();
				if (!c_strbuf)
					continue;
				memset(towchar, 0, sizeof(WCHAR) * MAX_PATH);
				charTowchar(c_strbuf, towchar, sizeof(WCHAR) * MAX_PATH);
				if (!lstrlenW(towchar))
					continue;
				lstrcpyW(processNameArray[j], towchar);
				processNameLen++;
				auto code = rule_list[j]["module"].asInt();
				rulenode.module = code;
				int k = 0;
				// 不同的模式解析
				switch (code)
				{
				case 1:
				{
					// 目录重定向模式
					rulenode.processname = rule_list[j]["processname"].asString();
					rulenode.redirectflag = rule_list[j]["redirectflag"].asInt();
					rulenode.redirectdirectory = rule_list[j]["redirectdirectory"].asString();
					g_ruleNode.push_back(rulenode);
				}
				break;
				case 2:
				{
					// 权限访问控制
					rulenode.processname = rule_list[j]["processname"].asString();
					auto ctrl_list = rule_list[j]["pecontrol"];
					for (k = 0; k < ctrl_list.size(); ++k) {
						memset(&rulecontrol, 0, sizeof(PE_CONTROL));
						rulecontrol.name = ctrl_list[k]["name"].asString();
						rulecontrol.type = ctrl_list[k]["type"].asInt();
						rulecontrol.Permissions = ctrl_list[k]["Permissions"].asInt();
						rulenode.pecontrol.push_back(rulecontrol);
						g_ruleNode.push_back(rulenode);
					}
				}
				break;
				default:
					break;
				}
			}
		}
		catch (const std::exception&)
		{
			cout << "line:53 - 第%d规则解析错误." << i << endl;
		}
	}

	// 设置进程名
	nf_SetRuleProcess(processNameArray[0], sizeof(processNameArray), processNameLen);
	// Class Init to Driver Event Handle
	// 比如驱动拦截数据，ALPC通知Sever, Server调用Class规则处理类，允许还是通
}