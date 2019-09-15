// dllmain.cpp : Defines the entry point for the DLL application.

#define LOWORD(_dw)     ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#define HIWORD(_dw)     ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#define LODWORD(_qw)    ((INT)(_qw))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))

#include "menu.h"
#include "ManhuntSdk.h"
#include <stdarg.h>
#include <algorithm>


struct CConfig
{
	int headshot = 0;
	int bloodStay = 0;
	int firearm3 = 0;
	int flash = 0;
	int cameraShake = 0;
	int funmode = 0;
	int tvpCamera = 0;
	int debug = 0;
	int whitenoise = 0;
};

void DummyReturnVoid() { return; }

CConfig config;

void ResetConfig() {
	config.headshot = 0;
	config.bloodStay = 0;
	config.firearm3 = 0;
	config.flash = 0;
	config.cameraShake = 0;
	config.funmode = 0;
	config.tvpCamera = 0;
	config.debug = 0;
	config.whitenoise = 0;
}

void UpdateWhitenoise() {
	if (config.whitenoise == 1) {
		//disable whitenoise
		Memory::VP::InjectHook(0x5C5CED, DummyReturnVoid, PATCH_CALL);

		Memory::VP::Patch<char>(0x5AF74F + 1, 0x00);
		Memory::VP::Patch<char>(0x5AF734 + 1, 0x00);


	}
	else {
		Memory::VP::Patch<char>(0x5C5CED, 0xE8);
		Memory::VP::Patch<char>(0x5C5CED + 1, 0x9E);
		Memory::VP::Patch<char>(0x5C5CED + 2, 0x9A);
		Memory::VP::Patch<char>(0x5C5CED + 3, 0xFE);
		Memory::VP::Patch<char>(0x5C5CED + 4, 0xFF);

		Memory::VP::Patch<char>(0x5AF74F + 1, 0x01);
		Memory::VP::Patch<char>(0x5AF734 + 1, 0x01);
	}
}


void UpdateFunMode() {

	if (config.funmode == 1) {
		*(int*)0x76BE40 = 32;
		*(char*)0x6B26E5 = 0;
	}
	else {
		*(int*)0x76BE40 = 0;
		*(char*)0x6B26E5 = 1;
	}
}

void UpdateCameraShake() {
	if (config.cameraShake == 1) {
		Memory::VP::Patch<char>(0x5956CA + 1, 0x85);
	}
	else {
		Memory::VP::Patch<char>(0x5956CA + 1, 0x84);
	}
}

void UpdateFlash() {
	int colRampName = *(int*)0x75C0A8;
	colRampName = colRampName + 0x208;

	if (config.flash == 1) {
		//break the colramp name so he can not load it anymore
		Memory::VP::Patch<char>(colRampName + 2, 0x00);

	}
	else {
		//restore the colramp name so he can not load it anymore
		Memory::VP::Patch<char>(colRampName + 2, 0x5f);
	}
}

void UpdateBloodStay() {
	if (config.bloodStay == 1) {
		Memory::VP::Patch<char>(0x5E4C10, 0x74);
	}
	else {
		Memory::VP::Patch<char>(0x5E4C10, 0x75);
	}

}


void SaveSettings() {
	std::ofstream myfile;
	myfile.open("settings.umh");
	myfile << "headshot=" << config.headshot << "\n";
	myfile << "bloodStay=" << config.bloodStay << "\n";
	myfile << "firearm3=" << config.firearm3 << "\n";
	myfile << "flash=" << config.flash << "\n";
	myfile << "cameraShake=" << config.cameraShake << "\n";
	myfile << "funmode=" << config.funmode << "\n";
	myfile << "tvpCamera=" << config.tvpCamera << "\n";
	myfile << "debug=" << config.debug << "\n";
	myfile << "whitenoise=" << config.whitenoise << "\n";
	myfile.close();
}

bool settingsLoaded = false;
bool settingsFlashLoaded = false;
void ReadSettings() {
	std::ifstream fin("settings.umh");
	std::string line;
	while (std::getline(fin, line)) {

		std::string stdAttribute = line.substr(0, line.find("="));
		std::string stdValue = line.substr(line.find("=") + 1);

		char* attr = new char[stdAttribute.length() + 1];
		strcpy(attr, stdAttribute.c_str());

		char* value = new char[stdValue.length() + 1];
		strcpy(value, stdValue.c_str());

		if (strcmp(attr, "headshot") == 0) {
			config.headshot = atoi(value);
		}
		else if (strcmp(attr, "debug") == 0) {
			config.debug = atoi(value);
		}
		else if (strcmp(attr, "bloodStay") == 0) {
			config.bloodStay = atoi(value);
		}
		else if (strcmp(attr, "firearm3") == 0) {
			config.firearm3 = atoi(value);
		}
		else if (strcmp(attr, "cameraShake") == 0) {
			config.cameraShake = atoi(value);
		}
		else if (strcmp(attr, "funmode") == 0) {
			config.funmode = atoi(value);
		}
		else if (strcmp(attr, "whitenoise") == 0) {
			config.whitenoise = atoi(value);
		}
		else if (strcmp(attr, "flash") == 0) {
			config.flash = atoi(value);
		}
	}
}

void ApplySettings() {
	UpdateBloodStay();
	UpdateCameraShake();
	UpdateFunMode();
	UpdateWhitenoise();
//	UpdateFlash();
}


void Init()
{
	CMenu* menu = new CMenu();

	while (true)
	{
		Sleep(1);
		menu->ProcessMenu();

		//we need to wait in some cases until the level is loaded before we can restore previous settings
		if (*(CSDKEntity * *)0x789490 != false && settingsFlashLoaded == false) {

			UpdateFlash();
			settingsFlashLoaded = true;
		}

	}
}

signed int __fastcall OverwriteGetHardcodedExecutoinLevel(int ptr)
{
	int v1; // esi@1
	signed int result; // eax@2
	int v3; // eax@4
	int v4; // edi@4

	v1 = ptr;
	if (*(char*)(ptr + 4359))
	{
		result = *(int*)(ptr + 4372);
		if (*(int*)(ptr + 4372) >= 2) {
			return 2;
		}
	}
	else
	{
		v3 = *(int*)(ptr + 1160);
		v4 = *(int*)(ptr + 108);


		//this prevent the firearm to have 3 execution stages
		if (config.firearm3 == 1) {
			if ((v3 == 4 || v3 == 5) && v3 != 3 || *(int*)(v1 + 4472)) {
				return 2;
			}

		}
		else {
			bool isWeaponFireArm = ((bool(__thiscall*)(int*))0x4F89A0)((int*)ptr);
			if ((v3 == 4 || v3 == 5) && v3 != 3 || isWeaponFireArm || *(int*)(v1 + 4472)) {
				return 2;
			}
		}

		if (*(float*)(v4 + 5480) > (double) * (float*)(v1 + 0x1234)) {
			return 0;
		}

		result = 1;

		if (*(float*)(v4 + 5484) <= (double) * (float*)(v1 + 4660)) {
			result = 2;
		}
	}

	return result;
}


int explodeHead_ebx;
int explodeHead_unk;
int explodeHead_true = 0x5097FC;
int explodeHead_false = 0x5097C1;

void __declspec(naked) HookExtraWeaponCheck() {
	_asm {
		mov explodeHead_ebx, ebx
		mov explodeHead_unk, eax
		pushad
	}
//	if (explodeHead_ebx == CT_SAWNOFF || explodeHead_ebx == CT_SHOTGUN || explodeHead_ebx == CT_SHOTGUN_TORCH)
	
	if (config.headshot == 1) {
		if (explodeHead_ebx == 52 || explodeHead_ebx == 46 || explodeHead_ebx == 47)
		{
			if (explodeHead_unk != *(char*)0x69939C)
			{
				// decrease headshot distance for shotguns
				Memory::VP::Patch<float>(0x6991C0, 10.0);
				_asm
				{
					popad
					jmp explodeHead_true
				}
			}

		}
		else
		{
			// increase headshot distance for everything else
			Memory::VP::Patch<float>(0x6991C0, 100.0);
			_asm {
				popad
				jmp  explodeHead_false
			}
		}
	}
	else {
		_asm
		{
			popad
			jmp explodeHead_true
		}

	}


}


int endCase = 0x55F31D;
int command;
char* commandText;

void __cdecl HandleNeoMenuChangeLanguageLoadMap(int a1, int a2) {

	char* cmd = (char*)(*(int*)a2);

	((void(__cdecl*)(int, int))0x55F1A0)(a1, a2);
}


void __cdecl HandleNeoMenuCommands(int a1, int a2) {

	char* cmd = (char*)(*(int*)a2);
	
//	printf("HandleNeoMenuCommands %s\n", cmd);

	bool isCustomCmd = false;

	if (strcmp(cmd, "tvpOriginal") == 0) {
		isCustomCmd = true;
		config.tvpCamera = 0;
	}
	else if (strcmp(cmd, "tvpStatic") == 0) {
		isCustomCmd = true;
		config.tvpCamera = 1;
	}
	else if (strcmp(cmd, "tvpPs2Leak") == 0) {
		isCustomCmd = true;
		config.tvpCamera = 2;
	}
	else if (strcmp(cmd, "tvpPs2Psp") == 0) {
		isCustomCmd = true;
		config.tvpCamera = 3;
	}
	else if (strcmp(cmd, "tvpWii") == 0) {
		isCustomCmd = true;
		config.tvpCamera = 4;
	}
	else if (strcmp(cmd, "set default settings") == 0) {
		ResetConfig();
	}

	

	if (isCustomCmd) {
		//send apply msg
		((void(__thiscall*)(int*, int*, int))0x562B00)((int*)0x75F110, (int*)(a1 + 120), 1);

		SaveSettings();

		return;
	}


	((void(__cdecl*)(int, int))0x55CC60)(a1, a2);
}




/*
void __fastcall HookNeoMenuSettings( int GameSettingsPtr) {
	CallMethod<0x561C50, int>(GameSettingsPtr);
}
*/


int __fastcall WrapGetNeoMenuValue(int ptr, int a2, int a3) {
	char* fieldName = *(char**)a3;

//	printf("WrapGetNeoMenuValue %s\n", fieldName);

	if (strcmp(fieldName, "headshot") == 0) {
		return config.headshot;
	}
	else if (strcmp(fieldName, "bloodStay") == 0) {
		return config.bloodStay;
	}
	else if (strcmp(fieldName, "firearm3") == 0) {
		return config.firearm3;
	}
	else if (strcmp(fieldName, "cameraShake") == 0) {
		printf("camera shake %i", config.cameraShake);
		return config.cameraShake;
	}
	else if (strcmp(fieldName, "funmode") == 0) {
		return config.funmode;
	}
	else if (strcmp(fieldName, "debug") == 0) {
		return config.debug;
	}
	else if (strcmp(fieldName, "whitenoise") == 0) {
		return config.whitenoise;
	}
	else if (strcmp(fieldName, "flash") == 0) {
		return config.flash;
	}

	return ((int(__thiscall*)(int, int))0x562510)(ptr, a3);
}



int __fastcall UpdateNeoMenuActiveStates(int ptr, int a2, int namePtr, int status) {
	
	char* fieldName = *(char**)namePtr;

	if (strcmp(fieldName, "headshot") == 0) {
		config.headshot = status;
	}
	else if (strcmp(fieldName, "firearm3") == 0) {
		config.firearm3 = status;
	}
	else if (strcmp(fieldName, "funmode") == 0) {
		config.funmode = status;
		UpdateFunMode();
	}
	else if (strcmp(fieldName, "cameraShake") == 0) {
		config.cameraShake = status;
		UpdateCameraShake();
	}
	else if (strcmp(fieldName, "flash") == 0) {
		config.flash = status;
		UpdateFlash();

	}
	else if (strcmp(fieldName, "debug") == 0) {
		config.debug = status;
	}
	else if (strcmp(fieldName, "whitenoise") == 0) {
		config.whitenoise = status;
		UpdateWhitenoise();
	}
	else if (strcmp(fieldName, "bloodStay") == 0) {
		config.bloodStay = status;
		UpdateBloodStay();
	}

	//update our config file, todo: only call this if required
	SaveSettings();

	return ((int(__thiscall*)(int, int, int))0x562630)(ptr, namePtr, status);
}



int __fastcall SetVecPairValues(int input, int a, int data) {
	
	int vecPairs = ((int(__thiscall*)(int, int))0x53B920)(input, data);

	if (config.tvpCamera == 1) {
		*(float*)(data + 36) = 5.5;
	}

	return vecPairs;

}


char* GetReplacementName(char* filename) {


	return filename;
}

int __cdecl WrapReadBinary(char* filename) {


	std::string modFilename = filename;

	std::string modFile = "mods/";
	modFile.append("Ultimate Manhunt 2/");

	//printf("Load: %s\n", filename);


	if (strcmp(filename, "global/ini/resource13.glg") == 0 && config.tvpCamera != 0) {

		if (config.tvpCamera == 1) {
			modFile.append(modFilename);
			//static cam handled inline in SetVecPairValues
		}
		//ps2 leak cams
		else if (config.tvpCamera == 2) {
			modFile.append("global/ini/resource13_ps2leak.glg");
		}
		else if (config.tvpCamera == 3) {
			modFile.append("global/ini/resource13_ps2psp.glg");
		}
		else if (config.tvpCamera == 4) {
			modFile.append("global/ini/resource13_wii.glg");
		}
	}
	else {
		modFile.append(modFilename);

	}
	

	size_t isSavegame = modFilename.find("MH2_00.sav");
	if (isSavegame != std::string::npos) {
		std::string modFileSave = "mods/Ultimate Manhunt 2/MH2_00.sav";

		std::ifstream f(modFileSave);
		if (f.good()) {
//			std::cout << "Load Savegame from mod folder" << std::endl;

			filename = new char[modFileSave.length() + 1];
			strcpy(filename, modFileSave.c_str());;

			return ((int(__cdecl*)(char*))0x53BCC0)(filename);
		}
	}


	// is the wanted file in our mod folder ?
	std::ifstream f(modFile.c_str());
	if (f.good()) {
		printf("Use custom file : %s\n", modFile.c_str());
		//copy the string to char*
		filename = new char[modFile.length() + 1];
		strcpy(filename, modFile.c_str());;
	}

	return ((int(__cdecl*)(char*))0x53BCC0)(filename);
}

FILE __cdecl Wrap2ReadBinary(char* filename, char* mode) {

	std::string modFilename = filename;

	std::string modFile = "mods/";
	modFile.append("Ultimate Manhunt 2/");
	modFile.append(modFilename);

	// is the wanted file in our mod folder ?
	std::ifstream f(modFile.c_str());
	if (f.good()) {
//		std::cout << "Replace file : " << filename << std::endl;

		//copy the string to char*
		filename = new char[modFile.length() + 1];
		strcpy(filename, modFile.c_str());;
	}

	return ((FILE(__cdecl*)(char*, char*, int))0x61B338)(filename, mode, 64);
}


extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{

		ReadSettings();
		ApplySettings();


		if (config.debug) {
			AllocConsole();
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}



		printf("Enable 60 FPS patch ..");
		Memory::VP::Patch(0x40D2A3, 0x412B);
		printf(". OK\n");



		printf("Enable ModLoader ..");
		// Hook another readBinary call
		Memory::VP::InjectHook(0x57B2E5, Wrap2ReadBinary, PATCH_CALL);

		// Hook the readBinary call
		Memory::VP::InjectHook(0x418A1E, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4B0842, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4C3A7C, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4C7F68, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4C8D87, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4CCD10, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4CD186, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4CD245, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4CFEB7, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4E9029, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x4EBB04, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x53BD0B, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x53CC17, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x53EA07, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x56AEE9, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x56E550, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x583EC4, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x586249, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x5BC856, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x5BC86B, WrapReadBinary, PATCH_CALL);
		Memory::VP::InjectHook(0x5DEA76, WrapReadBinary, PATCH_CALL);
		printf(". OK\n");

		/*
			3-Stage firearm executions code
		*/
		//replace the function to get 3 stages for firearm
		printf("Hooking Firearm 3-Stage executions handler ..");
		Memory::VP::InjectHook(0x599840, OverwriteGetHardcodedExecutoinLevel, PATCH_JUMP);

		/* Fix the DIE position, the 3 and 2 anims are not available so reuse 4 */
		//rename BAT_FALL_DIE_POSE_1HFIREARM_3_ANIM to BAT_FALL_DIE_POSE_1HFIREARM_4_ANIM
		Memory::VP::Patch<char>(0x6423B4, 0x34);
		//rename BAT_FALL_DIE_POSE_1HFIREARM_2_ANIM to BAT_FALL_DIE_POSE_1HFIREARM_4_ANIM
		Memory::VP::Patch<char>(0x6423D8, 0x34);

		//rename BAT_FALL_DIE_POSE_2HFIREARM_3_ANIM to BAT_FALL_DIE_POSE_2HFIREARM_4_ANIM
		Memory::VP::Patch<char>(0x642320, 0x34);
		//rename BAT_FALL_DIE_POSE_2HFIREARM_2_ANIM to BAT_FALL_DIE_POSE_2HFIREARM_4_ANIM
		Memory::VP::Patch<char>(0x642344, 0x34);
		printf(". OK\n");



		/*
			Enable headshot for any shooter weapon
			Ermaccer Code (https://github.com/ermaccer/Manhunt2.CleanHeadshots/blob/022ca6c7346e5d4645cff08f50c9da14fe2d0771/source/dllmain.cpp)
		*/

		printf("Hooking headshot handler ..");
		Memory::VP::Patch<char>(0x5097F1, 0xE9);
		Memory::VP::Patch<int>(0x5097F1 + 1, (int)HookExtraWeaponCheck - ((int)0x5097F1 + 5));
		printf(". OK\n");


		printf("Hooking NeoMenu functions ..");
		//Memory::VP::InjectHook(0x560078, HandleNeoMenuChangeLanguageLoadMap, PATCH_CALL);
		Memory::VP::InjectHook(0x560069, HandleNeoMenuCommands, PATCH_CALL);
		
		Memory::VP::InjectHook(0x5570B1, WrapGetNeoMenuValue, PATCH_CALL);
		Memory::VP::InjectHook(0x5571E6, WrapGetNeoMenuValue, PATCH_CALL);
		Memory::VP::InjectHook(0x55DFA0, WrapGetNeoMenuValue, PATCH_CALL);
		Memory::VP::InjectHook(0x55E58E, WrapGetNeoMenuValue, PATCH_CALL);
		Memory::VP::InjectHook(0x5625B6, WrapGetNeoMenuValue, PATCH_CALL);

		Memory::VP::InjectHook(0x55DEAA, UpdateNeoMenuActiveStates, PATCH_CALL);
		Memory::VP::InjectHook(0x55E4B6, UpdateNeoMenuActiveStates, PATCH_CALL);
		Memory::VP::InjectHook(0x55E721, UpdateNeoMenuActiveStates, PATCH_CALL);

		//replace the translation output "Key Missing:" to nothing...
		//we do not translate our keys because of limitations in the gxt / handler so we just remove the prependix
		for (int i = 0; i < 24; i++) {
			Memory::VP::Patch<char>(0x6A9984 + i, 0x20);
		}

		printf(". OK\n");
		
	
		printf("Hooking Timed Vector Pair handler ..");
		Memory::VP::InjectHook(0x5CB3FC, SetVecPairValues, PATCH_CALL);
		Memory::VP::InjectHook(0x5CB445, SetVecPairValues, PATCH_CALL);
		printf(". OK\n");

		printf("Init Debug Menu ..");
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), nullptr, 0, nullptr);
		printf(". OK\n");



		

		//keep the colramp from regular gameplay also when execution runs, is this needed !?
		//Memory::VP::Patch<char>(0x5910FE + 1, 0x03);




		
	}
}


