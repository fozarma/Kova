from PIL import Image, ImageDraw
from io import BytesIO
from pathlib import Path
import struct
root=Path(__file__).parent
sizes=[16,24,32,48,64,128,256]

def generate(size):
 scale=size*4
 im=Image.new('RGBA',(scale,scale),(0,0,0,0))
 d=ImageDraw.Draw(im)
 k=scale/256
 def pts(p):return [(round(x*k),round(y*k)) for x,y in p]
 def rect(xy,radius=0,fill=None,outline=None,width=1):d.rounded_rectangle([round(v*k) for v in xy],radius=round(radius*k),fill=fill,outline=outline,width=max(1,round(width*k)))
 rect((8,8,248,248),50,(18,20,23,255),(76,81,88,255),3)
 white=(242,244,246,255)
 # Clean, monogrammatic K in three shapes (not the old H symbol).
 d.polygon(pts([(69,58),(102,58),(102,198),(69,198)]),fill=white)
 d.polygon(pts([(111,125),(176,58),(219,58),(154,128),(219,198),(177,198)]),fill=white)
 d.polygon(pts([(117,123),(142,103),(177,198),(145,198)]),fill=white)
 d.ellipse([round(v*k) for v in (201,36,222,57)],fill=(47,211,125,255))
 im=im.resize((size,size),Image.Resampling.LANCZOS)
 raw=BytesIO();im.save(raw,format='PNG',optimize=True)
 return raw.getvalue(),im

results=[generate(s) for s in sizes]
header=struct.pack('<HHH',0,1,len(results));index=b'';data=b'';offset=6+16*len(results)
for size,(raw,_) in zip(sizes,results):
 index+=struct.pack('<BBBBHHII',size if size<256 else 0,size if size<256 else 0,0,0,1,32,len(raw),offset)
 data+=raw;offset+=len(raw)
(root/'Kova.ico').write_bytes(header+index+data)
results[-1][1].save(root/'Kova_icon.png')
def resource(type_id,name_id,payload):
 hdr=struct.pack('<IIHHHHIHHII',len(payload),32,0xffff,type_id,0xffff,name_id,0,0x1030,0,0,0)
 return hdr+payload+b'\0'*((-len(payload))%4)
res=resource(0,0,b'')
for i,(raw,_) in enumerate(results):res+=resource(3,101+i,raw)
grp=struct.pack('<HHH',0,1,len(results))
for i,(size,(raw,_)) in enumerate(zip(sizes,results)):
 grp+=struct.pack('<BBBBHHIH',size if size<256 else 0,size if size<256 else 0,0,0,1,32,len(raw),101+i)
res+=resource(14,1,grp)
(root/'Kova.res').write_bytes(res)
print('generated',len(res),'bytes resource')
