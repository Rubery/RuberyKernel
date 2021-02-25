#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <cstdint>
#include <vector>
#include <math.h>
#include <vector>


typedef struct _NULL_MEMORY
{
	void* buffer_address;
	UINT_PTR address;
	ULONGLONG size;
	ULONG pid;
	BOOLEAN write;
	BOOLEAN read;
	BOOLEAN req_base;
	void* output;
	const char* module_name;
	ULONG64 base_address;
}NULL_MEMORY;

uintptr_t base_address = 0;
std::uint32_t process_id = 0;
bool god = false;
bool xp = false;
bool ammo = false;
bool gun = false;
bool hp = false;
bool tp = false;

template<typename ... Arg>
uint64_t call_hook(const Arg ... args)
{
	void* hooked_func = GetProcAddress(LoadLibrary("win32u.dll"), "NtQueryCompositionSurfaceStatistics");

	auto func = static_cast<uint64_t(_stdcall*)(Arg...)>(hooked_func);

	return func(args ...);
}

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

std::uint32_t get_process_id(std::string_view process_name)
{
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return NULL;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
	{
		if (process_name.compare(processentry.szExeFile) == NULL)
		{
			return processentry.th32ProcessID;
		}
	}
	return NULL;
}

ULONG64 get_client_base(const char* module_name)
{

	NULL_MEMORY instructions = { 0 };
	instructions.pid = process_id;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.module_name = module_name;
	call_hook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}

template <class T>
T Read(UINT_PTR read_address)
{
	T response{};
	NULL_MEMORY instructions;
	instructions.pid = process_id;
	instructions.size = sizeof(T);
	instructions.address = read_address;
	instructions.read = TRUE;
	instructions.write = FALSE;
	instructions.req_base = FALSE;
	instructions.output = &response;
	call_hook(&instructions);

	return response;
}

bool write_memory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
{
	NULL_MEMORY instructions;
	instructions.address = write_address;
	instructions.pid = process_id;
	instructions.write = TRUE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.buffer_address = (void*)source_address;
	instructions.size = write_size;

	call_hook(&instructions);

	return true;
}

template<typename S>
bool write(UINT_PTR write_address, const S& value)
{
	return write_memory(write_address, (UINT_PTR)&value, sizeof(S));

}

void money() // set money for all players
{
	INT_PTR PlayerCompPtr = Read<UINT_PTR>(base_address + 0x10AC72F8); // 0x10AC72F8 = playerbase
	if (GetAsyncKeyState(VK_F1))
	{
		printf("money set\n");
		write<ULONG>(PlayerCompPtr + 0x5D14 + 0x20, 100000);
		write<ULONG>(PlayerCompPtr + 0x5D14 + 0xB940 + 0x20, 100000);
		write<ULONG>(PlayerCompPtr + 0x5D14 + (2 * 0xB940) + 0x20, 100000);
		write<ULONG>(PlayerCompPtr + 0x5D14 + (3 * 0xB940) + 0x20, 100000);

		Sleep(1000);
	}
}

void raygun()
{
	if (GetAsyncKeyState(VK_F2))
	{
		INT_PTR PlayerCompPtr = Read<UINT_PTR>(base_address + 0x10AC72F8);

		gun = !gun;
		if (gun == true) {

			printf("raygun set\n");
			write<ULONG>(PlayerCompPtr + 0xB0 + 0 * 0x40, 22);
			write<ULONG>(PlayerCompPtr + 0xB0 + 1 * 0x40, 22);

			Sleep(1000);
		}
		else {
			printf("raygun unset\n");
			write<ULONG>(PlayerCompPtr + 0xB0 + 0 * 0x40, 1);
			write<ULONG>(PlayerCompPtr + 0xB0 + 1 * 0x40, 1);
			write<ULONG>(PlayerCompPtr + 0xB0 + 0 * 0x40 + 0xB830, 1);
			write<ULONG>(PlayerCompPtr + 0xB0 + 1 * 0x40 + 0xB830, 1);
			write<ULONG>(PlayerCompPtr + 0xB0 + 0 * 0x40 + (2 * 0xB830), 1);
			write<ULONG>(PlayerCompPtr + 0xB0 + 1 * 0x40 + (2 * 0xB830), 1);
			Sleep(1000);
		}
	}
}

void xpgun()
{
	if (GetAsyncKeyState(VK_F4))
	{
		INT_PTR PlayerCompPtr = Read<UINT_PTR>(base_address + 0x10AC72F8);

		xp = !xp;
		if (xp == true) {
			printf("xp on\n");

			write<FLOAT>(base_address + 0x10AEF2F0 + 0x08, 2000);

			write<FLOAT>(base_address + 0x10AEF2F0 + 0x10, 500);

			Sleep(1000);

		}
		else
		{
			printf("xp off\n");
			write<FLOAT>(base_address + 0x10AEF2F0 + 0x08, 1);
			write<FLOAT>(base_address + 0x10AEF2F0 + 0x10, 1);


			Sleep(1000);

		}

	}
}
void godmode()
{

	if (GetAsyncKeyState(VK_F3))
	{
		INT_PTR PlayerCompPtr = Read<UINT_PTR>(base_address + 0x10AC72F8);

		god = !god;
		if (god == true) {
			printf("godmode on\n");
			write<BYTE>(PlayerCompPtr + 0xE67, 0xA0);
			write<BYTE>(PlayerCompPtr + 0xE67 + 0xB830, 0xA0);
			write<BYTE>(PlayerCompPtr + 0xE67 + (2 * 0xB830), 0xA0);
			write<BYTE>(PlayerCompPtr + 0xE67 + (3 * 0xB830), 0xA0);


			Sleep(1000);

		}
		else
		{
			printf("godmode off\n");
			write<BYTE>(PlayerCompPtr + 0xE67, 0x20);
			write<BYTE>(PlayerCompPtr + 0xE67 + 0xB830, 0x20);
			write<BYTE>(PlayerCompPtr + 0xE67 + (2 * 0xB830), 0x20);
			write<BYTE>(PlayerCompPtr + 0xE67 + (3 * 0xB830), 0x20);


			Sleep(1000);

		}

	}

}

void tp1hp() // tp close to you and 1 hp
{
	if (GetAsyncKeyState(VK_F9))
	{
		tp = !tp;
		INT_PTR ZMBotBase = Read<UINT_PTR>(base_address + 0x10A9FBC8 + 0x68);
		INT_PTR ZM_Bot_List_Offset = Read<UINT_PTR>(ZMBotBase + 0x8);
		INT_PTR PlayerPedPtr = Read<UINT_PTR>(base_address + 0x10A9FBC8 + 0x8);

		while (tp == true) {
			FLOAT playerCoords1 = Read<FLOAT>(PlayerPedPtr + 0x2D4);
			FLOAT playerCoords2 = Read<FLOAT>(PlayerPedPtr + 0x2D4 + 0x4);
			FLOAT playerCoords3 = Read<FLOAT>(PlayerPedPtr + 0x2D4 + 0x8);

			for (int i = 0; i < 90; i++)
			{
				write<FLOAT>(ZM_Bot_List_Offset + (0x5F8 * i) + 0x2D4, playerCoords1 + 0x100);
				write<FLOAT>(ZM_Bot_List_Offset + (0x5F8 * i) + 0x2D4 + 0x4, playerCoords2);
				write<FLOAT>(ZM_Bot_List_Offset + (0x5F8 * i) + 0x2D4 + 0x8, playerCoords3);
				write<INT>(ZM_Bot_List_Offset + (0x5F8 * i) + 0x398, 20);
				write<INT>(ZM_Bot_List_Offset + (0x5F8 * i) + 0x39C, 20);

			}
			if (GetAsyncKeyState(VK_F9))
			{
				tp = !tp;
			}

		}


	}
}
void runcheat()
{
	printf("[Rubery-Kernel]: Looking for BlackOpsColdWar.exe process...\n");
	process_id = get_process_id("BlackOpsColdWar.exe");

	if (process_id) {
		printf("[Rubery-Kernel]: Process identifier found: %d\n", process_id);
		printf("[Rubery-Kernel]: Looking for base address...\n");

		base_address = get_client_base("BlackOpsColdWar.exe");
		if (base_address)
		{
			printf("[Rubery-Kernel]: Base address found: %d\n", process_id);

			for (;;)
			{

				money();
				godmode();
				raygun();
				xpgun();
				tp1hp();
			}



		}
		else
		{
			printf("[Rubery-Kernel]: Failed to find base address\n");

		}
	}
	else
	{
		printf("[Rubery-Kernel]: Failed to find BlackOpsColdWar.exe process\n");

	}
}

int main()
{
	printf("[Rubery-Kernel]: Loading modules...\n");
	LoadLibraryA("user32.dll");
	LoadLibraryA("win32u.dll");
	if (GetModuleHandleA("win32u.dll"))
	{
		printf("[Rubery-Kernel]: Modules Loaded\n");
		runcheat();

	}
	else
	{
		printf("[Rubery-Kernel]: Failed to find modules\n");

	}
	return 0;

}