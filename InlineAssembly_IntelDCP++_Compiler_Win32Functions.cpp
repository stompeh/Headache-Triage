// December 9th, 2022 -- Intel oneAPI DPC++/C++ Compiler & Intel C++ Compiler Classic, v. 2022.2.1
// MSVC does not support x64 inline assembly. One solution is to use the Intel C++ 2022 compiler, which is compatible with Visual Studio.
/* Note the oneAPI DPC++ compiler will throw an error "unsupported architecture spir64 for MS-Style inline assembly" */


// The original way of using the Win32 call, and how it looks in a disassembler
const wchar_t boopy[] = L"Boopy";
const wchar_t zoops[] = L"Zoops";
MessageBoxExW(NULL, boopy, zoops, MB_OK, 0);

lea     r8,[zoops]  
lea     rdx,[boopy]  
xor     eax,eax  
mov     ecx,eax  
xor     r9d,r9d  
mov     dword ptr [rsp+20h],0  
call    qword ptr [__imp_MessageBoxExW (07FF72A5013A0h)]
//==============================================================================



// __imp_MessageBoxExW isn't a symbol Intel C++ 2022 recognizes in source, but it does recognize regular Win32 function names.
// Even if using "call qword ptr MessageBoxExW", the compiler will still output a similar result moving a pointer to the address into r10
// I'm not an expert. I think the call tries to access the address of r10 which triggers a read at xFFFFFFFFFFFFFFFF 
// which throws a read violation of kernel memory...or something.
// Again this is only my best guess at this time.
void ReadAccessViolation()
{
    const wchar_t boopy[] = L"Boopy";
	const wchar_t zoops[] = L"Zoops";
    
    __asm
	{
		lea r8, [zoops]
		lea rdx, [boopy]
		xor eax, eax
		mov ecx, eax
		xor r9d, r9d
		mov dword ptr[rsp + 0x20], 0x0
		call MessageBoxExW
	}
}

mov         r10,qword ptr [__imp_MessageBoxExW (07FF66A5613A0h)]  
lea         r8,[zoops]  
lea         rdx,[boopy]  
xor         eax,eax  
mov         ecx,eax  
xor         r9d,r9d  
mov         dword ptr [rsp+20h],0  
call        qword ptr [r10]
//==============================================================================



// This workaround is simply calling r10...or even r11. It's one extra instruction but at least it's working.
// Perhaps this is how it was intended. I also made the mistake of using "mov" instead of "lea" when initially trying to load MessageBox into a register.
//Either way I'm glad it's finally over.
void FinalWorkingResult()
{
    const wchar_t boopy[] = L"Boopy";
	const wchar_t zoops[] = L"Zoops";
  
    __asm
	{
		lea r10, MessageBoxExW
		lea r8, [zoops]
		lea rdx, [boopy]
		xor eax, eax
		mov ecx, eax
		xor r9d, r9d
		mov dword ptr[rsp + 0x20], 0x0
		call r10
	}
    
    /* When compiled (Visual Studio disassembler):
     mov         r11,qword ptr [__imp_MessageBoxExW (07FF6C38713A0h)]  
     lea         r10,[r11]  
     lea         r8,[zoops]  
     lea         rdx,[boopy]  
     xor         eax,eax  
     mov         ecx,eax  
     xor         r9d,r9d  
     mov         dword ptr [rsp+20h],0  
     call        r10
    */
}
