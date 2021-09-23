/* =============================================================
INTRODUCTION TO GAME PROGRAMMING SE102
SAMPLE 00 - INTRODUCTORY CODE
This sample illustrates how to:
1/ Create a window
2/ Initiate Direct3D 10, DirectX Sprite
3/ Draw a static brick sprite to the screen
4/ Create frame rate independent movements
5/ Some good C++ programming practices
- Use constants whenever possible
- 0 Warnings
6/ Debug using __FILE__ __LINE__
HOW TO INSTALL Microsoft.DXSDK.D3DX
===================================
1) Tools > NuGet package manager > Package Manager Console
2) execute command :  Install-Package Microsoft.DXSDK.D3DX
WARNING: This one file example has a hell LOT of *sinful* programming practices
================================================================ */

#include <windows.h>

#include <d3d10.h>
#include <d3dx10.h>

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

#define WINDOW_CLASS_NAME L"SampleWindow"
#define WINDOW_TITLE L"00 - Intro"
#define WINDOW_ICON_PATH L"brick.ico" 

HWND hWnd = 0;

// Each color is from 0.0f to 1.0f  ( 0/255 to 255/255 ) 
#define BACKGROUND_COLOR D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f)

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MAX_FRAME_RATE 100

ID3D10Device* pD3DDevice = NULL;
IDXGISwapChain* pSwapChain = NULL;
ID3D10RenderTargetView* pRenderTargetView = NULL;

int BackBufferWidth = 0;
int BackBufferHeight = 0;

#define TEXTURE_PATH_BRICK L"brick.png"

#define BRICK_START_X 8.0f
#define BRICK_START_Y 200.0f

#define BRICK_START_VX 0.2f
#define BRICK_START_VY 0.2f

#define BRICK_WIDTH 16.0f
#define BRICK_HEIGHT 16.0f

#define TEXTURE_PATH_BALL L"ball.jpg"

#define BALL_START_X 8.0f
#define BALL_START_Y 200.0f

#define BALL_START_VX 0.2f
#define BALL_START_VY 0.2f

#define BALL_WIDTH 16.0f
#define BALL_HEIGHT 16.0f

ID3D10Texture2D* texBrick = NULL;				// Texture object to store brick image
ID3DX10Sprite* spriteObject = NULL;				// Sprite handling object 

D3DX10_SPRITE spriteBrick;

float brick_x = BRICK_START_X;
float brick_vx = BRICK_START_VX;
float brick_vy = BRICK_START_VY;
float brick_y = BRICK_START_Y;

ID3D10Texture2D* texBall = NULL;				// Texture object to store brick image

D3DX10_SPRITE spriteBall;

float ball_x = BALL_START_X;
float ball_vx = BALL_START_VX;
float ball_vy = BALL_START_VY;
float ball_y = BALL_START_Y;


LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// DEBUG SUPPORT FUNCTIONS //////////////
#define _W(x)  __W(x)
#define __W(x)  L##x

#define VA_PRINTS(s) {				\
		va_list argp;				\
		va_start(argp, fmt);		\
		vswprintf_s(s, fmt, argp);	\
		va_end(argp);				\
}		

void DebugOut(wchar_t* fmt, ...)
{
	wchar_t s[4096];
	VA_PRINTS(s);
	OutputDebugString(s);
}

void DebugOutTitle(wchar_t* fmt, ...)
{
	wchar_t s[1024];
	VA_PRINTS(s);
	SetWindowText(hWnd, s);
}
//////////////////////////////////////////

void InitDirectX(HWND hWnd)
{

	// retrieve client area width & height so that we can create backbuffer height & width accordingly 
	RECT r;
	GetClientRect(hWnd, &r);

	BackBufferWidth = r.right + 1;
	BackBufferHeight = r.bottom + 1;


	// Create & clear the DXGI_SWAP_CHAIN_DESC structure
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Fill in the needed values
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = BackBufferWidth;
	swapChainDesc.BufferDesc.Height = BackBufferHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	// Create the D3D device and the swap chain
	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_REFERENCE,
		NULL,
		0,
		D3D10_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain,
		&pD3DDevice);

	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] D3D10CreateDeviceAndSwapChain has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// Get the back buffer from the swapchain
	ID3D10Texture2D* pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] pSwapChain->GetBuffer has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// create the render target view
	hr = pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);

	// release the back buffer
	pBackBuffer->Release();

	// Make sure the render target view was created successfully
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] CreateRenderTargetView has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// set the render target
	pD3DDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// create and set the viewport
	D3D10_VIEWPORT viewPort;
	viewPort.Width = BackBufferWidth;
	viewPort.Height = BackBufferHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	pD3DDevice->RSSetViewports(1, &viewPort);


	// create the sprite object to handle sprite drawing 
	hr = D3DX10CreateSprite(pD3DDevice, 0, &spriteObject);

	// Make sure the sprite creation was successful
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] D3DX10CreateSprite has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	D3DXMATRIX matProjection;

	// Create the projection matrix using the values in the viewport
	D3DXMatrixOrthoOffCenterLH(&matProjection,
		(float)viewPort.TopLeftX,
		(float)viewPort.Width,
		(float)viewPort.TopLeftY,
		(float)viewPort.Height,
		0.1f,
		10);
	hr = spriteObject->SetProjectionTransform(&matProjection);




	DebugOut((wchar_t*)L"[INFO] InitDirectX has been successful\n");

	return;
}

/*
	Load game resources. In this example, we only load a brick image
*/
void LoadResources()
{
	ID3D10Resource* pD3D10Resource = NULL;

	// Loads the texture into a temporary ID3D10Resource object
	HRESULT hr = D3DX10CreateTextureFromFile(pD3DDevice,
		TEXTURE_PATH_BRICK,
		NULL,
		NULL,
		&pD3D10Resource,
		NULL);

	// Make sure the texture was loaded successfully
	if (FAILED(hr))
	{
		DebugOut((wchar_t*)L"[ERROR] Failed to load texture file: %s \n", TEXTURE_PATH_BRICK);
		return;
	}

	// Translates the ID3D10Resource object into a ID3D10Texture2D object
	pD3D10Resource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&texBrick);
	pD3D10Resource->Release();

	if (!texBrick) {
		DebugOut((wchar_t*)L"[ERROR] Failed to convert from ID3D10Resource to ID3D10Texture2D \n");
		return;
	}

	// Get the texture details
	D3D10_TEXTURE2D_DESC desc;
	texBrick->GetDesc(&desc);

	// Create a shader resource view of the texture
	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;

	// Clear out the shader resource view description structure
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));

	// Set the texture format
	SRVDesc.Format = desc.Format;
	// Set the type of resource
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;

	ID3D10ShaderResourceView* gSpriteTextureRV = NULL;

	pD3DDevice->CreateShaderResourceView(texBrick, &SRVDesc, &gSpriteTextureRV);

	// Set the spriteÅfs shader resource view
	spriteBrick.pTexture = gSpriteTextureRV;

	// top-left location in U,V coords
	spriteBrick.TexCoord.x = 0;
	spriteBrick.TexCoord.y = 0;

	// Determine the texture size in U,V coords
	spriteBrick.TexSize.x = 1.0f;
	spriteBrick.TexSize.y = 1.0f;

	// Set the texture index. Single textures will use 0
	spriteBrick.TextureIndex = 0;

	// The color to apply to this sprite, full color applies white.
	spriteBrick.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


	DebugOut((wchar_t*)L"[INFO] Texture loaded Ok: %s \n", TEXTURE_PATH_BRICK);

	//Ball init
	// Loads the texture into a temporary ID3D10Resource object
	HRESULT hr2 = D3DX10CreateTextureFromFile(pD3DDevice,
		TEXTURE_PATH_BALL,
		NULL,
		NULL,
		&pD3D10Resource,
		NULL);

	// Make sure the texture was loaded successfully
	if (FAILED(hr2))
	{
		DebugOut((wchar_t*)L"[ERROR] Failed to load texture file: %s \n", TEXTURE_PATH_BALL);
		return;
	}

	// Translates the ID3D10Resource object into a ID3D10Texture2D object
	pD3D10Resource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&texBall);
	pD3D10Resource->Release();

	if (!texBall) {
		DebugOut((wchar_t*)L"[ERROR] Failed to convert from ID3D10Resource to ID3D10Texture2D \n");
		return;
	}

	// Get the texture details
	D3D10_TEXTURE2D_DESC desc2;
	texBall->GetDesc(&desc2);

	// Create a shader resource view of the texture
	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc2;

	// Clear out the shader resource view description structure
	ZeroMemory(&SRVDesc2, sizeof(SRVDesc2));

	// Set the texture format
	SRVDesc2.Format = desc2.Format;
	// Set the type of resource
	SRVDesc2.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	SRVDesc2.Texture2D.MipLevels = desc2.MipLevels;

	ID3D10ShaderResourceView* gSpriteTextureRV2 = NULL;

	pD3DDevice->CreateShaderResourceView(texBall, &SRVDesc2, &gSpriteTextureRV2);

	// Set the spriteÅfs shader resource view
	spriteBall.pTexture = gSpriteTextureRV2;

	// top-left location in U,V coords
	spriteBall.TexCoord.x = 0;
	spriteBall.TexCoord.y = 0;

	// Determine the texture size in U,V coords
	spriteBall.TexSize.x = 1.0f;
	spriteBall.TexSize.y = 1.0f;

	// Set the texture index. Single textures will use 0
	spriteBall.TextureIndex = 0;

	// The color to apply to this sprite, full color applies white.
	spriteBall.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


	DebugOut((wchar_t*)L"[INFO] Texture loaded Ok: %s \n", TEXTURE_PATH_BALL);
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
	IMPORTANT: no render-related code should be used inside this function.
*/
void Update(DWORD dt)
{
	//Uncomment the whole function to see the brick moves and bounces back when hitting left and right edges
	//brick_x++;

	brick_x += brick_vx * dt;
	brick_y += brick_vy * dt;

	if (brick_x <= 0 || brick_x >= BackBufferWidth - BRICK_WIDTH) {

		brick_vx = -brick_vx;


		//	//Why not having these logics would make the brick disappear sometimes?  
		// Because if brick_x < 0 (brick touches left) then brick_vx (move left) = -brick_vx (try move right)
		// but sometimes (brick_x + brick_vx) < 0 (try move right but not enough to make brick_x > 0)
		// brick_x is still out of screen on the left, but since brick_x < 0 so brick_vx (move right)= -brick_vx (try move left)
		// it will move to left even more then move right but brick_x + brick_vx < 0 so it will try move left again creating loop
		// brick always is out of screen
		////	if (brick_x <= 0)
		////	{
		////		brick_x = 0;
		////	}
		////	else if (brick_x >= BackBufferWidth - BRICK_WIDTH)
		////	{
		////		brick_x = BackBufferWidth - BRICK_WIDTH;
		////	}
	}
	if (brick_y <= 0 || brick_y >= BackBufferHeight - BRICK_HEIGHT) {

		brick_vy = -brick_vy;
	}
}

/*
	Render a frame
	IMPORTANT: world status must NOT be changed during rendering
*/
void Render()
{
	if (pD3DDevice != NULL)
	{
		// clear the target buffer
		pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);

		// start drawing the sprites
		spriteObject->Begin(D3DX10_SPRITE_SORT_TEXTURE);

		// The translation matrix to be created
		D3DXMATRIX matTranslation;
		// Create the translation matrix
		D3DXMatrixTranslation(&matTranslation, brick_x, (BackBufferHeight - brick_y), 0.1f);

		// Scale the sprite to its correct width and height
		D3DXMATRIX matScaling;
		D3DXMatrixScaling(&matScaling, BRICK_WIDTH, BRICK_HEIGHT, 1.0f);

		// Setting the spriteÅfs position and size
		spriteBrick.matWorld = (matScaling * matTranslation);

		spriteObject->DrawSpritesImmediate(&spriteBrick, 1, 0, 0);

		//Ball init
		// The translation matrix to be created
		D3DXMATRIX matTranslation2;
		// Create the translation matrix
		D3DXMatrixTranslation(&matTranslation2, ball_x, (BackBufferHeight - ball_y), 0.1f);

		// Scale the sprite to its correct width and height
		D3DXMATRIX matScaling2;
		D3DXMatrixScaling(&matScaling2, BALL_WIDTH, BALL_HEIGHT, 1.0f);

		// Setting the spriteÅfs position and size
		spriteBall.matWorld = (matScaling2 * matTranslation2);

		spriteObject->DrawSpritesImmediate(&spriteBall, 1, 0, 0);

		// Finish up and send the sprites to the hardware
		spriteObject->End();

		//DebugOutTitle((wchar_t*)L"%s (%0.1f,%0.1f) v:%0.1f", WINDOW_TITLE, brick_x, brick_y, brick_vx);

		// display the next item in the swap chain
		pSwapChain->Present(0, 0);
	}
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	//Try this to see how the debug function prints out file and line 
	//wc.hInstance = (HINSTANCE)-100; 

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = (HICON)LoadImage(hInstance, WINDOW_ICON_PATH, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		DWORD ErrCode = GetLastError();
		DebugOut((wchar_t*)L"[ERROR] CreateWindow failed! ErrCode: %d\nAt: %s %d \n", ErrCode, _W(__FILE__), __LINE__);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	ULONGLONG frameStart = GetTickCount64();
	ULONGLONG tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ULONGLONG now = GetTickCount64();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		ULONGLONG dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			Update((DWORD)dt);
			Render();
		}
		else
			Sleep((DWORD)(tickPerFrame - dt));
	}

	return 1;
}

void Cleanup()
{
	// release the rendertarget
	if (pRenderTargetView)
	{
		pRenderTargetView->Release();
	}
	// release the swapchain
	if (pSwapChain)
	{
		pSwapChain->Release();
	}
	// release the D3D Device
	if (pD3DDevice)
	{
		pD3DDevice->Release();
	}

	if (spriteObject)
	{
		spriteObject->Release();
		spriteObject = NULL;
	}

	DebugOut((wchar_t*)L"[INFO] Cleanup Ok\n");
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	hWnd = CreateGameWindow(hInstance, nCmdShow, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (hWnd == 0) return 0;

	InitDirectX(hWnd);

	LoadResources();
	Run();
	Cleanup();

	return 0;
}