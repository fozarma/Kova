"""Generate Vela's folded V at every Windows shell icon size."""
import struct
import zlib
from pathlib import Path
root=Path(__file__).parent
def png(size):
    def inside(x,y,points):
        return all((b[0]-a[0])*(y-a[1])-(b[1]-a[1])*(x-a[0])>=0 for a,b in zip(points,points[1:]+points[:1]))
    shapes=[([(17,24),(31,24),(53,68),(44,82)],(243,246,251)),
            ([(72,21),(86,21),(56,82),(50,68)],(231,239,249))]
    raw=bytearray()
    for j in range(size):
        raw.append(0)
        for i in range(size):
            values=[0]*4
            for v in (.25,.75):
                for u in (.25,.75):
                    x,y=100*(i+u)/size,100*(j+v)/size
                    edge=max(23-x,0,x-77)**2+max(23-y,0,y-77)**2
                    if edge>18**2:continue
                    glow=1/(1+((x-82)/46)**2+((y-18)/57)**2)**2
                    ground=tuple(round(v+d*glow) for v,d in [(8,8),(11,14),(15,25)])
                    if edge>17**2:ground=(67,78,92)
                    col=next((c for verts,c in shapes if inside(x,y,verts)),ground)
                    for k in range(3):values[k]+=col[k]
                    values[3]+=1
            raw.extend([round(c/max(values[3],1)) for c in values[:3]]+[round(255*values[3]/4)])
    def chunk(t,d):return struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d))
    return b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',size,size,8,6,0,0,0))+chunk(b'IDAT',zlib.compress(raw))+chunk(b'IEND',b'')
sizes=[16,24,32,48,64,128,256]
icons=[png(s) for s in sizes]
offset=6+16*len(icons)
entries=bytearray()
for size,data in zip(sizes,icons):
    entries.extend(struct.pack('<BBBBHHII',size%256,size%256,0,0,1,32,len(data),offset))
    offset+=len(data)
(root/'Vela.ico').write_bytes(struct.pack('<HHH',0,1,len(icons))+entries+b''.join(icons))
(root/'Vela_icon.png').write_bytes(icons[-1])
