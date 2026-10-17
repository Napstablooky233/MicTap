#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <tlhelp32.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <audioclient.h>
#include <mmreg.h>
#include <functiondiscoverykeys_devpkey.h>
#include <shellapi.h>
#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cwctype>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "process_loopback_params.h"

/* ================= localization ================= */
#pragma warning(disable : 4996)

#define STRINGS(X) \
    X(Title,          L"MicTap",                                      L"MicTap") \
    X(Subtitle,       L"把应用声音和麦克风混合后，送入任何需要麦克风的应用。",  L"Mix app audio and your microphone into any app that needs a mic.") \
    X(SecApp,         L"应用音频",                                    L"App Audio") \
    X(SecMic,         L"物理麦克风",                                  L"Microphone") \
    X(LblSource,      L"来源",                                        L"Source") \
    X(LblVolume,      L"音量 %",                                      L"Volume %") \
    X(LblDevice,      L"输入设备",                                    L"Input device") \
    X(BtnRefresh,     L"刷新列表",                                    L"Refresh") \
    X(BtnRefreshing,  L"刷新中…",                                     L"Refreshing…") \
    X(ChkAll,         L"显示全部进程（含未播放的）",                    L"Show inactive processes") \
    X(BtnStartApp,    L"开启应用音频",                                L"Start App Audio") \
    X(BtnStopApp,     L"停止应用音频",                                L"Stop App Audio") \
    X(BtnStartMic,    L"开启麦克风",                                  L"Start Microphone") \
    X(BtnStopMic,     L"停止麦克风",                                  L"Stop Microphone") \
    X(RangeHint,      L"0–200",                                       L"0–200") \
    X(InfoLeft,       L"混音写入",                                     L"Mixes into") \
    X(InfoMid,        L"· 接收应用读取",                              L"· apps read") \
    X(StStopped,      L"已停止",                                      L"Stopped") \
    X(StStarting,     L"启动中",                                      L"Starting") \
    X(StRunning,      L"运行中",                                      L"Running") \
    X(StError,        L"出错",                                        L"Error") \
    X(BtnDetect,      L"检测设备",                                    L"Detect devices") \
    X(BtnSound,       L"系统录音设置",                                L"Sound settings") \
    X(BtnLogs,        L"打开日志目录",                                L"Open logs") \
    X(Hint,           L"提示：勾选多个应用可同时混音（最多 8 个）。带 ● 的条目正在输出声音。", \
                      L"Tip: tick several apps to mix them at once (up to 8). Items marked ● are playing audio.") \
    X(StNoCable,      L"未检测到 CABLE Input。请安装 VB-CABLE 后点“检测设备”。", \
                      L"CABLE Input not found. Install VB-CABLE, then click “Detect devices”.") \
    X(StNoCable2,     L"未检测到 CABLE Input，请先安装 VB-CABLE。",     L"CABLE Input not found — install VB-CABLE first.") \
    X(StReady,        L"就绪 · 两路可独立开启或停止",                   L"Ready · start either lane independently") \
    X(StMixing,       L"混音中 → CABLE Output（在接收应用里选择该输入设备）", L"Mixing → CABLE Output (select it as the mic in the other app)") \
    X(StErrProc,      L"无法捕获该应用：进程可能已退出，或系统不允许捕获。", L"Cannot capture this app: it may have exited or capture is not allowed.") \
    X(StErrMic,       L"无法打开所选麦克风：设备可能被独占或已断开。",     L"Cannot open that microphone: it may be exclusive or disconnected.") \
    X(StFound,        L"已发现 %d 项 · 勾选要混入的应用",                L"%d items · tick the apps to mix") \
    X(StPicked,       L"已勾选 %d 个应用",                               L"%d apps ticked") \
    X(StEmpty,        L"未发现正在播放声音的应用。启动播放后点“刷新列表”，或勾选“显示全部进程”。", \
                      L"No app is playing audio. Start playback and refresh, or tick “Show inactive processes”.") \
    X(StEmptyAll,     L"没有可列出的进程。",                            L"No processes to list.") \
    X(StNeedStop,     L"请先停止应用音频，再刷新来源列表。",              L"Stop app audio before refreshing the source list.") \
    X(StSelectApp,    L"请至少勾选一个应用。",                          L"Tick at least one app.") \
    X(StGainApp,      L"应用音量请输入 0–200。",                        L"App volume must be between 0 and 200.") \
    X(StGainMic,      L"麦克风音量请输入 0–200。",                      L"Mic volume must be between 0 and 200.") \
    X(StPickMic,      L"请选择一个麦克风。",                            L"Select a microphone.") \
    X(StStale,        L"目标应用已退出或重启，请刷新后重新选择。",         L"That app exited or restarted. Refresh and select it again.") \
    X(StGone,         L"该进程已不可用。",                              L"That process is no longer available.") \
    X(StDevFail,      L"设备枚举失败。",                                L"Device enumeration failed.") \
    X(StLaunchFail,   L"音频引擎线程启动失败，请重启程序。",              L"Audio engine thread failed to start. Restart the app.") \
    X(Playing,        L"正在播放",                                     L"playing") \
    X(CatMusic,       L"音乐",                                        L"Music") \
    X(CatBrowser,     L"浏览器",                                      L"Browser") \
    X(CatVideo,       L"视频",                                        L"Video") \
    X(CatChat,        L"通讯",                                        L"Chat")

enum StrId {
#define X(id, zh, en) id,
    STRINGS(X)
#undef X
    StrCount
};

static const wchar_t* const g_zh[StrCount] = {
#define X(id, zh, en) zh,
    STRINGS(X)
#undef X
};
static const wchar_t* const g_en[StrCount] = {
#define X(id, zh, en) en,
    STRINGS(X)
#undef X
};

int g_lang = 0;   /* 0 = zh-CN, 1 = en-US */

inline const wchar_t* S(StrId id) { return g_lang ? g_en[id] : g_zh[id]; }

std::wstring fmtStr(StrId id, int n) {
    wchar_t b[256]{};
    swprintf(b, 256, S(id), n);
    return b;
}

template<class T> struct Com {
    T* p = nullptr;
    ~Com() { reset(); }
    void reset() { if (p) { p->Release(); p = nullptr; } }
    T** put() { reset(); return &p; }
    T* operator->() { return p; }
    Com() = default;
    Com(const Com&) = delete;
};

/* ================= engine ================= */
CRITICAL_SECTION cfgLock, errLock;
std::wstring cfgOutId, cfgMicId;
std::vector<DWORD> cfgProcPids;          /* every app the user ticked */
std::atomic<int> wantProc{0}, wantMic{0};
std::atomic<int> stateProc{0}, stateMic{0};
std::atomic<float> gainProc{1.0f}, gainMic{1.0f};
std::atomic<bool> engineStop{false};
std::wstring errProc, errMic;
HWND mainWin = nullptr;

void reportError(int lane, StrId id) {
    EnterCriticalSection(&errLock);
    if (lane == 0) errProc = S(id); else errMic = S(id);
    LeaveCriticalSection(&errLock);
    if (lane == 0) stateProc = 2; else stateMic = 2;
    if (mainWin) PostMessageW(mainWin, WM_APP + 1, lane, 0);
}

struct Source {
    Com<IAudioClient> client;
    Com<IAudioCaptureClient> cap;
    std::deque<float> q;
    HANDLE watch = nullptr;      /* target process handle, lets us notice it exiting */
    DWORD pid = 0;               /* which process this lane captures */
    bool open = false;
    /* cross-thread readable counters (engine thread writes, UI / --route reads) */
    std::atomic<unsigned long long> captured{0};
    std::atomic<float> peak{0.0f};
    std::atomic<size_t> queued{0};
    void Close() {
        if (client.p) client.p->Stop();
        cap.reset();
        client.reset();
        if (watch) { CloseHandle(watch); watch = nullptr; }
        open = false;
        pid = 0;
        q.clear();
        queued = 0;
    }
};

/* one capture lane per ticked app; every lane is mixed into CABLE Input */
constexpr int kMaxApps = 8;
Source g_apps[kMaxApps];
Source g_mic;

struct Render {
    Com<IAudioClient> client;
    Com<IAudioRenderClient> render;
    UINT capacity = 0;
    bool open = false;
    void Close() {
        if (client.p) client.p->Stop();
        render.reset();
        client.reset();
        open = false;
        capacity = 0;
    }
};

WAVEFORMATEX unifiedFormat() {
    WAVEFORMATEX f{};
    f.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    f.nChannels = 2;
    f.nSamplesPerSec = 48000;
    f.wBitsPerSample = 32;
    f.nBlockAlign = 8;
    f.nAvgBytesPerSec = 384000;
    return f;
}

class Activation final : public IActivateAudioInterfaceCompletionHandler, public IAgileObject {
    std::atomic<ULONG> refs{1};
public:
    HANDLE event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HRESULT result = E_PENDING;
    Com<IAudioClient> client;
    ~Activation() { if (event) CloseHandle(event); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (id == IID_IUnknown || id == __uuidof(IActivateAudioInterfaceCompletionHandler))
            *out = static_cast<IActivateAudioInterfaceCompletionHandler*>(this);
        else if (id == __uuidof(IAgileObject))
            *out = static_cast<IAgileObject*>(this);
        else return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs; }
    ULONG STDMETHODCALLTYPE Release() override { ULONG n = --refs; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE ActivateCompleted(IActivateAudioInterfaceAsyncOperation* op) override {
        Com<IUnknown> object;
        HRESULT activation = E_FAIL;
        HRESULT call = op ? op->GetActivateResult(&activation, object.put()) : E_POINTER;
        result = FAILED(call) ? call : activation;
        if (SUCCEEDED(result))
            result = object.p ? object->QueryInterface(__uuidof(IAudioClient), reinterpret_cast<void**>(client.put())) : E_POINTER;
        SetEvent(event);
        return S_OK;
    }
};

static inline bool isUnified(const WAVEFORMATEX* w) {
    if (!w) return false;
    if (w->nSamplesPerSec != 48000 || w->nChannels != 2 || w->wBitsPerSample != 32) return false;
    if (w->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) return true;
    if (w->wFormatTag == WAVE_FORMAT_EXTENSIBLE && w->cbSize >= 22) {
        const WAVEFORMATEXTENSIBLE* e = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(w);
        static const GUID kFloat = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
        return IsEqualGUID(e->SubFormat, kFloat) != 0;
    }
    return false;
}

/* 48 kHz / stereo / float32 is what the mixer runs at, so a stream that already
   is that format is opened without AUTOCONVERTPCM: nothing is resampled and
   nothing is quantised again. A converter in the path is what makes audio dull. */
static std::wstring g_srcFormat;

static DWORD conversionFlags(IAudioClient* client) {
    WAVEFORMATEX* mix = nullptr;
    bool same = false;
    if (client && SUCCEEDED(client->GetMixFormat(&mix)) && mix) {
        same = isUnified(mix);
        wchar_t buf[96];
        swprintf(buf, 96, L"%lu/%u/%u%s", (unsigned long)mix->nSamplesPerSec, mix->nChannels,
                 mix->wBitsPerSample, same ? L"/float" : L"/other");
        g_srcFormat = buf;
        CoTaskMemFree(mix);
    }
    return same ? 0 : AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
}

bool openOutput(Render& r, const std::wstring& id) {
    r.Close();
    if (id.empty()) return false;
    Com<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) return false;
    Com<IMMDevice> dev;
    if (FAILED(en->GetDevice(id.c_str(), dev.put()))) return false;
    Com<IAudioClient> client;
    Com<IAudioRenderClient> render;
    UINT capacity = 0;
    WAVEFORMATEX f = unifiedFormat();
    if (FAILED(dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(client.put())))) return false;
    if (FAILED(client->Initialize(AUDCLNT_SHAREMODE_SHARED,
            conversionFlags(client.p),
            300000, 0, &f, nullptr))) return false;
    if (FAILED(client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(render.put())))) return false;
    if (FAILED(client->GetBufferSize(&capacity))) return false;
    if (FAILED(client->Start())) return false;
    /* transfer ownership only after every step succeeded */
    r.client.p = client.p; client.p = nullptr;
    r.render.p = render.p; render.p = nullptr;
    r.capacity = capacity;
    r.open = true;
    return true;
}

bool openProcessSource(Source& s, DWORD pid) {
    s.Close();
    if (!pid) return false;
    /* a dead or missing target must fail here, not silently capture silence */
    HANDLE watch = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!watch) return false;
    Activation* handler = new Activation;
    if (!handler->event) { handler->Release(); CloseHandle(watch); return false; }
    AUDIOCLIENT_ACTIVATION_PARAMS ap{};
    ap.ActivationType = AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
    ap.ProcessLoopbackParams.TargetProcessId = pid;
    ap.ProcessLoopbackParams.ProcessLoopbackMode = PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE;
    PROPVARIANT params{};
    params.vt = VT_BLOB;
    params.blob.cbSize = sizeof(ap);
    params.blob.pBlobData = reinterpret_cast<BYTE*>(&ap);
    Com<IActivateAudioInterfaceAsyncOperation> op;
    HRESULT hr = ActivateAudioInterfaceAsync(VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK,
        __uuidof(IAudioClient), &params, handler, op.put());
    /* wait in slices so a stop request is never blocked by an 8s activation */
    bool activated = false;
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < 20 && !engineStop; ++i) {
            if (WaitForSingleObject(handler->event, 400) == WAIT_OBJECT_0) { activated = true; break; }
        }
    }
    bool ok = false;
    if (activated && SUCCEEDED(handler->result) && handler->client.p) {
        Com<IAudioCaptureClient> cap;
        WAVEFORMATEX f = unifiedFormat();
        if (SUCCEEDED(handler->client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_LOOPBACK | conversionFlags(handler->client.p),
                300000, 0, &f, nullptr))
            && SUCCEEDED(handler->client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(cap.put())))
            && SUCCEEDED(handler->client->Start())) {
            s.client.p = handler->client.p; handler->client.p = nullptr;
            s.cap.p = cap.p; cap.p = nullptr;
            s.watch = watch;
            watch = nullptr;
            s.open = true;
            ok = true;
        }
    }
    if (watch) CloseHandle(watch);
    handler->Release();
    return ok;
}

bool openMicSource(Source& s, const std::wstring& id) {
    s.Close();
    if (id.empty()) return false;
    Com<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) return false;
    Com<IMMDevice> dev;
    if (FAILED(en->GetDevice(id.c_str(), dev.put()))) return false;
    Com<IAudioClient> client;
    Com<IAudioCaptureClient> cap;
    WAVEFORMATEX f = unifiedFormat();
    if (FAILED(dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(client.put())))) return false;
    if (FAILED(client->Initialize(AUDCLNT_SHAREMODE_SHARED,
            conversionFlags(client.p),
            300000, 0, &f, nullptr))) return false;
    if (FAILED(client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(cap.put())))) return false;
    if (FAILED(client->Start())) return false;
    s.client.p = client.p; client.p = nullptr;
    s.cap.p = cap.p; cap.p = nullptr;
    s.open = true;
    return true;
}

/* only the last 3% is touched, and the knee is smooth: hard clipping at full
   scale is audible as grit, a soft knee is not */
static inline float softClip(float v) {
    const float knee = 0.97f;
    if (v > knee) return knee + (1.0f - knee) * std::tanh((v - knee) / (1.0f - knee));
    if (v < -knee) return -knee - (1.0f - knee) * std::tanh((-v - knee) / (1.0f - knee));
    return v;
}

/* pull every pending packet; stores raw samples, gain is applied at the mix stage */
void pump(Source& s) {
    UINT packets = 0;
    int fails = 0;
    for (;;) {
        if (FAILED(s.cap->GetNextPacketSize(&packets))) {
            if (++fails > 8) { s.Close(); return; }
            break;
        }
        if (packets == 0) break;
        BYTE* data = nullptr;
        UINT frames = 0;
        DWORD flags = 0;
        if (FAILED(s.cap->GetBuffer(&data, &frames, &flags, nullptr, nullptr))) {
            if (++fails > 8) { s.Close(); return; }
            break;
        }
        size_t count = static_cast<size_t>(frames) * 2;
        if ((flags & AUDCLNT_BUFFERFLAGS_SILENT) || !data) {
            s.q.insert(s.q.end(), count, 0.0f);
        } else {
            const float* in = reinterpret_cast<const float*>(data);
            for (size_t i = 0; i < count; ++i) {
                float v = in[i];
                if (!std::isfinite(v)) v = 0.0f;
                float a = std::fabs(v);
                if (a > s.peak.load()) s.peak.store(a);
                s.q.push_back(v);
            }
        }
        s.captured.fetch_add(frames);
        s.cap->ReleaseBuffer(frames);
    }
    /* bounded latency: hold ~50ms, drop the oldest audio beyond 150ms */
    const size_t target = 48000 / 20 * 2;
    const size_t highWater = target * 3;
    if (s.q.size() > highWater) {
        size_t drop = s.q.size() - target;
        drop &= ~static_cast<size_t>(1);
        for (size_t i = 0; i < drop; ++i) s.q.pop_front();
    }
    s.queued = s.q.size() / 2;
}

DWORD WINAPI engineThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    Source& mic = g_mic;
    std::vector<DWORD> active;               /* pids currently occupying the lanes */
    bool laneFailed[kMaxApps] = {};
    Render out;
    bool failedMic = false;
    DWORD outRetryAt = 0;
    int outFails = 0;
    float curProc = 0.0f, curMic = 0.0f;                 /* gain ramp state */
    const float rampStep = 1.0f / (0.020f * 48000.0f);   /* 20ms ramp */
    const float headroom = 0.95f;                        /* 0.45 dB of headroom, rest goes to the knee */

    while (!engineStop) {
        std::wstring outId, micId;
        std::vector<DWORD> wanted;
        EnterCriticalSection(&cfgLock);
        outId = cfgOutId; micId = cfgMicId; wanted = cfgProcPids;
        LeaveCriticalSection(&cfgLock);
        if (wanted.size() > (size_t)kMaxApps) wanted.resize(kMaxApps);

        int wp = wantProc.load(), wm = wantMic.load();
        if (!wp) wanted.clear();
        bool needOut = !wanted.empty() || wm != 0;

        /* the ticked set changed: drop the old lanes, they get reopened below */
        if (wanted != active) {
            for (int i = 0; i < kMaxApps; ++i) { g_apps[i].Close(); laneFailed[i] = false; }
            active = wanted;
            if (active.empty()) stateProc = 0;
        }

        /* CABLE Input: open on demand, reopen with exponential backoff on failure */
        if (!out.open && needOut && GetTickCount() >= outRetryAt) {
            if (openOutput(out, outId)) outFails = 0;
            else {
                out.Close();
                if (outFails < 5) ++outFails;
                outRetryAt = GetTickCount() + (500u << outFails);
                reportError(wp ? 0 : 1, StNoCable);
            }
        }
        if (out.open && !needOut) out.Close();

        int lanesOpen = 0;
        for (int i = 0; i < (int)active.size(); ++i) {
            Source& s = g_apps[i];
            if (!s.open && !laneFailed[i] && !engineStop) {
                if (openProcessSource(s, active[i])) { s.pid = active[i]; stateProc = 1; }
                else if (!engineStop.load()) {
                    laneFailed[i] = true;
                    if (stateProc != 1) { stateProc = 2; reportError(0, StErrProc); }
                }
            }
            if (s.open) ++lanesOpen;
        }
        if (wp && lanesOpen) stateProc = 1;
        if (!wp && stateProc != 2) stateProc = 0;
        if (wm && !mic.open && !failedMic) {
            if (openMicSource(mic, micId)) { stateMic = 1; failedMic = false; }
            else if (!engineStop.load()) { failedMic = true; reportError(1, StErrMic); }
        }
        if (!wm) {
            if (mic.open) mic.Close();
            failedMic = false;
            if (stateMic != 2) stateMic = 0;
        }

        for (int i = 0; i < (int)active.size(); ++i) if (g_apps[i].open) pump(g_apps[i]);
        if (mic.open) pump(mic);

        /* a ticked app exited: retire its lane, keep the others running */
        int lanesAlive = 0;
        for (int i = 0; i < (int)active.size(); ++i) {
            Source& s = g_apps[i];
            if (!s.open) continue;
            if (s.watch && WaitForSingleObject(s.watch, 0) == WAIT_OBJECT_0) {
                s.Close();
                laneFailed[i] = true;
                stateProc = 2;
                reportError(0, StStale);
                continue;
            }
            ++lanesAlive;
        }
        if (wp && lanesAlive == 0 && !active.empty()) { wantProc = 0; stateProc = 2; }

        if (out.open) {
            UINT pad = 0;
            if (FAILED(out.client->GetCurrentPadding(&pad))) {
                out.Close();                 /* device invalidated: reopen next round */
            } else {
                UINT free = out.capacity > pad ? out.capacity - pad : 0;
                if (free) {
                    BYTE* data = nullptr;
                    if (FAILED(out.render->GetBuffer(free, &data)) || !data) {
                        out.Close();
                    } else {
                        float* dst = reinterpret_cast<float*>(data);
                        float gp = gainProc.load(), gm = gainMic.load();
                        for (UINT i = 0; i < free; ++i) {
                            if (curProc < gp) { curProc += rampStep; if (curProc > gp) curProc = gp; }
                            else if (curProc > gp) { curProc -= rampStep; if (curProc < gp) curProc = gp; }
                            if (curMic < gm) { curMic += rampStep; if (curMic > gm) curMic = gm; }
                            else if (curMic > gm) { curMic -= rampStep; if (curMic < gm) curMic = gm; }
                            float l = 0.0f, r = 0.0f;
                            for (int k = 0; k < (int)active.size(); ++k) {
                                Source& s = g_apps[k];
                                if (s.open && s.q.size() >= 2) {
                                    l += s.q[0] * curProc; r += s.q[1] * curProc;
                                    s.q.pop_front(); s.q.pop_front();
                                }
                            }
                            if (mic.open && mic.q.size() >= 2) {
                                l += mic.q[0] * curMic; r += mic.q[1] * curMic;
                                mic.q.pop_front(); mic.q.pop_front();
                            }
                            dst[i * 2] = softClip(l * headroom);
                            dst[i * 2 + 1] = softClip(r * headroom);
                        }
                        out.render->ReleaseBuffer(free, 0);
                        for (int k = 0; k < (int)active.size(); ++k) g_apps[k].queued = g_apps[k].q.size() / 2;
                        mic.queued = mic.q.size() / 2;
                    }
                }
            }
        }
        Sleep(2);
    }
    for (int i = 0; i < kMaxApps; ++i) g_apps[i].Close();
    mic.Close();
    out.Close();
    CoUninitialize();
    return 0;
}

/* ================= discovery ================= */
struct Target {
    DWORD pid = 0;
    FILETIME created{};
    std::wstring path;
    bool verified = false;
};

struct Preset { const wchar_t* exe; const wchar_t* nameZh; const wchar_t* nameEn; StrId cat; };
const Preset g_presets[] = {
    {L"cloudmusic.exe", L"网易云音乐", L"NetEase Cloud Music", CatMusic},
    {L"QQMusic.exe", L"QQ音乐", L"QQ Music", CatMusic},
    {L"Spotify.exe", L"Spotify", L"Spotify", CatMusic},
    {L"foobar2000.exe", L"foobar2000", L"foobar2000", CatMusic},
    {L"KuGou.exe", L"酷狗音乐", L"KuGou Music", CatMusic},
    {L"KwMusic.exe", L"酷我音乐", L"Kuwo Music", CatMusic},
    {L"chrome.exe", L"Google Chrome", L"Google Chrome", CatBrowser},
    {L"msedge.exe", L"Microsoft Edge", L"Microsoft Edge", CatBrowser},
    {L"firefox.exe", L"Firefox", L"Firefox", CatBrowser},
    {L"brave.exe", L"Brave", L"Brave", CatBrowser},
    {L"opera.exe", L"Opera", L"Opera", CatBrowser},
    {L"vivaldi.exe", L"Vivaldi", L"Vivaldi", CatBrowser},
    {L"vlc.exe", L"VLC", L"VLC", CatVideo},
    {L"PotPlayerMini64.exe", L"PotPlayer", L"PotPlayer", CatVideo},
    {L"PotPlayerMini.exe", L"PotPlayer", L"PotPlayer", CatVideo},
    {L"mpv.exe", L"mpv", L"mpv", CatVideo},
    {L"mpc-hc64.exe", L"MPC-HC", L"MPC-HC", CatVideo},
    {L"QQLive.exe", L"腾讯视频", L"Tencent Video", CatVideo},
    {L"Youku.exe", L"优酷", L"Youku", CatVideo},
    {L"iQIYI.exe", L"爱奇艺", L"iQIYI", CatVideo},
    {L"QQ.exe", L"腾讯QQ", L"QQ", CatChat},
    {L"WeChat.exe", L"微信", L"WeChat", CatChat},
    {L"Discord.exe", L"Discord", L"Discord", CatChat},
    {L"Telegram.exe", L"Telegram", L"Telegram", CatChat},
};

std::map<DWORD, bool> activeSessions() {
    std::map<DWORD, bool> result;
    Com<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) return result;
    Com<IMMDevice> dev;
    if (FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, dev.put()))) return result;
    Com<IAudioSessionManager2> mgr;
    if (FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(mgr.put())))) return result;
    Com<IAudioSessionEnumerator> sessions;
    if (FAILED(mgr->GetSessionEnumerator(sessions.put()))) return result;
    int count = 0;
    sessions->GetCount(&count);
    for (int i = 0; i < count; ++i) {
        Com<IAudioSessionControl> ctl;
        if (FAILED(sessions->GetSession(i, ctl.put()))) continue;
        Com<IAudioSessionControl2> ctl2;
        if (FAILED(ctl->QueryInterface(IID_PPV_ARGS(ctl2.put())))) continue;
        DWORD pid = 0;
        if (FAILED(ctl2->GetProcessId(&pid)) || !pid) continue;
        AudioSessionState st = AudioSessionStateInactive;
        ctl2->GetState(&st);
        bool active = st == AudioSessionStateActive;
        auto it = result.find(pid);
        if (it == result.end() || active) result[pid] = active;
    }
    return result;
}

BOOL CALLBACK windowEnum(HWND w, LPARAM param) {
    auto* titles = reinterpret_cast<std::map<DWORD, std::wstring>*>(param);
    if (!IsWindowVisible(w)) return TRUE;
    int len = GetWindowTextLengthW(w);
    if (len <= 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (!pid) return TRUE;
    wchar_t buf[512]{};
    GetWindowTextW(w, buf, 512);
    std::wstring title = buf;
    if (title.empty()) return TRUE;
    auto it = titles->find(pid);
    if (it == titles->end() || it->second.size() < title.size()) (*titles)[pid] = title;
    return TRUE;
}

void collectWindows(std::map<DWORD, std::wstring>& titles) {
    titles.clear();
    EnumWindows(windowEnum, reinterpret_cast<LPARAM>(&titles));
}

std::wstring findCableInput() {
    std::wstring id;
    Com<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) return id;
    Com<IMMDeviceCollection> coll;
    if (FAILED(en->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, coll.put()))) return id;
    UINT n = 0;
    coll->GetCount(&n);
    for (UINT i = 0; i < n; ++i) {
        Com<IMMDevice> d;
        if (FAILED(coll->Item(i, d.put()))) continue;
        Com<IPropertyStore> props;
        PROPVARIANT v{};
        std::wstring name;
        if (SUCCEEDED(d->OpenPropertyStore(STGM_READ, props.put()))
            && SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &v)) && v.vt == VT_LPWSTR)
            name = v.pwszVal;
        PropVariantClear(&v);
        if (name.find(L"CABLE Input (") != std::wstring::npos) {
            LPWSTR raw = nullptr;
            if (SUCCEEDED(d->GetId(&raw))) { id = raw; CoTaskMemFree(raw); }
            break;
        }
    }
    return id;
}

bool identify(DWORD pid, Target& t) {
    t.pid = pid;
    t.verified = false;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return false;
    wchar_t path[32768]{};
    DWORD len = 32768;
    FILETIME exit{}, kernel{}, user{};
    bool ok = QueryFullProcessImageNameW(h, 0, path, &len) && GetProcessTimes(h, &t.created, &exit, &kernel, &user);
    CloseHandle(h);
    if (!ok) return false;
    t.path = path;
    t.verified = true;
    return true;
}

/* ================= ui ================= */
/* palette */
static const COLORREF C_PAPER   = RGB(247, 242, 235);
static const COLORREF C_CARD    = RGB(255, 253, 246);
static const COLORREF C_FIELD   = RGB(255, 255, 255);
static const COLORREF C_LINE    = RGB(156, 143, 126);
static const COLORREF C_LINE2   = RGB(221, 211, 194);
static const COLORREF C_INK     = RGB(8, 31, 92);
static const COLORREF C_INK2    = RGB(74, 85, 104);
static const COLORREF C_INK3    = RGB(95, 102, 114);
static const COLORREF C_ACCENT  = RGB(51, 78, 172);
static const COLORREF C_ACC_HI  = RGB(44, 67, 145);
static const COLORREF C_ACC_LO  = RGB(34, 54, 111);
static const COLORREF C_BAR     = RGB(240, 236, 226);
static const COLORREF C_PILL_OFF= RGB(237, 234, 227);
static const COLORREF C_PILL_RUN= RGB(228, 240, 232);
static const COLORREF C_PILL_WRK= RGB(247, 239, 221);
static const COLORREF C_PILL_ERR= RGB(243, 228, 225);
static const COLORREF C_GREEN   = RGB(31, 111, 74);
static const COLORREF C_AMBER   = RGB(138, 90, 18);
static const COLORREF C_RED     = RGB(140, 59, 42);
static const COLORREF C_DIS_BG  = RGB(227, 222, 207);
static const COLORREF C_DIS_LN  = RGB(208, 199, 182);
static const COLORREF C_DIS_INK = RGB(163, 155, 138);

/* control ids */
enum {
    ID_TITLE = 1, ID_SUBTITLE = 2, ID_PILL_APP = 3,
    ID_PROC_BOX = 10, ID_REFRESH = 11, ID_PROC_GAIN = 12, ID_PROC_BTN = 13,
    ID_MIC_BOX = 20, ID_MIC_GAIN = 21, ID_MIC_BTN = 22, ID_PILL_MIC = 24,
    ID_DETECT = 30, ID_SOUND = 31, ID_LOGS = 32,
    ID_CHK_ALL = 40,
    ID_LANG_ZH = 50, ID_LANG_EN = 51,
    ID_INFO = 60, ID_STATUS = 61, ID_HINT = 62, ID_RANGE_APP = 63, ID_RANGE_MIC = 64,
    ID_SEC1_NO = 70, ID_SEC1_TITLE = 71, ID_SEC2_NO = 72, ID_SEC2_TITLE = 73,
    ID_LBL_SOURCE = 80, ID_LBL_GAIN_APP = 81, ID_LBL_DEVICE = 82, ID_LBL_GAIN_MIC = 83
};

HWND procBox, micBox, procGain, micGain, procButton, micButton, showAll, statusText;
HWND pillApp, pillMic, langZh, langEn, btnDetect, btnSound, btnLogs, btnRefresh;
HFONT fTitle, fSec, fBody, fSmall, fBold;
HBRUSH bPaper, bCard, bField, bBar, bPillOff, bPillRun, bPillWrk, bPillErr;
HANDLE audioThread = nullptr;
std::wstring g_root, g_outId;
std::vector<Target> g_targets;
std::vector<std::wstring> g_picked;      /* ticked apps, kept across refreshes and restarts */
bool g_syncing = false;                  /* true while the list is being repopulated */

std::wstring targetKey(const Target& t) {
    wchar_t b[32]{};
    swprintf(b, 32, L"*%08lX%08lX", t.created.dwLowDateTime, t.created.dwHighDateTime);
    return t.path + b;
}

void savePicked() {
    std::wstring joined;
    for (size_t i = 0; i < g_picked.size(); ++i) { if (i) joined += L"|"; joined += g_picked[i]; }
    std::wstring ini = g_root + L"\\settings.ini";
    WritePrivateProfileStringW(L"ui", L"picked", joined.c_str(), ini.c_str());
}

void loadPicked() {
    std::wstring ini = g_root + L"\\settings.ini";
    std::vector<wchar_t> buf(8192, 0);
    GetPrivateProfileStringW(L"ui", L"picked", L"", buf.data(), (DWORD)buf.size(), ini.c_str());
    g_picked.clear();
    std::wstring all = buf.data();
    size_t start = 0;
    for (;;) {
        size_t bar = all.find(L'|', start);
        std::wstring one = all.substr(start, bar == std::wstring::npos ? std::wstring::npos : bar - start);
        if (!one.empty()) g_picked.push_back(one);
        if (bar == std::wstring::npos) break;
        start = bar + 1;
    }
}
std::vector<std::wstring> g_micIds;
std::wstring g_hint;
int g_dpi = 96;
int g_toneProc = 0, g_toneMic = 0, g_statusTone = 0;
bool g_hovering = false;

void updateLaneUi();
void layoutAll(HWND w);
void applyLanguage(HWND w);
void rebuildFonts();

/* logical (96 dpi) rectangle helper */
struct Box { int x, y, w, h; };

static const Box B_CARD1   {24,  96, 832, 288};
static const Box B_CARD2   {24, 400, 832, 144};
static const Box B_BAR     {24, 592, 832,  48};

int D(int v) { return MulDiv(v, g_dpi, 96); }

std::wstring textOf(HWND w) {
    wchar_t b[2048]{};
    GetWindowTextW(w, b, 2048);
    return b;
}

void setStatus(const std::wstring& s, int tone = 0) {
    g_statusTone = tone;
    if (statusText) SetWindowTextW(statusText, s.c_str());
    if (mainWin) InvalidateRect(mainWin, nullptr, FALSE);
}

std::vector<HFONT> g_retiredFonts;  /* deleted only after every control switched */

void rebuildFonts() {
    const wchar_t* face = g_lang ? L"Segoe UI" : L"Microsoft YaHei UI";
    HFONT nTitle = CreateFontW(-D(22), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
    HFONT nSec   = CreateFontW(-D(15), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
    HFONT nBody  = CreateFontW(-D(13), 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
    HFONT nBold  = CreateFontW(-D(13), 0, 0, 0, FW_MEDIUM,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
    HFONT nSmall = CreateFontW(-D(12), 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
    if (fTitle) g_retiredFonts.push_back(fTitle);
    if (fSec)   g_retiredFonts.push_back(fSec);
    if (fBody)  g_retiredFonts.push_back(fBody);
    if (fBold)  g_retiredFonts.push_back(fBold);
    if (fSmall) g_retiredFonts.push_back(fSmall);
    fTitle = nTitle; fSec = nSec; fBody = nBody; fBold = nBold; fSmall = nSmall;
}

void flushRetiredFonts() {
    for (HFONT f : g_retiredFonts) if (f) DeleteObject(f);
    g_retiredFonts.clear();
}

HWND make(HWND parent, LPCWSTR cls, LPCWSTR text, Box b, int id, DWORD style, HFONT font = nullptr) {
    HWND c = CreateWindowW(cls, text, WS_CHILD | WS_VISIBLE | style,
        D(b.x), D(b.y), D(b.w), D(b.h), parent, (HMENU)(INT_PTR)id, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(c, WM_SETFONT, (WPARAM)(font ? font : fBody), TRUE);
    return c;
}

int measureText(HWND w, HFONT font, const std::wstring& text) {
    HDC dc = GetDC(w);
    HGDIOBJ old = SelectObject(dc, font);
    SIZE s{};
    GetTextExtentPoint32W(dc, text.c_str(), (int)text.length(), &s);
    SelectObject(dc, old);
    ReleaseDC(w, dc);
    return s.cx;
}

void refreshDevices() {
    g_outId.clear();
    std::wstring selectedMic;
    int ix = (int)SendMessageW(micBox, CB_GETCURSEL, 0, 0);
    if (ix >= 0 && ix < (int)g_micIds.size()) selectedMic = g_micIds[ix];
    g_micIds.clear();
    SendMessageW(micBox, CB_RESETCONTENT, 0, 0);

    Com<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) {
        setStatus(S(StDevFail), 2);
        return;
    }
    std::wstring defaultMic;
    Com<IMMDevice> def;
    if (SUCCEEDED(en->GetDefaultAudioEndpoint(eCapture, eCommunications, def.put()))) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(def->GetId(&id))) { defaultMic = id; CoTaskMemFree(id); }
    }
    int chosen = 0;
    for (int flow = 0; flow < 2; ++flow) {
        Com<IMMDeviceCollection> coll;
        if (FAILED(en->EnumAudioEndpoints(flow ? eCapture : eRender, DEVICE_STATE_ACTIVE, coll.put()))) continue;
        UINT n = 0;
        coll->GetCount(&n);
        for (UINT i = 0; i < n; ++i) {
            Com<IMMDevice> d;
            if (FAILED(coll->Item(i, d.put()))) continue;
            LPWSTR id = nullptr;
            if (FAILED(d->GetId(&id))) continue;
            Com<IPropertyStore> props;
            PROPVARIANT v{};
            std::wstring name;
            if (SUCCEEDED(d->OpenPropertyStore(STGM_READ, props.put()))
                && SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &v)) && v.vt == VT_LPWSTR)
                name = v.pwszVal;
            if (!flow && name.find(L"CABLE Input (") != std::wstring::npos) g_outId = id;
            if (flow && name.find(L"VB-Audio Virtual Cable") == std::wstring::npos && !name.empty()) {
                if ((selectedMic.empty() && defaultMic == id) || selectedMic == id) chosen = (int)g_micIds.size();
                g_micIds.push_back(id);
                SendMessageW(micBox, CB_ADDSTRING, 0, (LPARAM)name.c_str());
            }
            PropVariantClear(&v);
            CoTaskMemFree(id);
        }
    }
    if (!g_micIds.empty()) SendMessageW(micBox, CB_SETCURSEL, chosen, 0);
    SendMessageW(micBox, CB_SETDROPPEDWIDTH, D(720), 0);
    EnterCriticalSection(&cfgLock);
    cfgOutId = g_outId;
    LeaveCriticalSection(&cfgLock);
}

struct Row { std::wstring label; Target target; int rank; std::wstring exe; bool sounding; };

std::wstring presetName(const std::wstring& exe, StrId* category) {
    for (const auto& p : g_presets) {
        if (!_wcsicmp(exe.c_str(), p.exe)) {
            *category = p.cat;
            return g_lang ? p.nameEn : p.nameZh;
        }
    }
    *category = StrCount;
    return L"";
}

std::wstring clip(const std::wstring& s, size_t n) {
    if (s.size() <= n) return s;
    std::wstring r = s.substr(0, n);
    while (!r.empty() && r.back() == L' ') r.pop_back();
    return r + L"\u2026";
}

/* One raw process picked up by the scan, before identical binaries get folded. */
struct Cand {
    DWORD pid = 0;
    DWORD parent = 0;
    std::wstring exe;
    std::wstring key;                 /* lower-cased image path, else lower-cased exe name */
    std::wstring title;
    std::wstring friendly;
    StrId category = StrCount;
    Target target;
    bool known = false;
    bool sounding = false;
    bool windowed = false;
};

/* Pick the process that best represents a group of identical binaries.
   Tree roots win over children (the loopback capture already follows the tree),
   then a process that is really producing audio, then the oldest one - a freshly
   spawned helper must never win over the real application. */
size_t pickRoot(const std::vector<Cand>& cands, const std::vector<size_t>& ids) {
    std::map<DWORD, bool> inside;
    for (size_t i : ids) inside[cands[i].pid] = true;
    std::vector<size_t> roots;
    for (size_t i : ids) if (!inside.count(cands[i].parent)) roots.push_back(i);
    if (roots.empty()) roots = ids;
    size_t best = roots.front();
    for (size_t i : roots) {
        const Cand& a = cands[i];
        const Cand& b = cands[best];
        if (a.sounding != b.sounding) {
            if (a.sounding) best = i;
            continue;
        }
        if (a.target.verified && b.target.verified) {
            LONG d = CompareFileTime(&a.target.created, &b.target.created);
            if (d < 0) { best = i; continue; }
            if (d > 0) continue;
        }
        if (a.pid < b.pid) best = i;
    }
    return best;
}

void buildRows(bool all, std::vector<Row>& rows) {
    rows.clear();
    std::vector<PROCESSENTRY32W> entries;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(snap, &pe)) do { entries.push_back(pe); } while (Process32NextW(snap, &pe));
        CloseHandle(snap);
    }
    std::map<DWORD, bool> sessions = activeSessions();
    std::map<DWORD, std::wstring> titles;
    collectWindows(titles);

    std::vector<Cand> cands;
    DWORD self = GetCurrentProcessId();
    for (const auto& pe : entries) {
        if (!pe.th32ProcessID) continue;
        StrId category = StrCount;
        std::wstring friendly = presetName(pe.szExeFile, &category);
        bool known = category != StrCount;
        auto it = sessions.find(pe.th32ProcessID);
        bool sounding = it != sessions.end() && it->second;
        auto wt = titles.find(pe.th32ProcessID);
        bool windowed = wt != titles.end();
        if (!known && !sounding && !windowed && !all) continue;
        if (!known && !sounding && !windowed) {
            if (pe.th32ProcessID == self || pe.th32ProcessID == 0 || pe.th32ProcessID == 4) continue;
        }
        Cand c;
        c.pid = pe.th32ProcessID;
        c.parent = pe.th32ParentProcessID;
        c.exe = pe.szExeFile;
        c.category = category;
        c.friendly = friendly;
        c.known = known;
        c.sounding = sounding;
        c.windowed = windowed;
        if (windowed) c.title = wt->second;
        identify(pe.th32ProcessID, c.target);
        if (!c.target.verified) c.target.pid = pe.th32ProcessID;
        c.key = (c.target.verified && !c.target.path.empty()) ? c.target.path : pe.szExeFile;
        for (auto& ch : c.key) ch = (wchar_t)towlower(ch);
        cands.push_back(c);
    }

    /* Fold identical binaries into one entry: QQ and NetEase Cloud Music both run a
       parent plus several same-image children, and listing them separately is noise. */
    std::map<std::wstring, std::vector<size_t>> groups;
    for (size_t i = 0; i < cands.size(); ++i) groups[cands[i].key].push_back(i);

    for (auto& g : groups) {
        const std::vector<size_t>& ids = g.second;
        size_t root = pickRoot(cands, ids);
        const Cand& rc = cands[root];
        bool sounding = false, known = false, windowed = false;
        StrId category = StrCount;
        std::wstring friendly, title;
        for (size_t i : ids) {
            const Cand& c = cands[i];
            sounding = sounding || c.sounding;
            windowed = windowed || c.windowed;
            if (c.known && !known) { known = true; category = c.category; friendly = c.friendly; }
            if (title.empty() && !c.title.empty()) title = c.title;
        }
        std::wstring label;
        label += sounding ? L"● " : L"   ";
        if (known) label += std::wstring(L"[") + S(category) + L"] " + friendly;
        else if (windowed && title.size() > 1) label += clip(title, 34);
        else label += rc.exe;
        if (ids.size() > 1) label += L" ×" + std::to_wstring(ids.size());
        label += L" · PID " + std::to_wstring(rc.pid);
        if (sounding) label += std::wstring(L" · ") + S(Playing);
        int rank;
        if (known && sounding) rank = 0;        /* 已知播放器正在发声：最优先 */
        else if (known) rank = 1;               /* 已知播放器但当前静音 */
        else if (sounding) rank = 2;            /* 其他正在发声的进程 */
        else rank = 3;                          /* 窗口应用 / 全部进程 */
        rows.push_back({label, rc.target, rank, rc.exe, sounding});
    }

    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        if (a.rank != b.rank) return a.rank < b.rank;
        return a.label < b.label;
    });
}


void refreshProcesses() {
    if (wantProc.load()) { setStatus(S(StNeedStop), 2); return; }
    bool all = SendMessageW(showAll, BM_GETCHECK, 0, 0) == BST_CHECKED;
    std::vector<Row> rows;
    buildRows(all, rows);
    /* repopulating fires one LVN_ITEMCHANGED per tick: keep the stored set intact while it runs */
    g_syncing = true;
    SendMessageW(procBox, LVM_DELETEALLITEMS, 0, 0);
    g_targets.clear();
    int ticked = 0;
    for (size_t i = 0; i < rows.size(); ++i) {
        LVITEMW it{};
        it.mask = LVIF_TEXT;
        it.iItem = (int)i;
        it.pszText = const_cast<LPWSTR>(rows[i].label.c_str());
        SendMessageW(procBox, LVM_INSERTITEMW, 0, (LPARAM)&it);
        g_targets.push_back(rows[i].target);
        bool on = std::find(g_picked.begin(), g_picked.end(), targetKey(rows[i].target)) != g_picked.end();
        ListView_SetCheckState(procBox, (int)i, on);
        if (on) ++ticked;
    }
    g_syncing = false;
    if (rows.empty()) g_hint = S(all ? StEmptyAll : StEmpty);
    else if (ticked) g_hint = fmtStr(StPicked, ticked);
    else g_hint = fmtStr(StFound, (int)rows.size());
    updateLaneUi();
}

/* keep the ticked set in step with the list so a refresh or a language switch keeps it */
void syncPicked() {
    g_picked.clear();
    int n = (int)SendMessageW(procBox, LVM_GETITEMCOUNT, 0, 0);
    for (int i = 0; i < n && i < (int)g_targets.size(); ++i)
        if (ListView_GetCheckState(procBox, i)) g_picked.push_back(targetKey(g_targets[i]));
    int ticked = (int)g_picked.size();
    if (ticked) g_hint = fmtStr(StPicked, ticked);
    else {
        bool all = SendMessageW(showAll, BM_GETCHECK, 0, 0) == BST_CHECKED;
        g_hint = g_targets.empty() ? S(all ? StEmptyAll : StEmpty) : fmtStr(StFound, (int)g_targets.size());
    }
    savePicked();
}

bool pickedTargets(std::vector<Target>& out, std::wstring& error) {
    out.clear();
    int n = (int)SendMessageW(procBox, LVM_GETITEMCOUNT, 0, 0);
    for (int i = 0; i < n && i < (int)g_targets.size(); ++i) {
        if (!ListView_GetCheckState(procBox, i)) continue;
        Target t = g_targets[i];
        if (t.verified) {
            Target now{};
            if (!identify(t.pid, now) || now.path != t.path || CompareFileTime(&now.created, &t.created) != 0) {
                error = S(StStale);
                return false;
            }
        }
        if (!t.pid) continue;
        out.push_back(t);
        if ((int)out.size() >= kMaxApps) break;
    }
    if (out.empty()) { error = S(StSelectApp); return false; }
    return true;
}

int toneOf(int lane, bool run, int st) {
    if (!run) return 0;
    if (st == 2) return 3;
    if (st == 1) return 2;
    return 1;
}

void updateLaneUi() {
    static std::wstring lastAppBtn, lastMicBtn, lastPillApp, lastPillMic, lastStatus;
    bool pRun = wantProc.load() != 0;
    bool mRun = wantMic.load() != 0;
    int ps = stateProc.load(), ms = stateMic.load();
    std::wstring pe, me;
    EnterCriticalSection(&errLock);
    pe = errProc; me = errMic;
    LeaveCriticalSection(&errLock);

    g_toneProc = toneOf(0, pRun, ps);
    g_toneMic = toneOf(1, mRun, ms);

    std::wstring appBtn = pRun ? S(BtnStopApp) : S(BtnStartApp);
    std::wstring micBtn = mRun ? S(BtnStopMic) : S(BtnStartMic);
    if (appBtn != lastAppBtn) { SetWindowTextW(procButton, appBtn.c_str()); lastAppBtn = appBtn; InvalidateRect(procButton, nullptr, FALSE); }
    if (micBtn != lastMicBtn) { SetWindowTextW(micButton, micBtn.c_str()); lastMicBtn = micBtn; InvalidateRect(micButton, nullptr, FALSE); }

    std::wstring pillA = S(g_toneProc == 0 ? StStopped : (g_toneProc == 1 ? StStarting : (g_toneProc == 2 ? StRunning : StError)));
    std::wstring pillM = S(g_toneMic == 0 ? StStopped : (g_toneMic == 1 ? StStarting : (g_toneMic == 2 ? StRunning : StError)));
    if (pillA != lastPillApp) { SetWindowTextW(pillApp, pillA.c_str()); lastPillApp = pillA; InvalidateRect(pillApp, nullptr, FALSE); }
    if (pillM != lastPillMic) { SetWindowTextW(pillMic, pillM.c_str()); lastPillMic = pillM; InvalidateRect(pillMic, nullptr, FALSE); }

    EnableWindow(procBox, !pRun);
    EnableWindow(showAll, !pRun);
    EnableWindow(micBox, !mRun);

    std::wstring text;
    int tone = 0;
    if (g_outId.empty()) { text = S(StNoCable); tone = 3; }
    else if (ps == 2 && !pe.empty()) { text = pe; tone = 3; }
    else if (ms == 2 && !me.empty()) { text = me; tone = 3; }
    else if (pRun || mRun) { text = S(StMixing); tone = 2; }
    else { text = g_hint.empty() ? S(StReady) : g_hint; tone = 0; }
    if (text != lastStatus || tone != g_statusTone) {
        lastStatus = text;
        setStatus(text, tone);
    }
}

void toggleProc(HWND) {
    if (wantProc.load()) { wantProc = 0; stateProc = 0; updateLaneUi(); return; }
    if (g_outId.empty()) { setStatus(S(StNoCable2), 3); return; }
    std::vector<Target> picks;
    std::wstring error;
    if (!pickedTargets(picks, error)) { setStatus(error, 3); return; }
    std::wstring gtext = textOf(procGain);
    wchar_t* end = nullptr;
    long gain = wcstol(gtext.c_str(), &end, 10);
    if (end == gtext.c_str() || gain < 0 || gain > 200) { setStatus(S(StGainApp), 3); return; }
    EnterCriticalSection(&cfgLock);
    cfgProcPids.clear();
    for (const auto& t : picks) cfgProcPids.push_back(t.pid);
    LeaveCriticalSection(&cfgLock);
    gainProc = (float)(gain / 100.0);
    EnterCriticalSection(&errLock);
    errProc.clear();
    LeaveCriticalSection(&errLock);
    stateProc = 0;
    wantProc = 1;
    updateLaneUi();
}

void toggleMic(HWND) {
    if (wantMic.load()) { wantMic = 0; stateMic = 0; updateLaneUi(); return; }
    if (g_outId.empty()) { setStatus(S(StNoCable2), 3); return; }
    int sel = (int)SendMessageW(micBox, CB_GETCURSEL, 0, 0);
    if (sel < 0 || sel >= (int)g_micIds.size()) { setStatus(S(StPickMic), 3); return; }
    std::wstring gtext = textOf(micGain);
    wchar_t* end = nullptr;
    long gain = wcstol(gtext.c_str(), &end, 10);
    if (end == gtext.c_str() || gain < 0 || gain > 200) { setStatus(S(StGainMic), 3); return; }
    EnterCriticalSection(&cfgLock);
    cfgMicId = g_micIds[sel];
    LeaveCriticalSection(&cfgLock);
    gainMic = (float)(gain / 100.0);
    EnterCriticalSection(&errLock);
    errMic.clear();
    LeaveCriticalSection(&errLock);
    stateMic = 0;
    wantMic = 1;
    updateLaneUi();
}

/* ---------- painting helpers ---------- */
void fillRound(HDC dc, RECT r, int radius, COLORREF fill, COLORREF line) {
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 1, line);
    HGDIOBJ ob = SelectObject(dc, b);
    HGDIOBJ op = SelectObject(dc, p);
    RoundRect(dc, r.left, r.top, r.right, r.bottom, radius, radius);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(b);
    DeleteObject(p);
}

void paintChrome(HWND w, HDC dc) {
    RECT rc;
    GetClientRect(w, &rc);
    FillRect(dc, &rc, bPaper);
    RECT c1{D(B_CARD1.x), D(B_CARD1.y), D(B_CARD1.x + B_CARD1.w), D(B_CARD1.y + B_CARD1.h)};
    RECT c2{D(B_CARD2.x), D(B_CARD2.y), D(B_CARD2.x + B_CARD2.w), D(B_CARD2.y + B_CARD2.h)};
    fillRound(dc, c1, D(6), C_CARD, C_LINE2);
    fillRound(dc, c2, D(6), C_CARD, C_LINE2);
    RECT bar{D(B_BAR.x), D(B_BAR.y), D(B_BAR.x + B_BAR.w), D(B_BAR.y + B_BAR.h)};
    HBRUSH bb = CreateSolidBrush(C_BAR);
    FillRect(dc, &bar, bb);
    DeleteObject(bb);
    RECT accent{bar.left, bar.top, bar.left + D(3), bar.bottom};
    COLORREF ac = g_statusTone == 3 ? C_RED : (g_statusTone == 2 ? C_GREEN : C_LINE);
    HBRUSH ab = CreateSolidBrush(ac);
    FillRect(dc, &accent, ab);
    DeleteObject(ab);
}

void drawPrimaryButton(const DRAWITEMSTRUCT* d) {
    bool running = (d->CtlID == ID_MIC_BTN ? wantMic : wantProc).load() != 0;
    bool disabled = (d->itemState & ODS_DISABLED) != 0;
    bool pressed = (d->itemState & ODS_SELECTED) != 0;
    RECT r = d->rcItem;
    COLORREF fill, line, text;
    if (disabled) { fill = C_DIS_BG; line = C_DIS_LN; text = C_DIS_INK; }
    else if (running) { fill = C_PAPER; line = C_ACCENT; text = C_INK; }
    else { fill = pressed ? C_ACC_LO : C_ACCENT; line = fill; text = RGB(255, 253, 246); }
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, running ? D(2) : 1, line);
    HGDIOBJ ob = SelectObject(d->hDC, b);
    HGDIOBJ op = SelectObject(d->hDC, p);
    RoundRect(d->hDC, r.left, r.top, r.right, r.bottom, D(4), D(4));
    SelectObject(d->hDC, ob);
    SelectObject(d->hDC, op);
    DeleteObject(b);
    DeleteObject(p);
    if (!disabled && (d->itemState & ODS_FOCUS)) {
        RECT f = r;
        InflateRect(&f, -D(5), -D(5));
        HBRUSH fb = CreateSolidBrush(line);
        FrameRect(d->hDC, &f, fb);
        DeleteObject(fb);
    }
    SetBkMode(d->hDC, TRANSPARENT);
    SetTextColor(d->hDC, text);
    SelectObject(d->hDC, fBold);
    std::wstring label = textOf(d->hwndItem);
    DrawTextW(d->hDC, label.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

void drawSecondaryButton(const DRAWITEMSTRUCT* d, bool selected) {
    bool disabled = (d->itemState & ODS_DISABLED) != 0;
    bool pressed = (d->itemState & ODS_SELECTED) != 0;
    RECT r = d->rcItem;
    COLORREF fill = disabled ? C_DIS_BG : (selected ? C_ACCENT : (pressed ? RGB(243, 238, 228) : C_CARD));
    COLORREF line = disabled ? C_DIS_LN : (selected ? C_ACCENT : C_LINE);
    COLORREF text = disabled ? C_DIS_INK : (selected ? RGB(255, 253, 246) : C_INK);
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 1, line);
    HGDIOBJ ob = SelectObject(d->hDC, b);
    HGDIOBJ op = SelectObject(d->hDC, p);
    RoundRect(d->hDC, r.left, r.top, r.right, r.bottom, D(4), D(4));
    SelectObject(d->hDC, ob);
    SelectObject(d->hDC, op);
    DeleteObject(b);
    DeleteObject(p);
    if (!disabled && (d->itemState & ODS_FOCUS)) {
        RECT f = r;
        InflateRect(&f, -D(4), -D(4));
        HBRUSH fb = CreateSolidBrush(C_ACCENT);
        FrameRect(d->hDC, &f, fb);
        DeleteObject(fb);
    }
    SetBkMode(d->hDC, TRANSPARENT);
    SetTextColor(d->hDC, text);
    SelectObject(d->hDC, fBody);
    std::wstring label = textOf(d->hwndItem);
    DrawTextW(d->hDC, label.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

void drawInfoRow(const DRAWITEMSTRUCT* d) {
    static const wchar_t* kIn = L"CABLE Input";
    static const wchar_t* kOut = L"CABLE Output";
    RECT r = d->rcItem;
    SetBkMode(d->hDC, TRANSPARENT);
    int x = r.left;
    SelectObject(d->hDC, fSmall);
    struct Seg { const wchar_t* t; COLORREF c; HFONT f; };
    Seg segs[4] = {
        {S(InfoLeft), C_INK2, fSmall},
        {kIn, C_ACCENT, fBold},
        {S(InfoMid), C_INK2, fSmall},
        {kOut, C_ACCENT, fBold},
    };
    for (int i = 0; i < 4; ++i) {
        SelectObject(d->hDC, segs[i].f);
        SetTextColor(d->hDC, segs[i].c);
        SIZE s{};
        std::wstring t = segs[i].t;
        GetTextExtentPoint32W(d->hDC, t.c_str(), (int)t.length(), &s);
        RECT cell{x, r.top, x + s.cx + D(2), r.bottom};
        DrawTextW(d->hDC, t.c_str(), -1, &cell, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        x += s.cx + D(10);
    }
}

/* ---------- layout ---------- */
void layoutAll(HWND w) {
    auto place = [&](int id, Box b) {
        HWND c = GetDlgItem(w, id);
        if (c) SetWindowPos(c, nullptr, D(b.x), D(b.y), D(b.w), D(b.h), SWP_NOZORDER | SWP_NOACTIVATE);
    };
    place(ID_TITLE,         {24,  24, 560, 32});
    place(ID_SUBTITLE,      {24,  64, 700, 20});
    place(ID_LANG_ZH,       {752, 24,  48, 32});
    place(ID_LANG_EN,       {808, 24,  48, 32});
    place(ID_SEC1_NO,       {40, 112,  24, 24});
    place(ID_SEC1_TITLE,    {72, 112, 320, 24});
    place(ID_PILL_APP,      {744,112,  96, 24});
    place(ID_LBL_SOURCE,    {40, 148,  96, 24});
    place(ID_REFRESH,       {720,144, 120, 32});
    place(ID_CHK_ALL,       {144,296, 480, 24});
    place(ID_LBL_GAIN_APP,  {40, 342,  96, 20});
    place(ID_RANGE_APP,     {224,342,  64, 20});
    place(ID_PROC_BTN,      {640,332, 200, 40});
    place(ID_SEC2_NO,       {40, 416,  24, 24});
    place(ID_SEC2_TITLE,    {72, 416, 320, 24});
    place(ID_PILL_MIC,      {744,416,  96, 24});
    place(ID_LBL_DEVICE,    {40, 452,  96, 24});
    place(ID_LBL_GAIN_MIC,  {40, 498,  96, 20});
    place(ID_RANGE_MIC,     {224,498,  64, 20});
    place(ID_MIC_BTN,       {640,488, 200, 40});
    place(ID_INFO,          {24, 560, 832, 20});
    place(ID_STATUS,        {44, 602, 792, 28});
    place(ID_HINT,          {24, 704, 832, 32});

    /* app list, and the mic combo keeps its own drop height */
    HWND c = GetDlgItem(w, ID_PROC_BOX);
    if (c) SetWindowPos(c, nullptr, D(144), D(144), D(568), D(146), SWP_NOZORDER | SWP_NOACTIVATE);
    c = GetDlgItem(w, ID_MIC_BOX);
    if (c) SetWindowPos(c, nullptr, D(144), D(448), D(696), D(200), SWP_NOZORDER | SWP_NOACTIVATE);
    place(ID_PROC_GAIN,     {144,336,  72, 32});
    place(ID_MIC_GAIN,      {144,492,  72, 32});

    /* bottom row: chained, width measured from localized text */
    int x = 24;
    struct { int id; StrId text; } btns[3] = {
        {ID_DETECT, BtnDetect}, {ID_SOUND, BtnSound}, {ID_LOGS, BtnLogs}
    };
    for (auto& it : btns) {
        int need = measureText(w, fBody, S(it.text)) + D(32);
        if (need < D(120)) need = D(120);
        HWND b = GetDlgItem(w, it.id);
        if (b) SetWindowPos(b, nullptr, D(x), D(656), need, D(32), SWP_NOZORDER | SWP_NOACTIVATE);
        x += MulDiv(need, 96, g_dpi) + 8;
    }
}

void applyLanguage(HWND w) {
    struct { int id; StrId text; } map[] = {
        {ID_TITLE, Title}, {ID_SUBTITLE, Subtitle},
        {ID_SEC1_TITLE, SecApp}, {ID_SEC2_TITLE, SecMic},
        {ID_LBL_SOURCE, LblSource}, {ID_LBL_GAIN_APP, LblVolume},
        {ID_LBL_DEVICE, LblDevice}, {ID_LBL_GAIN_MIC, LblVolume},
        {ID_REFRESH, BtnRefresh}, {ID_CHK_ALL, ChkAll},
        {ID_RANGE_APP, RangeHint}, {ID_RANGE_MIC, RangeHint},
        {ID_DETECT, BtnDetect}, {ID_SOUND, BtnSound}, {ID_LOGS, BtnLogs},
        {ID_HINT, Hint},
    };
    for (auto& m : map) {
        HWND c = GetDlgItem(w, m.id);
        if (!c) continue;
        HFONT f = fBody;
        if (m.id == ID_TITLE) f = fTitle;
        else if (m.id == ID_SUBTITLE || m.id == ID_SEC1_TITLE || m.id == ID_SEC2_TITLE) f = fSec;
        else if (m.id == ID_RANGE_APP || m.id == ID_RANGE_MIC || m.id == ID_HINT) f = fSmall;
        SendMessageW(c, WM_SETFONT, (WPARAM)f, TRUE);
        SetWindowTextW(c, S(m.text));
    }
    HWND c = GetDlgItem(w, ID_SEC1_NO);
    if (c) { SendMessageW(c, WM_SETFONT, (WPARAM)fBold, TRUE); SetWindowTextW(c, L"01"); }
    c = GetDlgItem(w, ID_SEC2_NO);
    if (c) { SendMessageW(c, WM_SETFONT, (WPARAM)fBold, TRUE); SetWindowTextW(c, L"02"); }

    /* everything that draws text must switch to the new fonts before the old ones die */
    int rest[][2] = {
        {ID_PILL_APP, 0}, {ID_PILL_MIC, 0}, {ID_STATUS, 0},
        {ID_PROC_GAIN, 1}, {ID_MIC_GAIN, 1},
        {ID_PROC_BOX, 0}, {ID_MIC_BOX, 0}, {ID_INFO, 2},
    };
    for (auto& r : rest) {
        HWND h = GetDlgItem(w, r[0]);
        if (!h) continue;
        HFONT ff = r[1] == 1 ? fBold : (r[1] == 2 ? fSmall : fBody);
        SendMessageW(h, WM_SETFONT, (WPARAM)ff, TRUE);
    }
    flushRetiredFonts();
    InvalidateRect(w, nullptr, FALSE);
    UpdateWindow(w);
    layoutAll(w);
    /* refresh combo entries so labels/categories follow the language */
    if (!wantProc.load()) refreshProcesses();
    updateLaneUi();
}

void drawComboEntry(const DRAWITEMSTRUCT* d) {
    if (!d || d->itemID == (UINT)-1) return;
    wchar_t buf[512]{};
    SendMessageW(d->hwndItem, CB_GETLBTEXT, (WPARAM)d->itemID, (LPARAM)buf);
    std::wstring t = buf;
    bool sel = (d->itemState & ODS_SELECTED) != 0;
    RECT r = d->rcItem;
    if (sel) {
        HBRUSH b = CreateSolidBrush(C_ACCENT);
        FillRect(d->hDC, &r, b);
        DeleteObject(b);
    } else {
        HBRUSH b = CreateSolidBrush(C_FIELD);
        FillRect(d->hDC, &r, b);
        DeleteObject(b);
    }
    SetBkMode(d->hDC, TRANSPARENT);
    SetTextColor(d->hDC, sel ? RGB(255, 253, 246) : C_INK);
    SelectObject(d->hDC, fBody);
    RECT tr = r;
    tr.left += D(6);
    tr.right -= D(4);
    DrawTextW(d->hDC, t.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}

LRESULT CALLBACK windowProc(HWND w, UINT m, WPARAM a, LPARAM b) {
    switch (m) {
    case WM_CREATE: {
        g_dpi = 96;
        {
            typedef int (WINAPI* Fn)(HWND);
            HMODULE u = GetModuleHandleW(L"user32.dll");
            auto p = u ? (Fn)GetProcAddress(u, "GetDpiForWindow") : nullptr;
            if (p) { int v = p(w); if (v >= 96) g_dpi = v; }
        }
        rebuildFonts();

        HWND c;
        c = make(w, L"STATIC", S(Title), {24, 24, 560, 32}, ID_TITLE, SS_LEFT, fTitle);
        (void)c;
        make(w, L"STATIC", S(Subtitle), {24, 64, 700, 20}, ID_SUBTITLE, SS_LEFT | SS_CENTERIMAGE, fSec);
        langZh = make(w, L"BUTTON", L"中文", {752, 24, 48, 32}, ID_LANG_ZH, BS_OWNERDRAW | WS_TABSTOP | WS_GROUP);
        langEn = make(w, L"BUTTON", L"EN", {808, 24, 48, 32}, ID_LANG_EN, BS_OWNERDRAW | WS_TABSTOP);
        make(w, L"STATIC", L"01", {40, 112, 24, 24}, ID_SEC1_NO, SS_LEFT | SS_CENTERIMAGE, fSmall);
        make(w, L"STATIC", S(SecApp), {72, 112, 320, 24}, ID_SEC1_TITLE, SS_LEFT | SS_CENTERIMAGE, fSec);
        pillApp = make(w, L"STATIC", S(StStopped), {744, 112, 96, 24}, ID_PILL_APP, SS_CENTER | SS_CENTERIMAGE, fBody);
        make(w, L"STATIC", S(LblSource), {40, 148, 96, 24}, ID_LBL_SOURCE, SS_LEFT | SS_CENTERIMAGE, fBody);
        procBox = make(w, WC_LISTVIEW, L"", {144, 144, 568, 146}, ID_PROC_BOX,
            LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS | WS_BORDER | WS_TABSTOP);
        ListView_SetExtendedListViewStyle(procBox,
            LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        {
            LVCOLUMNW col{};
            col.mask = LVCF_WIDTH;
            col.cx = D(520);
            SendMessageW(procBox, LVM_INSERTCOLUMNW, 0, (LPARAM)&col);
        }
        btnRefresh = make(w, L"BUTTON", S(BtnRefresh), {720, 144, 120, 32}, ID_REFRESH, BS_OWNERDRAW | WS_TABSTOP);
        showAll = make(w, L"BUTTON", S(ChkAll), {144, 184, 480, 24}, ID_CHK_ALL, BS_AUTOCHECKBOX | WS_TABSTOP);
        make(w, L"STATIC", S(LblVolume), {40, 226, 96, 20}, ID_LBL_GAIN_APP, SS_LEFT | SS_CENTERIMAGE, fBody);
        procGain = make(w, L"EDIT", L"100", {144, 336, 72, 32}, ID_PROC_GAIN, WS_BORDER | ES_NUMBER | ES_CENTER | ES_AUTOHSCROLL | WS_TABSTOP, fBold);
        make(w, L"STATIC", S(RangeHint), {224, 226, 64, 20}, ID_RANGE_APP, SS_LEFT | SS_CENTERIMAGE, fSmall);
        procButton = make(w, L"BUTTON", S(BtnStartApp), {640, 216, 200, 40}, ID_PROC_BTN, BS_OWNERDRAW | WS_TABSTOP | WS_GROUP);

        make(w, L"STATIC", L"02", {40, 304, 24, 24}, ID_SEC2_NO, SS_LEFT | SS_CENTERIMAGE, fSmall);
        make(w, L"STATIC", S(SecMic), {72, 304, 320, 24}, ID_SEC2_TITLE, SS_LEFT | SS_CENTERIMAGE, fSec);
        pillMic = make(w, L"STATIC", S(StStopped), {744, 304, 96, 24}, ID_PILL_MIC, SS_CENTER | SS_CENTERIMAGE, fBody);
        make(w, L"STATIC", S(LblDevice), {40, 340, 96, 24}, ID_LBL_DEVICE, SS_LEFT | SS_CENTERIMAGE, fBody);
        micBox = make(w, L"COMBOBOX", L"", {144, 336, 696, 200}, ID_MIC_BOX,
            CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP);
        make(w, L"STATIC", S(LblVolume), {40, 386, 96, 20}, ID_LBL_GAIN_MIC, SS_LEFT | SS_CENTERIMAGE, fBody);
        micGain = make(w, L"EDIT", L"100", {144, 380, 72, 32}, ID_MIC_GAIN, WS_BORDER | ES_NUMBER | ES_CENTER | ES_AUTOHSCROLL | WS_TABSTOP, fBold);
        make(w, L"STATIC", S(RangeHint), {224, 386, 64, 20}, ID_RANGE_MIC, SS_LEFT | SS_CENTERIMAGE, fSmall);
        micButton = make(w, L"BUTTON", S(BtnStartMic), {640, 376, 200, 40}, ID_MIC_BTN, BS_OWNERDRAW | WS_TABSTOP | WS_GROUP);

        make(w, L"STATIC", L"", {24, 448, 832, 20}, ID_INFO, SS_OWNERDRAW, fSmall);
        statusText = make(w, L"STATIC", S(StReady), {44, 490, 792, 28}, ID_STATUS, SS_LEFT | SS_CENTERIMAGE, fBody);
        btnDetect = make(w, L"BUTTON", S(BtnDetect), {24, 544, 120, 32}, ID_DETECT, BS_OWNERDRAW | WS_TABSTOP | WS_GROUP);
        btnSound = make(w, L"BUTTON", S(BtnSound), {152, 544, 136, 32}, ID_SOUND, BS_OWNERDRAW | WS_TABSTOP);
        btnLogs = make(w, L"BUTTON", S(BtnLogs), {296, 544, 136, 32}, ID_LOGS, BS_OWNERDRAW | WS_TABSTOP);
        make(w, L"STATIC", S(Hint), {24, 592, 832, 32}, ID_HINT, SS_LEFT | SS_CENTERIMAGE, fSmall);

        /* combo metrics */
        SendMessageW(micBox, CB_SETITEMHEIGHT, (WPARAM)-1, D(22));
        SendMessageW(micBox, CB_SETITEMHEIGHT, (WPARAM)0, D(24));

        loadPicked();
        layoutAll(w);
        refreshDevices();
        refreshProcesses();
        updateLaneUi();
        SetTimer(w, 1, 400, nullptr);
        return 0;
    }
    case WM_APP + 1:
        updateLaneUi();
        return 0;
    case WM_DPICHANGED: {
        g_dpi = HIWORD(a) ? HIWORD(a) : 96;
        rebuildFonts();
        RECT* r = (RECT*)b;
        SetWindowPos(w, nullptr, r->left, r->top, r->right - r->left, r->bottom - r->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        applyLanguage(w);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(a);
        if (HIWORD(a) == BN_CLICKED) {
            switch (id) {
            case ID_REFRESH: refreshProcesses(); return 0;
            case ID_CHK_ALL: refreshProcesses(); return 0;
            case ID_PROC_BTN: toggleProc(w); return 0;
            case ID_MIC_BTN: toggleMic(w); return 0;
            case ID_DETECT: refreshDevices(); refreshProcesses(); updateLaneUi(); return 0;
            case ID_SOUND: ShellExecuteW(w, L"open", L"control.exe", L"mmsys.cpl,,1", nullptr, SW_SHOWNORMAL); return 0;
            case ID_LOGS: {
                std::wstring logs = g_root + L"\\logs";
                CreateDirectoryW(logs.c_str(), nullptr);
                ShellExecuteW(w, L"open", logs.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                return 0;
            }
            case ID_LANG_ZH:
            case ID_LANG_EN: {
                int want = id == ID_LANG_EN ? 1 : 0;
                if (want != g_lang) {
                    g_lang = want;
                    std::wstring ini = g_root + L"\\settings.ini";
                    WritePrivateProfileStringW(L"ui", L"lang", g_lang ? L"en" : L"zh", ini.c_str());
                    applyLanguage(w);
                    InvalidateRect(langZh, nullptr, FALSE);
                    InvalidateRect(langEn, nullptr, FALSE);
                }
                return 0;
            }
            }
        }
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(w, &ps);
        RECT rc;
        GetClientRect(w, &rc);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
        HGDIOBJ old = SelectObject(mem, bmp);
        paintChrome(w, mem);
        BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(w, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_TIMER: updateLaneUi(); return 0;
    case WM_NOTIFY: {
        auto h = (LPNMHDR)b;
        if (h && h->idFrom == ID_PROC_BOX && h->code == LVN_ITEMCHANGED) {
            if (g_syncing) return 0;      /* the list is being rebuilt, ignore its notifications */
            syncPicked();
            updateLaneUi();
            return 0;
        }
        break;
    }
    case WM_DRAWITEM: {
        auto d = (DRAWITEMSTRUCT*)b;
        switch (d->CtlID) {
        case ID_PROC_BTN: case ID_MIC_BTN: drawPrimaryButton(d); return TRUE;
        case ID_LANG_ZH: drawSecondaryButton(d, g_lang == 0); return TRUE;
        case ID_LANG_EN: drawSecondaryButton(d, g_lang == 1); return TRUE;
        case ID_REFRESH: case ID_DETECT: case ID_SOUND: case ID_LOGS:
            drawSecondaryButton(d, false); return TRUE;
        case ID_INFO: drawInfoRow(d); return TRUE;
        case ID_PROC_BOX: break;                 /* the list view paints itself */
        case ID_MIC_BOX:
            if (d->itemID == (UINT)-1) return TRUE;   /* never let the system paint a white block */
            drawComboEntry(d);
            return TRUE;
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HWND hc = (HWND)b;
        int id = GetDlgCtrlID(hc);
        HBRUSH back = bPaper;
        COLORREF fg = C_INK;
        COLORREF bg = C_PAPER;
        switch (id) {
        case ID_TITLE: fg = C_INK; break;
        case ID_SUBTITLE: fg = C_INK2; break;
        case ID_SEC1_NO: case ID_SEC2_NO: fg = C_ACCENT; back = bCard; bg = C_CARD; break;
        case ID_SEC1_TITLE: case ID_SEC2_TITLE: fg = C_INK; back = bCard; bg = C_CARD; break;
        case ID_LBL_SOURCE: case ID_LBL_DEVICE: case ID_LBL_GAIN_APP: case ID_LBL_GAIN_MIC:
            fg = C_INK2; back = bCard; bg = C_CARD; break;
        case ID_RANGE_APP: case ID_RANGE_MIC:
            fg = C_INK3; back = bCard; bg = C_CARD; break;
        case ID_HINT: fg = C_INK3; break;
        case ID_CHK_ALL:
            fg = C_INK2; back = bCard; bg = C_CARD; break;
        case ID_STATUS:
            back = bBar;
            fg = g_statusTone == 3 ? C_RED : (g_statusTone == 2 ? C_GREEN : C_INK2);
            break;
        case ID_PILL_APP:
        case ID_PILL_MIC: {
            int tone = id == ID_PILL_APP ? g_toneProc : g_toneMic;
            back = tone == 3 ? bPillErr : (tone == 2 ? bPillRun : (tone == 1 ? bPillWrk : bPillOff));
            fg = tone == 3 ? C_RED : (tone == 2 ? C_GREEN : (tone == 1 ? C_AMBER : C_INK2));
            break;
        }
        }
        SetTextColor((HDC)a, fg);
        SetBkColor((HDC)a, bg);
        return (LRESULT)back;
    }
    case WM_CTLCOLORBTN:
        /* the check box lives on a card, keep it off the system gray */
        SetBkColor((HDC)a, C_CARD);
        return (LRESULT)bCard;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        SetTextColor((HDC)a, C_INK);
        SetBkColor((HDC)a, C_FIELD);
        return (LRESULT)bField;
    case WM_CLOSE:
        DestroyWindow(w);
        return 0;
    case WM_DESTROY:
        KillTimer(w, 1);
        wantProc = 0;
        wantMic = 0;
        engineStop = true;   /* join happens in wWinMain, before anything is torn down */
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(w, m, a, b);
}

int WINAPI wWinMain(HINSTANCE h, HINSTANCE, LPWSTR, int show) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    {
        typedef BOOL (WINAPI* SetCtx)(HANDLE);
        HMODULE u = GetModuleHandleW(L"user32.dll");
        auto p = u ? (SetCtx)GetProcAddress(u, "SetProcessDpiAwarenessContext") : nullptr;
        bool ok = p && p((HANDLE)-4); /* PER_MONITOR_AWARE_V2 */
        if (!ok) SetProcessDPIAware();
    }
    InitializeCriticalSection(&cfgLock);
    InitializeCriticalSection(&errLock);
    {
        INITCOMMONCONTROLSEX icc{};
        icc.dwSize = sizeof(icc);
        icc.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
        InitCommonControlsEx(&icc);
    }
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    g_root = path;
    size_t slash = g_root.find_last_of(L'\\');
    if (slash != std::wstring::npos) g_root = g_root.substr(0, slash);
    {
        std::wstring ini = g_root + L"\\settings.ini";
        g_lang = GetPrivateProfileIntW(L"ui", L"lang", 0, ini.c_str()) ? 1 : 0;
        if (!g_lang) {
            wchar_t buf[16]{};
            GetPrivateProfileStringW(L"ui", L"lang", L"zh", buf, 16, ini.c_str());
            g_lang = _wcsicmp(buf, L"en") == 0 ? 1 : 0;
        }
    }

    if (std::wstring(GetCommandLineW()).find(L"--dump") != std::wstring::npos) {
        std::vector<Row> rows;
        buildRows(true, rows);
        std::wstring dir = g_root + L"\\logs";
        CreateDirectoryW(dir.c_str(), nullptr);
        std::wstring file = dir + L"\\discovery.txt";
        std::string out = "rows=" + std::to_string(rows.size()) + "\n";
        {
            HANDLE s2 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            int raw = 0;
            PROCESSENTRY32W p2{};
            p2.dwSize = sizeof(p2);
            if (s2 != INVALID_HANDLE_VALUE) {
                if (Process32FirstW(s2, &p2)) do { ++raw; } while (Process32NextW(s2, &p2));
                CloseHandle(s2);
            }
            out += "raw=" + std::to_string(raw) + "\n";
        }
        for (const auto& r : rows) {
            int n = WideCharToMultiByte(CP_UTF8, 0, r.label.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string line(n > 0 ? n - 1 : 0, '\0');
            if (n > 1) WideCharToMultiByte(CP_UTF8, 0, r.label.c_str(), -1, line.data(), n, nullptr, nullptr);
            out += line + "\n";
            /* the tick key the settings file stores, handy when debugging selection */
            std::wstring key = targetKey(r.target);
            int m = WideCharToMultiByte(CP_UTF8, 0, key.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string kline(m > 0 ? m - 1 : 0, '\0');
            if (m > 1) WideCharToMultiByte(CP_UTF8, 0, key.c_str(), -1, kline.data(), m, nullptr, nullptr);
            out += "  key=" + kline + "\n";
        }
        FILE* f = _wfopen(file.c_str(), L"wb");
        if (f) { fwrite(out.data(), 1, out.size(), f); fclose(f); }
        CoUninitialize();
        return 0;
    }

    std::wstring cmdline = GetCommandLineW();
    if (cmdline.find(L"--route") != std::wstring::npos) {
        std::vector<DWORD> pids;
        int secs = 5;
        int micIndex = -1;
        size_t pos = cmdline.find(L"--route");
        std::wstring spec = cmdline.substr(pos + 7);
        /* --route 1234,5678 5 [micIndex] : every listed pid becomes its own lane */
        if (!spec.empty() && spec[0] == L' ') spec = spec.substr(1);
        while (!spec.empty() && (iswdigit(spec[0]) || spec[0] == L',')) {
            wchar_t* end = nullptr;
            unsigned long v = wcstoul(spec.c_str(), &end, 10);
            if (end == spec.c_str()) break;
            if (v) pids.push_back((DWORD)v);
            spec = std::wstring(end);
            if (!spec.empty() && spec[0] == L',') spec = spec.substr(1);
            else break;
        }
        {
            wchar_t* end = nullptr;
            long s = wcstol(spec.c_str(), &end, 10);
            if (end != spec.c_str()) {
                if (s >= 1) secs = (int)s;
                spec = std::wstring(end);
                long mi = wcstol(spec.c_str(), &end, 10);
                if (end != spec.c_str()) micIndex = (int)mi;
            }
        }
        if (pids.size() > (size_t)kMaxApps) pids.resize(kMaxApps);
        std::wstring out = findCableInput();
        EnterCriticalSection(&cfgLock);
        cfgOutId = out;
        cfgProcPids = pids;
        LeaveCriticalSection(&cfgLock);
        gainProc = 1.0f;
        std::wstring idlist;
        for (size_t i = 0; i < pids.size(); ++i) { if (i) idlist += L","; idlist += std::to_wstring(pids[i]); }
        std::wstring report = L"pid=" + idlist + L" cable_input=" + (out.empty() ? L"MISSING" : L"FOUND")
            + L" lanes=" + std::to_wstring(pids.size()) + L" src_format=" + (g_srcFormat.empty() ? L"?" : g_srcFormat) + L"\n";
        if (!out.empty() && !pids.empty()) {
            wantProc = 1;
            if (micIndex >= 0) {
                Com<IMMDeviceEnumerator> en;
                if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(en.put())))) {
                    Com<IMMDeviceCollection> coll;
                    if (SUCCEEDED(en->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, coll.put()))) {
                        Com<IMMDevice> d;
                        if (SUCCEEDED(coll->Item((UINT)micIndex, d.put()))) {
                            LPWSTR id = nullptr;
                            if (SUCCEEDED(d->GetId(&id))) {
                                EnterCriticalSection(&cfgLock);
                                cfgMicId = id;
                                LeaveCriticalSection(&cfgLock);
                                CoTaskMemFree(id);
                                wantMic = 1;
                            }
                        }
                    }
                }
            }
            HANDLE th = CreateThread(nullptr, 0, engineThread, nullptr, 0, nullptr);
            if (!th) { report += L"engine_thread=FAILED\n"; }
            for (int i = 0; i < secs * 10; ++i) Sleep(100);
            report += L"captured_frames=" + std::to_wstring(g_apps[0].captured.load())
                + L" peak=" + std::to_wstring(g_apps[0].peak.load())
                + L" queued=" + std::to_wstring(g_apps[0].queued.load())
                + L" open=" + std::to_wstring(g_apps[0].open ? 1 : 0) + L"\n";
            if (pids.size() > 1) {
                for (size_t i = 1; i < pids.size(); ++i)
                    report += L"lane" + std::to_wstring(i) + L"_frames=" + std::to_wstring(g_apps[i].captured.load())
                        + L" lane" + std::to_wstring(i) + L"_peak=" + std::to_wstring(g_apps[i].peak.load())
                        + L" lane" + std::to_wstring(i) + L"_open=" + std::to_wstring(g_apps[i].open ? 1 : 0) + L"\n";
            }
            if (micIndex >= 0)
                report += L"mic_frames=" + std::to_wstring(g_mic.captured.load())
                    + L" mic_peak=" + std::to_wstring(g_mic.peak.load())
                    + L" mic_open=" + std::to_wstring(g_mic.open ? 1 : 0) + L"\n";
            wantProc = 0;
            wantMic = 0;
            Sleep(400);
            engineStop = true;
            if (th) { WaitForSingleObject(th, 5000); CloseHandle(th); }
        }
        std::wstring dir = g_root + L"\\logs";
        CreateDirectoryW(dir.c_str(), nullptr);
        std::wstring file = dir + L"\\route-test.txt";
        int n = WideCharToMultiByte(CP_UTF8, 0, report.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string bytes(n > 0 ? n - 1 : 0, '\0');
        if (n > 1) WideCharToMultiByte(CP_UTF8, 0, report.c_str(), -1, bytes.data(), n, nullptr, nullptr);
        FILE* f = _wfopen(file.c_str(), L"wb");
        if (f) { fwrite(bytes.data(), 1, bytes.size(), f); fclose(f); }
        CoUninitialize();
        return 0;
    }

    bPaper   = CreateSolidBrush(C_PAPER);
    bCard    = CreateSolidBrush(C_CARD);
    bField   = CreateSolidBrush(C_FIELD);
    bBar     = CreateSolidBrush(C_BAR);
    bPillOff = CreateSolidBrush(C_PILL_OFF);
    bPillRun = CreateSolidBrush(C_PILL_RUN);
    bPillWrk = CreateSolidBrush(C_PILL_WRK);
    bPillErr = CreateSolidBrush(C_PILL_ERR);

    WNDCLASSW wc{};
    wc.hInstance = h;
    wc.lpszClassName = L"MicTapWnd";
    wc.lpfnWndProc = windowProc;
    wc.hbrBackground = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassW(&wc);

    RECT r{0, 0, D(880), D(752)};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectEx(&r, style, FALSE, 0);
    mainWin = CreateWindowExW(0, wc.lpszClassName, S(Title), style | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, nullptr, nullptr, h, nullptr);
    if (!mainWin) return 1;
    audioThread = CreateThread(nullptr, 0, engineThread, nullptr, 0, nullptr);
    if (!audioThread) {
        MessageBoxW(mainWin, S(StLaunchFail), S(Title), MB_ICONERROR);
        DestroyWindow(mainWin);
    }
    ShowWindow(mainWin, show);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(mainWin, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    /* single join point: the engine thread must be gone before shared state dies */
    engineStop = true;
    if (audioThread) { WaitForSingleObject(audioThread, INFINITE); CloseHandle(audioThread); audioThread = nullptr; }
    if (fTitle) DeleteObject(fTitle);
    if (fSec) DeleteObject(fSec);
    if (fBody) DeleteObject(fBody);
    if (fSmall) DeleteObject(fSmall);
    if (fBold) DeleteObject(fBold);
    DeleteObject(bPaper);
    DeleteObject(bCard);
    DeleteObject(bField);
    DeleteObject(bBar);
    DeleteObject(bPillOff);
    DeleteObject(bPillRun);
    DeleteObject(bPillWrk);
    DeleteObject(bPillErr);
    DeleteCriticalSection(&cfgLock);
    DeleteCriticalSection(&errLock);
    CoUninitialize();
    return 0;
}
