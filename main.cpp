#include <windows.h>
#include <fstream>
#include <cmath>
#include "nya_commonhooklib.h"

class Camera {
public:
	uint8_t _0[0xF0];
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
	uint8_t _100[0xC];
	float fNearZ;
	float fFarZ;
	float fFOV;
};

class CameraExtents {
public:
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
};

float f219_21 = 21.0;
float f219_9 = 9.0;
uintptr_t Aspect219ASM_jmp = 0x45747F;
void __attribute__((naked)) __fastcall Aspect219ASM() {
	__asm__ (
		"fstp st(1)\n\t"
		"fstp st\n\t"
		"fld dword ptr %1\n\t"
		"fstp dword ptr [esp+0x10]\n\t"
		"fld dword ptr %2\n\t"
		"fstp dword ptr [esp+0x14]\n\t"
		"jmp %0\n\t"
			:
			: "m" (Aspect219ASM_jmp), "m" (f219_21), "m" (f219_9)
	);
}

float f329_21 = 32.0;
float f329_9 = 9.0;
uintptr_t Aspect329ASM_jmp = 0x45747F;
void __attribute__((naked)) __fastcall Aspect329ASM() {
	__asm__ (
		"fstp st(1)\n\t"
		"fstp st\n\t"
		"fld dword ptr %1\n\t"
		"fstp dword ptr [esp+0x10]\n\t"
		"fld dword ptr %2\n\t"
		"fstp dword ptr [esp+0x14]\n\t"
		"jmp %0\n\t"
			:
			: "m" (Aspect329ASM_jmp), "m" (f329_21), "m" (f329_9)
	);
}

LRESULT __stdcall SendDlgItemMessageAHooked(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (nIDDlgItem == 1124 && Msg == 0x14E) {
		auto tmp = SendDlgItemMessageA(hDlg, 1124, 0x143u, 0, (LPARAM)"21:9");
		SendDlgItemMessageA(hDlg, 1124, 0x151u, tmp, 4);
		tmp = SendDlgItemMessageA(hDlg, 1124, 0x143u, 0, (LPARAM)"32:9");
		SendDlgItemMessageA(hDlg, 1124, 0x151u, tmp, 5);
	}
	return SendDlgItemMessageA(hDlg, nIDDlgItem, Msg, wParam, lParam);
}
auto pSendDlgItemMessageAHooked = &SendDlgItemMessageAHooked;

int GetAspectRatioForSetup(int x, int y) {
	auto aspect = (double)x / (double)y;
	if (std::abs(aspect - (16.0 / 10.0)) < 0.05) {
		return 1;
	}
	if (std::abs(aspect - (16.0 / 9.0)) < 0.05) {
		return 2;
	}
	if (std::abs(aspect - (5.0 / 4.0)) < 0.05) {
		return 3;
	}
	if (std::abs(aspect - (21.0 / 9.0)) < 0.05) {
		return 4;
	}
	if (std::abs(aspect - (32.0 / 9.0)) < 0.05) {
		return 5;
	}
	return 0; // 4:3
}

double fAspectRatio = 16.0 / 9.0;
void RecalculateAspectRatio() {
	fAspectRatio = *(float*)0x724BB4 / *(float*)0x724BB8;
}

double fOrigAspect = 16.0 / 9.0;
void __fastcall UltrawideFOV(Camera* pCam) {
	RecalculateAspectRatio();

	auto mult = 1.0 / fOrigAspect;
	auto v12 = tan(pCam->fFOV * 0.5) * pCam->fNearZ;
	pCam->fRight = v12 * fAspectRatio * mult;
	pCam->fLeft = -v12 * fAspectRatio * mult;
	pCam->fBottom = v12 * mult;
	pCam->fTop = -v12 * mult;
}

uintptr_t UltrawideFOVASM_jmp = 0x4F44FC;
void __attribute__((naked)) __fastcall UltrawideFOVASM() {
	__asm__ (
		"fstp dword ptr [esi+0x104]\n\t"
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"sub dword ptr [esp+0x1C], 1\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVASM_jmp), "i" (UltrawideFOV)
	);
}

void __fastcall UltrawideFOVSky(CameraExtents* pCameraExtents, Camera* pCamera) {
	auto mult = 1.0 / fOrigAspect;
	auto v12 = tan(pCamera->fFOV * 0.5);
	pCameraExtents->fRight = v12 * fAspectRatio * mult;
	pCameraExtents->fLeft = -v12 * fAspectRatio * mult;
	pCameraExtents->fBottom = v12 * mult;
	pCameraExtents->fTop = -v12 * mult;
}

uintptr_t UltrawideFOVSkyASM_jmp = 0x591FAD;
void __attribute__((naked)) __fastcall UltrawideFOVSkyASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"mov edx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, [edi]\n"
		"mov edx, [eax+0xD0]\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVSkyASM_jmp), "i" (UltrawideFOVSky)
	);
}

uintptr_t UltrawideJumpTable[] = {
		0x457429,
		0x457435,
		0x45744F,
		0x457469,
		(uintptr_t)&Aspect219ASM,
		(uintptr_t)&Aspect329ASM,
};

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

			NyaHookLib::Patch(0x45642F, &pSendDlgItemMessageAHooked);
			NyaHookLib::Patch<uint8_t>(0x45741D + 2, 5);
			NyaHookLib::Patch(0x457422 + 3, &UltrawideJumpTable);

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x455C80, &GetAspectRatioForSetup);

			NyaHookLib::Fill(0x4F44A1, 0x90, 0x4F44A6 - 0x4F44A1); // remove sub instruction
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4F44F6, &UltrawideFOVASM);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x591FA5, &UltrawideFOVSkyASM);
		} break;
		default:
			break;
	}
	return TRUE;
}