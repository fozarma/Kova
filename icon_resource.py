from pathlib import Path
import struct
p=Path(__file__).parent
ico=(p/"Vela.ico").read_bytes()
def record(t,i,data):
    header=struct.pack("<IIHHHHIHHII",len(data),32,65535,t,65535,i,0,0x1030 if t else 0,0,0,0)
    return header+data+b"\0"*((-len(data))%4)
res=record(0,0,b"")
group=ico[:6]
for k in range(struct.unpack_from("<H",ico,4)[0]):
    entry=ico[6+16*k:22+16*k]
    size,offset=struct.unpack_from("<II",entry,8)
    res+=record(3,101+k,ico[offset:offset+size])
    group+=entry[:12]+struct.pack("<H",101+k)
(p/"Vela.res").write_bytes(res+record(14,1,group))
