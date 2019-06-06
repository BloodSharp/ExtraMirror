#include "cvar.h"
#include "Main.h"

CVARlist cvar;

#pragma warning(disable:4244)

void script_exec(char* filename)
{
	string file = szDirFile(filename);
	cmd.execFile(file.c_str());
}

void func_exec() { script_exec( cmd.argC(1) ); }

void func_alias()
{
	const char* name = cmd.argC(1);
	string& content = cmd.argS(2);
	cmd.AddAlias(name,content);
}

void func_echo()
{
	g_Engine.Con_Printf("%s\n",cmd.argC(1));
}

void set_cvar()
{
	char set_cvars[56];
	if ( cmd.names.find(g_Engine.Cmd_Argv(1)) )
	{
		sprintf_s(set_cvars,"%s %s",g_Engine.Cmd_Argv(1),g_Engine.Cmd_Argv(2));
		cmd.exec(set_cvars);
	}
	else
		cmd.exec(g_Engine.Cmd_Argv(1));
}

void CVARlist::init()
{
	memset((char*)this, 0, sizeof(*this));
	#define REGISTER_CVAR_FLOAT(name,defaultvalue) cmd.AddCvarFloat(#name, &##name );name=defaultvalue##f;
	#define REGISTER_CVAR_INT(name,defaultvalue) cmd.AddCvarInt(#name, &##name );name=defaultvalue;
	#define REGISTER_COMMAND(name) cmd.AddCommand( #name, func_##name);

	REGISTER_COMMAND(exec)
	REGISTER_COMMAND(echo)
	REGISTER_COMMAND(alias)

	// создадим тестовый квар вне игры
	REGISTER_CVAR_INT(cvar_test,1);

	g_Engine.pfnAddCommand("set",set_cvar);
}
