#pragma once

#define WIN32_LEAN_AND_MEAN

#include <array>
#include <string>
#include <vector>
#include <d3d11.h>
#include <algorithm>
#include <Windows.h>
#include <typeindex>
#include <directxmath.h>

#include "Time.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;
using namespace DirectX;

typedef XMINT2 int2;
typedef XMINT3 int3;
typedef XMINT4 int4;

typedef XMFLOAT2 float2;
typedef XMFLOAT3 float3;
typedef XMFLOAT4 float4;
typedef XMFLOAT4X4 float4x4;

typedef unsigned int uint; // HLSL has ruined me
typedef XMUINT2 uint2;
typedef XMUINT3 uint3;
typedef XMUINT4 uint4;

#define OUTPUT(x) OutputDebugStringA(std::string(x).c_str()) // Remember to use newline
#define DX_NAME(variable, name) (variable)->SetPrivateData(WKPDID_D3DDebugObjectName, (uint)string(name).size(), string(name).c_str())

#define SAFE_DELETE(x) { if((x)) { delete (x); (x) = nullptr; } }
#define SAFE_RELEASE(x) { if((x)) { (x)->Release(); (x) = nullptr; } }

#define DEGREES(fRadians) ((180.0f / 3.14f) * fRadians)
#define RADIANS(fDegrees) ((3.14f / 180.0f) * fDegrees)

#define FOV gFoV
#define SENSITIVITY gSensitivity

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

#define SCREEN_WIDTH gScreenWidth
#define SCREEN_HEIGHT gScreenHeight

#define WINDOW_WIDTH gWindowWidth
#define WINDOW_HEIGHT gWindowHeight
#define RATIO ((float)gWindowWidth / (float)gWindowHeight)

#define WINDOW_START_X ((gScreenWidth - gWindowWidth) / 2)
#define WINDOW_START_Y ((gScreenHeight - gWindowHeight) / 2)

#define WINDOW_STYLE (WS_POPUP | WS_BORDER)

#pragma region Array Macros

#define ARRAY_DELETE(arr, size) { for (uint i = 0; i < (size); i++) { delete (arr)[i]; } }
#define ARRAY_RELEASE(arr, size) { for (uint i = 0; i < (size); i++) { SAFE_RELEASE((arr)[i]); } }

#define VECTOR_FIND(vector, x, i) (find((vector).begin(), (vector).end(), x) - (vector).begin() < (vector).size()) // Checks if an object exists
#define VECTOR_INDEX(vector, x) find((vector).begin(), (vector).end(), x) - (vector).begin() // Grabs index of an object
#define VECTOR_FIND_AND_INDEX(vector, x, i) ((i = (uint)(find((vector).begin(), (vector).end(), x) - (vector).begin())) < (vector).size()) // Checks if an object exists and grabs the index
#define VECTOR_REMOVE(vector, x) { auto iter = (vector).begin(); auto end = (vector).end(); while (iter != end) { if (*iter == (x)) { (vector).erase(iter); break; } iter++; } }
#define VECTOR_DELETE(vector, x) { auto iter = (vector).begin(); auto end = (vector).end(); while (iter != end) { if (*iter == (x)) { SAFE_DELETE(*iter); (x) = nullptr; (vector).erase(iter); break; } iter++; } }
#define VECTOR_RELEASE(vector, x) { auto iter = (vector).begin(); auto end = (vector).end(); while (iter != end) { if (*iter == (x)) { SAFE_RELEASE(*iter); (x) = nullptr; (vector).erase(iter); break; } iter++; } }
#define VECTOR_DELETE_ALL(vector) { auto iter = (vector).begin(); auto end = (vector).end(); while (iter != end) { SAFE_DELETE(*iter); iter++; } (vector).clear(); }
#define VECTOR_RELEASE_ALL(vector) { auto iter = (vector).begin(); auto end = (vector).end(); while (iter != end) { SAFE_RELEASE(*iter); iter++; } (vector).clear(); }

#define MAP_REMOVE(map, x) { auto iter = (map).begin(); auto end = (map).end(); while (iter != end) { if (iter->second == (x)) { (map).erase(iter); break; } iter++; } }
#define MAP_DELETE(map, x) { auto iter = (map).begin(); auto end = (map).end(); while (iter != end) { if (iter->second == (x)) { SAFE_DELETE(iter->second); (x) = nullptr; (map).erase(iter); break; } iter++; } }
#define MAP_RELEASE(map, x) { auto iter = (map).begin(); auto end = (map).end(); while (iter != end) { if (iter->second == (x)) { SAFE_RELEASE(iter->second); (x) = nullptr; (map).erase(iter); break; } iter++; } }
#define MAP_DELETE_ALL(map) { auto iter = (map).begin(); auto end = (map).end(); while (iter != end) { SAFE_DELETE(iter->second); iter++; } (map).clear(); }
#define MAP_RELEASE_ALL(map) { auto iter = (map).begin(); auto end = (map).end(); while (iter != end) { SAFE_RELEASE(iter->second); iter++; } (map).clear(); }

#pragma endregion // Because I hate how ugly iteration loops look

// - Global Declarations - //
extern HWND gWindow;

extern float gFoV;
extern float gSensitivity;

extern int gScreenWidth;
extern int gScreenHeight;

extern int gWindowWidth;
extern int gWindowHeight;

extern const void* gTime;
extern const void* gGraphics;

// Global Accessors //
#define TICK ((CTime*)gTime)->Tick()
#define DELTA_TIME ((CTime*)gTime)->Delta()
#define CURRENT_TIME ((CTime*)gTime)->Current()

#define GRAPHICS ((Renderer*)gGraphics)

// Shader Forward Declarations //
template <typename T> class Shader;
typedef Shader<ID3D11VertexShader> VertexShader;
typedef Shader<ID3D11PixelShader> PixelShader;
typedef Shader<ID3D11HullShader> HullShader;
typedef Shader<ID3D11DomainShader> DomainShader;
typedef Shader<ID3D11GeometryShader> GeometryShader;
typedef Shader<ID3D11ComputeShader> ComputeShader;