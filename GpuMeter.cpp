#include <windows.h>
#include <d3dkmthk.h>

static LUID adapterId;
static UINT nodes, segments;
static ULONGLONG dedicatedCapacity;
static ULONGLONG lastTick, previous[64];
static bool valid[64];

extern "C" void initGpuMeter(const wchar_t* name){
    D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME open={};
    for(int i=0;i<31 && name[i];++i)open.DeviceName[i]=name[i];
    if(D3DKMTOpenAdapterFromGdiDisplayName(&open)<0)return;
    adapterId=open.AdapterLuid;
    D3DKMT_QUERYSTATISTICS query={};
    query.Type=D3DKMT_QUERYSTATISTICS_ADAPTER;
    query.AdapterLuid=adapterId;
    if(D3DKMTQueryStatistics(&query)>=0){
        const auto& info=query.QueryResult.AdapterInformation;
        if(info.NodeCount<=64)nodes=info.NodeCount;
        segments=info.NbSegments;
    }
    D3DKMT_SEGMENTSIZEINFO sizes={};
    D3DKMT_QUERYADAPTERINFO info={open.hAdapter,KMTQAITYPE_GETSEGMENTSIZE,&sizes,sizeof(sizes)};
    if(D3DKMTQueryAdapterInfo(&info)>=0)dedicatedCapacity=sizes.DedicatedVideoMemorySize;
    D3DKMT_CLOSEADAPTER close={open.hAdapter};D3DKMTCloseAdapter(&close);
}
extern "C" int sampleGpuMeter(ULONGLONG* used,ULONGLONG* capacity){
    const ULONGLONG now=GetTickCount64(),elapsed=(now-lastTick)*10000;
    int busiest=-1;
    for(UINT i=0;i<nodes;++i){
        D3DKMT_QUERYSTATISTICS query={};
        query.Type=D3DKMT_QUERYSTATISTICS_NODE;query.AdapterLuid=adapterId;query.QueryNode.NodeId=i;
        if(D3DKMTQueryStatistics(&query)<0){valid[i]=false;continue;}
        ULONGLONG busy=query.QueryResult.NodeInformation.GlobalInformation.RunningTime.QuadPart;
        if(valid[i] && elapsed && elapsed<50000000 && busy>=previous[i]){
            ULONGLONG change=busy-previous[i];
            int percent=(int)((min(change,elapsed)*100+elapsed/2)/elapsed);
            if(percent>busiest)busiest=percent;
        }
        previous[i]=busy;valid[i]=true;
    }
    lastTick=now;*used=*capacity=0;
    for(UINT i=0;i<segments;++i){
        D3DKMT_QUERYSTATISTICS query={};
        query.Type=D3DKMT_QUERYSTATISTICS_SEGMENT;query.AdapterLuid=adapterId;query.QuerySegment.SegmentId=i;
        if(D3DKMTQueryStatistics(&query)<0){*used=*capacity=0;break;}
        const auto& segment=query.QueryResult.SegmentInformation;
        if(!segment.Aperture){*used+=segment.BytesResident;*capacity+=segment.CommitLimit;}
    }
    if(*capacity && dedicatedCapacity)*capacity=dedicatedCapacity;
    return busiest;
}
