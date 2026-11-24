.code
__chkstk PROC
 push rcx
 push rax
 lea rcx,[rsp+24]
 cmp rax,4096
 jb last_page
probe_page:
 sub rcx,4096
 test byte ptr [rcx],0
 sub rax,4096
 cmp rax,4096
 jae probe_page
last_page:
 sub rcx,rax
 test byte ptr [rcx],0
 pop rax
 pop rcx
 ret
__chkstk ENDP
END
