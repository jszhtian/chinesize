#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "malie.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x44ddd,"\x75\x81","\xb8\xa1") //quote
};
#undef DP

__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
}

bool IsHalf(const string& s)
{
    for (auto c : s)
    {
        if ((unsigned char)c >= 0x80)
        {
            return false;
        }
    }
    return true;
}

void HOOKFUNC MyCFI(LPLOGFONTW lfi)
{
    //"�ͣ� �����å�"
    if (wcscmp(lfi->lfFaceName, L"MS UI Gothic") == 0)
    {
        wcscpy_s(lfi->lfFaceName, L"SimHei");
    }
}

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"�뤤����������Ů�Υ��ǥ��`") == 0)
    {
        *strp = L"������Ů�����İ�֮���¡����İ� | �aʾ��Ϸ���Ļ���ȤС�� ���� | ����Ⱥ�ţ�153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        InitializeCriticalSection(&g_init_lock);
        CreateThread(0, 0, malie_init_thread, 0, 0, 0);
        //PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        static const HookPointStruct points[] = {
            { nullptr, 0x1bbcec, MyReadStrTbl, "r", false, 0 },
            { nullptr, 0x1bb06f, MyReadVmData, "r", false, 0 },
            { nullptr, 0x1b4500, MyReadDataString, "\x02", false, 0 },
            { nullptr, 0x1b569c, MyReadLibp, "r", false, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook ʧ�ܣ�", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectW", MyCFI, "1", false, 0 },
            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 ʧ�ܣ�", 0, 0);
        }
    }
    return TRUE;
}