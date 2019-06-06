#include "utils.h"

PUserMsg UserMsgByName(char* szMsgName)
{
	PUserMsg Ptr = nullptr;
	Ptr = pUserMsgBase;

	while (Ptr->next)
	{
		if (!native_strcmp(Ptr->name, szMsgName))
			return Ptr;

		Ptr = Ptr->next;
	}

	Ptr->pfn = 0;
	return Ptr;
}

PEventMsg EventMsgByName(char* szMsgName)
{
	PEventMsg Ptr = nullptr;
	Ptr = pEventMsgBase;

	while (Ptr->next)
	{
		if (!native_strcmp(Ptr->name, szMsgName))
			return Ptr;

		Ptr = Ptr->next;
	}

	Ptr->pfn = 0;
	return Ptr;
}

pfnEventMsgHook HookEventMsg(char *szMsgName, pfnEventMsgHook pfn)
{
	PEventMsg Ptr = nullptr;
	pfnEventMsgHook Original = nullptr;

	Ptr = EventMsgByName(szMsgName);

	if (Ptr->pfn != 0)
	{
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
	}
	else
	{
		MessageBoxA(0, szMsgName, 0, MB_OK | MB_ICONERROR);
	}

	return Original;
}
PEngineMsg EngineMsgByName(char* szMsgName)
{
	PEngineMsg Ptr = nullptr;
	Ptr = pEngineMsgBase;

	while (native_strcmp(Ptr->name, OFF_SVC_MESSAGES_P))
	{
		if (!native_strcmp(Ptr->name, szMsgName))
			return Ptr;

		Ptr++;
	}

	Ptr->pfn = 0;
	return Ptr;
}

pcmd_t CommandByName(char* szName)
{
	pcmd_t pCmd = nullptr;
	pCmd = g_Engine.pfnGetCmdList();

	while (pCmd)
	{
		if (!native_strcmp(pCmd->name, szName))
			return pCmd;

		pCmd = pCmd->next;
	}

	return 0;
}

pfnUserMsgHook HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	PUserMsg Ptr = nullptr;
	pfnUserMsgHook Original = nullptr;

	Ptr = UserMsgByName(szMsgName);

	if (Ptr->pfn != 0)
	{
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
	}
	else
	{
		MessageBoxA(0, szMsgName, 0, MB_OK | MB_ICONERROR);
	}

	return Original;
}

pfnEngineMessage HookEngineMsg(char *szMsgName, pfnEngineMessage pfn)
{
	PEngineMsg Ptr = nullptr;
	pfnEngineMessage Original = nullptr;

	Ptr = EngineMsgByName(szMsgName);

	if (Ptr->pfn != 0)
	{
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
	}
	else
	{
		MessageBoxA(0, szMsgName, 0, MB_OK | MB_ICONERROR);
	}

	return Original;
}

char* native_strcpy(char *dest, const char *src)
{
	return lstrcpyA(dest, src);
}

char* native_strcat(char *dest, const char *src)
{
	return lstrcatA(dest, src);
}

int native_strcmp(char *pStr1, char *pStr2)
{
	return lstrcmpA(pStr1, pStr2);
}

int native_strlen(char *pStr)
{
	return lstrlenA(pStr);
}

void native_memcpy(void * dst, const void * src, size_t count)
{
	_asm
	{
		mov	edi, [dst]
			mov	esi, [src]
			mov	ecx, [count]
			rep	movsb
	}
}

void native_memset(void *szBuffer, DWORD dwLen, DWORD dwSym)
{
	_asm
	{
		pushad
			mov edi, [szBuffer]
			mov ecx, [dwLen]
			mov eax, [dwSym]
			rep stosb
			popad
	}
}