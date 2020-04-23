//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  File:       D3DCompiler.h
//  Content:    D3D Compilation Types and APIs
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __D3DCOMPILER_H__
#define __D3DCOMPILER_H__

// Current name of the DLL shipped in the same SDK as this header.


#define D3DCOMPILER_DLL_W L"d3dcompiler_42.dll"
#define D3DCOMPILER_DLL_A "d3dcompiler_42.dll"

#ifdef UNICODE
    #define D3DCOMPILER_DLL D3DCOMPILER_DLL_W 
#else
    #define D3DCOMPILER_DLL D3DCOMPILER_DLL_A
#endif

#include "d3d11shader.h"

// The compilation APIs are generally not tied to particular
// versions of D3D, thus their names and types don't reflect
// a particular version number unless it is necessary to
// do so.  However, for compatibility with previous compilation
// API definitions some of the neutral types are simply
// redefinitions of older types.
typedef struct _D3D10_SHADER_MACRO D3D_SHADER_MACRO;
typedef enum _D3D10_INCLUDE_TYPE D3D_INCLUDE_TYPE;
typedef interface ID3D10Blob ID3DBlob;
typedef ID3DBlob* LPD3DBLOB;
typedef interface ID3D10Include ID3DInclude;
typedef ID3DInclude* LPD3DINCLUDE;

//////////////////////////////////////////////////////////////////////////////
// APIs //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//----------------------------------------------------------------------------
// D3DCompile:
// ----------
// Compile source text into bytecode appropriate for the given target.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DCompile(__in_bcount(SrcDataSize) LPCVOID pSrcData,
           __in SIZE_T SrcDataSize,
           __in_opt LPCSTR pSourceName,
           __in_xcount_opt(pDefines->Name != NULL) CONST D3D_SHADER_MACRO* pDefines,
           __in_opt LPD3DINCLUDE pInclude,
           __in LPCSTR pEntrypoint,
           __in LPCSTR pTarget,
           __in UINT Flags1,
           __in UINT Flags2,
           __out LPD3DBLOB* ppCode,
           __out_opt LPD3DBLOB* ppErrorMsgs);

typedef HRESULT (WINAPI *pD3DCompile)
    (LPCVOID                         pSrcData,
     SIZE_T                          SrcDataSize,
     LPCSTR                          pFileName,
     CONST D3D10_SHADER_MACRO*       pDefines,
     LPD3D10INCLUDE                  pInclude,
     LPCSTR                          pEntrypoint,
     LPCSTR                          pTarget,
     UINT                            Flags1,
     UINT                            Flags2,
     LPD3D10BLOB*                    ppCode,
     LPD3D10BLOB*                    ppErrorMsgs);
     
//----------------------------------------------------------------------------
// D3DPreprocess:
// ----------
// Process source text with the compiler's preprocessor and return
// the resulting text.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DPreprocess(__in_bcount(SrcDataSize) LPCVOID pSrcData,
              __in SIZE_T SrcDataSize,
              __in_opt LPCSTR pSourceName,
              __in_opt CONST D3D_SHADER_MACRO* pDefines,
              __in_opt LPD3DINCLUDE pInclude,
              __out LPD3DBLOB* ppCodeText,
              __out_opt LPD3DBLOB* ppErrorMsgs);

typedef HRESULT (WINAPI *pD3DPreprocess)
    (LPCVOID                      pSrcData,
     SIZE_T                       SrcDataSize,
     LPCSTR                       pFileName,
     CONST D3D10_SHADER_MACRO*    pDefines,
     LPD3D10INCLUDE               pInclude,
     LPD3D10BLOB*                 ppCodeText,
     LPD3D10BLOB*                 ppErrorMsgs);

//----------------------------------------------------------------------------
// D3DGetDebugInfo:
// -----------------------
// Gets shader debug info.  Debug info is generated by D3DCompile and is
// embedded in the body of the shader.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DGetDebugInfo(__in_bcount(SrcDataSize) LPCVOID pSrcData,
                __in SIZE_T SrcDataSize,
                __out LPD3DBLOB* ppDebugInfo);

//----------------------------------------------------------------------------
// D3DReflect:
// ----------
// Shader code contains metadata that can be inspected via the
// reflection APIs.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DReflect(__in_bcount(SrcDataSize) LPCVOID pSrcData,
           __in SIZE_T SrcDataSize,
	   __in REFIID pInterface,
           __out void** ppReflector);

//----------------------------------------------------------------------------
// D3DDisassemble:
// ----------------------
// Takes a binary shader and returns a buffer containing text assembly.
//----------------------------------------------------------------------------

#define D3D_DISASM_ENABLE_COLOR_CODE            0x00000001
#define D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS  0x00000002
#define D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING 0x00000004
#define D3D_DISASM_ENABLE_INSTRUCTION_CYCLE     0x00000008

HRESULT WINAPI 
D3DDisassemble(__in_bcount(SrcDataSize) LPCVOID pSrcData,
               __in SIZE_T SrcDataSize,
               __in UINT Flags,
               __in_opt LPCSTR szComments,
               __out LPD3DBLOB* ppDisassembly);

typedef HRESULT (WINAPI *pD3DDisassemble)
    (__in_bcount(SrcDataSize) LPCVOID pSrcData,
     __in SIZE_T SrcDataSize,
     __in UINT Flags,
     __in_opt LPCSTR szComments,
     __out LPD3DBLOB* ppDisassembly);

//----------------------------------------------------------------------------
// D3DDisassemble10Effect:
// -----------------------
// Takes a D3D10 effect interface and returns a
// buffer containing text assembly.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DDisassemble10Effect(__in ID3D10Effect *pEffect, 
                       __in UINT Flags,
                       __out LPD3DBLOB* ppDisassembly);

//----------------------------------------------------------------------------
// D3DGetInputSignatureBlob:
// -----------------------
// Retrieve the input signature from a compilation result.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DGetInputSignatureBlob(__in_bcount(SrcDataSize) LPCVOID pSrcData,
                         __in SIZE_T SrcDataSize,
                         __out LPD3DBLOB* ppSignatureBlob);

//----------------------------------------------------------------------------
// D3DGetOutputSignatureBlob:
// -----------------------
// Retrieve the output signature from a compilation result.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DGetOutputSignatureBlob(__in_bcount(SrcDataSize) LPCVOID pSrcData,
                          __in SIZE_T SrcDataSize,
                          __out LPD3DBLOB* ppSignatureBlob);

//----------------------------------------------------------------------------
// D3DGetInputAndOutputSignatureBlob:
// -----------------------
// Retrieve the input and output signatures from a compilation result.
//----------------------------------------------------------------------------

HRESULT WINAPI
D3DGetInputAndOutputSignatureBlob(__in_bcount(SrcDataSize) LPCVOID pSrcData,
                                  __in SIZE_T SrcDataSize,
                                  __out LPD3DBLOB* ppSignatureBlob);

//----------------------------------------------------------------------------
// D3DStripShader:
// -----------------------
// Removes unwanted blobs from a compilation result
//----------------------------------------------------------------------------

typedef enum D3DCOMPILER_STRIP_FLAGS
{
    D3DCOMPILER_STRIP_REFLECTION_DATA = 1,
    D3DCOMPILER_STRIP_DEBUG_INFO      = 2,
    D3DCOMPILER_STRIP_TEST_BLOBS      = 4,
    D3DCOMPILER_STRIP_FORCE_DWORD     = 0x7fffffff,
} D3DCOMPILER_STRIP_FLAGS;

HRESULT WINAPI
D3DStripShader(__in_bcount(BytecodeLength) LPCVOID pShaderBytecode,
               __in SIZE_T BytecodeLength,
               __in UINT uStripFlags,
               __out LPD3DBLOB* ppStrippedBlob);

#ifdef __cplusplus
}
#endif //__cplusplus
    
#endif // #ifndef __D3DCOMPILER_H__
