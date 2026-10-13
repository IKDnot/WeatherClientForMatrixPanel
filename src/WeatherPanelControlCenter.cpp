#include "WeatherPanelControlCenter.h"

#include <WiFi.h>
#include <Arduino.h>
#include <stdio.h>

#include "DeviceConfiguration.h"
#include "DisplaySettings.h"
#include "P3RGB64x64MatrixPanel.h"

namespace {
constexpr char kHtmlContentType[] = "text/html; charset=utf-8";
constexpr char kJsonContentType[] = "application/json";
constexpr char kFrameBufferContentType[] = "application/octet-stream";
constexpr char kTextContentType[] = "text/plain; charset=utf-8";
constexpr char kCacheControlHeader[] = "Cache-Control";
constexpr char kNoStoreValue[] = "no-store";
constexpr uint8_t kPreviewAlphaValue = 255;
constexpr uint32_t kRuntimeStatusRefreshIntervalMs = 2000UL;
constexpr char kPage[] PROGMEM = R"HTML(
<!doctype html>
<html lang="ja">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#111827">
<title>Weather LED | コントロールセンター</title>
<style>
:root{color-scheme:dark;--bg:#0b1120;--card:#151f32;--line:#29364d;--text:#eef4ff;--muted:#a7b4c9;--accent:#68e0c0;--danger:#ff827e}
*{box-sizing:border-box}body{margin:0;min-height:100vh;background:radial-gradient(ellipse at 20% 0,#1a3650 0,transparent 48%),var(--bg);color:var(--text);font:16px/1.5 system-ui,-apple-system,"Hiragino Kaku Gothic ProN",Meiryo,sans-serif;display:grid;place-items:center;padding:24px}
main{width:min(100%,560px)}.brand{font-size:12px;letter-spacing:.16em;text-transform:uppercase;color:var(--accent);font-weight:750}.card{margin-top:14px;padding:clamp(22px,6vw,36px);border:1px solid var(--line);border-radius:22px;background:linear-gradient(145deg,rgba(24,36,56,.96),rgba(16,25,41,.97));box-shadow:0 24px 80px #0006}
h1{font-size:clamp(25px,6vw,33px);margin:8px 0 5px;letter-spacing:-.04em}.intro{color:var(--muted);margin:0 0 22px}.section-title{font-size:15px;font-weight:700;margin:23px 0 10px}.preview{padding:15px;border:1px solid var(--line);border-radius:14px;background:#0d1728}.preview-head{display:flex;justify-content:space-between;align-items:center;margin-bottom:12px;font-size:14px;font-weight:700}.live{font-size:11px;letter-spacing:.1em;color:var(--accent)}.preview-body{display:flex;align-items:center;gap:16px}.preview-canvas{display:block;width:min(46vw,192px);height:min(46vw,192px);max-width:192px;max-height:192px;background:#000;border:1px solid #35445c;image-rendering:pixelated}.preview-meta{font-size:12px;color:var(--muted);line-height:1.6}.download{margin-top:10px;border:1px solid #40516a;border-radius:9px;background:#18253a;color:var(--text);padding:8px 10px;font:inherit;font-size:12px;cursor:pointer}.download:hover{border-color:var(--accent)}
.runtime-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:14px}.runtime-item{min-width:0;padding:12px 14px;border:1px solid var(--line);border-radius:11px;background:#0d1728}.runtime-label{display:block;font-size:12px;color:var(--muted)}.runtime-value{display:block;margin-top:3px;font-size:14px;font-weight:700;color:var(--text);overflow-wrap:anywhere}.runtime-value.ok{color:var(--accent)}.runtime-value.error{color:var(--danger)}
.device-info{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-top:14px;padding:12px 14px;border:1px solid var(--line);border-radius:11px;background:#0d1728;color:var(--muted);font-size:13px}.device-info code{font:600 13px ui-monospace,SFMono-Regular,Menlo,monospace;color:var(--accent);letter-spacing:.04em}
.toggle-row{display:flex;justify-content:space-between;align-items:center;padding:17px 0;border-block:1px solid var(--line);gap:18px}.toggle-label{font-weight:700}.toggle-hint{font-size:13px;color:var(--muted);margin-top:3px}.switch{position:relative;width:54px;height:31px;flex:none}.switch input{opacity:0;width:0;height:0}.slider{position:absolute;inset:0;border-radius:30px;background:#465269;transition:.2s;cursor:pointer}.slider:before{content:"";position:absolute;width:23px;height:23px;left:4px;top:4px;border-radius:50%;background:#fff;transition:.2s}.switch input:checked+.slider{background:#24a98d}.switch input:checked+.slider:before{transform:translateX(23px)}
.times{display:grid;grid-template-columns:1fr 1fr;gap:14px;margin:20px 0}.field label{display:block;font-size:13px;color:var(--muted);margin-bottom:8px}.field select{width:100%;padding:13px 12px;border:1px solid var(--line);border-radius:11px;background:#0d1728;color:var(--text);font:inherit}.field select:focus{outline:2px solid var(--accent);outline-offset:2px}.field select[disabled]{opacity:.55;cursor:not-allowed}.hint{font-size:13px;color:var(--muted);margin:-5px 0 16px}.brightness{margin:15px 0 22px}.brightness-row{display:flex;justify-content:space-between;align-items:center;gap:12px;margin:14px 0 5px;font-size:13px;color:var(--muted)}.brightness-row output{font-weight:700;color:var(--accent);font-variant-numeric:tabular-nums}.brightness input[type=range]{width:100%;accent-color:var(--accent)}[hidden]{display:none!important}
.save{width:100%;border:0;border-radius:12px;padding:14px;background:var(--accent);color:#06251f;font:700 16px system-ui;cursor:pointer;transition:filter .15s,transform .15s}.save:hover{filter:brightness(1.08)}.save:active{transform:scale(.99)}.save:disabled{opacity:.55;cursor:wait}.status{min-height:24px;margin:14px 0 0;text-align:center;font-size:14px;color:var(--muted)}.status.ok{color:var(--accent)}.status.error{color:var(--danger)}.foot{font-size:12px;color:#8290a6;text-align:center;margin:18px 0 0}
</style>
</head>
<body><main><div class="brand">WEATHER LED · CONTROL CENTER</div><section class="card"><h1>お天気パネル設定</h1><p class="intro">天気表示モード、表示スケジュール、パネルの明るさを設定します。</p>
<div class="section-title">ライブプレビュー</div><section class="preview"><div class="preview-head"><span>パネル実画面</span><span class="live">LIVE · 64×64</span></div><div class="preview-body"><canvas id="panelPreview" class="preview-canvas" width="64" height="64" aria-label="LEDパネルの実画面"></canvas><div class="preview-meta"><div id="previewStatus">接続中…</div><button class="download" id="downloadPreview" type="button">PNGで保存</button></div></div></section>
<div class="runtime-grid" aria-live="polite"><div class="runtime-item"><span class="runtime-label">実機の表示状態</span><span class="runtime-value" id="runtimeDisplay">取得中…</span></div><div class="runtime-item"><span class="runtime-label">実機に表示中の天気</span><span class="runtime-value" id="runtimeWeather">取得中…</span></div><div class="runtime-item"><span class="runtime-label">最終更新</span><span class="runtime-value" id="runtimeUpdated">取得中…</span></div><div class="runtime-item"><span class="runtime-label">Wi-Fi</span><span class="runtime-value" id="runtimeWifi">取得中…</span></div></div>
<div class="device-info"><span>デバイス MAC</span><code id="macAddress">取得中…</code></div>
<form id="settings"><div class="section-title">天気表示設定</div><div class="times"><div class="field"><label for="weatherMode">表示する天気</label><select id="weatherMode"><option value="current">現在の天気</option><option value="forecast">時間先の予報</option></select></div><div class="field" id="forecastHoursField" hidden><label for="forecastHours">何時間後</label><select id="forecastHours"></select></div></div><p class="hint" id="forecastHint" hidden>予報は3時間ごとのデータから指定時刻に最も近いものを表示します。</p>
<div class="section-title">表示スケジュール</div><div class="toggle-row"><div><div class="toggle-label">自動ナイトモード</div><div class="toggle-hint">指定した時間帯はパネルを消灯</div></div><label class="switch"><input id="enabled" type="checkbox"><span class="slider"></span></label></div>
<div class="times"><div class="field"><label for="start">消灯開始</label><select id="start" name="start"></select></div><div class="field"><label for="end">再点灯</label><select id="end" name="end"></select></div></div><p class="hint">日付をまたぐ時間帯にも対応します。例：18:00〜翌6:00</p>
<div class="section-title">表示の明るさ</div><div class="brightness"><div class="brightness-row"><label for="dayBrightness">日中パネル</label><output id="dayBrightnessValue">100%</output></div><input id="dayBrightness" type="range" min="20" max="100" step="5" value="100"><div class="brightness-row"><label for="indicatorBrightness">夜間インジケーター</label><output id="indicatorBrightnessValue">3 / 15</output></div><input id="indicatorBrightness" type="range" min="0" max="15" step="1" value="3"></div>
<button class="save" id="save" type="submit">設定を保存</button><p class="status" id="status" role="status"></p></form></section><p class="foot">同じローカルネットワーク内からのみアクセスできます。</p></main>
<script>
const $=id=>document.getElementById(id),status=$('status'),save=$('save');
for(let hour=0;hour<24;hour++){const option=document.createElement('option');option.value=hour;option.textContent=String(hour).padStart(2,'0')+':00';$('start').append(option.cloneNode(true));$('end').append(option)}
for(let hour=3;hour<=120;hour+=3){const option=document.createElement('option');option.value=hour;option.textContent=hour+'時間後';$('forecastHours').append(option)}
function updateBrightnessLabels(){$('dayBrightnessValue').textContent=$('dayBrightness').value+'%';$('indicatorBrightnessValue').textContent=$('indicatorBrightness').value+' / 15'}
function updateForecastControls(){const forecastSelected=$('weatherMode').value==='forecast';$('forecastHoursField').hidden=!forecastSelected;$('forecastHours').disabled=!forecastSelected;$('forecastHint').hidden=!forecastSelected}
async function load(){try{const response=await fetch('/api/settings',{cache:'no-store'});if(!response.ok)throw Error('設定を取得できません');const config=await response.json();$('enabled').checked=config.enabled;$('start').value=config.startHour;$('end').value=config.endHour;$('dayBrightness').value=config.dayBrightnessPercent;$('indicatorBrightness').value=config.indicatorBrightness;$('weatherMode').value=config.forecastMode?'forecast':'current';$('forecastHours').value=config.forecastHours;updateBrightnessLabels();updateForecastControls()}catch(error){status.textContent=error.message;status.className='status error'}}
async function loadDeviceInfo(){try{const response=await fetch('/api/device',{cache:'no-store'});if(!response.ok)throw Error();const device=await response.json();$('macAddress').textContent=device.mac}catch(error){$('macAddress').textContent='取得できません'}}
async function updatePreview(){try{const response=await fetch('/api/framebuffer',{cache:'no-store'});if(!response.ok)throw Error();const pixels=await response.arrayBuffer();if(pixels.byteLength!==64*64*4)throw Error();const context=$('panelPreview').getContext('2d');context.putImageData(new ImageData(new Uint8ClampedArray(pixels),64,64),0,0);$('previewStatus').textContent='実機と同期中'}catch(error){$('previewStatus').textContent='プレビュー取得エラー'}}
async function updateRuntimeStatus(){try{const response=await fetch('/api/status',{cache:'no-store'});if(!response.ok)throw Error();const device=await response.json();$('runtimeDisplay').textContent=!device.timeSynchronized?'時刻同期待ち':device.nightModeActive?'夜間（インジケーター）':'日中表示';$('runtimeWeather').textContent=!device.weatherAvailable?'未取得':!device.lastUpdateSucceeded?'取得エラー':device.displayedForecast?device.displayedForecastHours+'時間後の予報':'現在の天気';$('runtimeWeather').className='runtime-value'+(device.lastUpdateSucceeded?' ok':device.weatherAvailable?' error':'');$('runtimeUpdated').textContent=!device.hasUpdated?'未取得':device.lastUpdateSucceeded?Math.floor(device.secondsSinceUpdate/60)+'分前':'直近の取得に失敗';$('runtimeWifi').textContent=device.wifiConnected?'接続中 · '+device.wifiRssi+' dBm':'未接続'}catch(error){$('runtimeDisplay').textContent='取得できません';$('runtimeWeather').textContent='取得できません';$('runtimeUpdated').textContent='取得できません';$('runtimeWifi').textContent='取得できません'}}
$('dayBrightness').addEventListener('input',updateBrightnessLabels);$('indicatorBrightness').addEventListener('input',updateBrightnessLabels);
$('weatherMode').addEventListener('change',updateForecastControls);
$('settings').addEventListener('submit',async event=>{event.preventDefault();save.disabled=true;status.textContent='保存しています…';status.className='status';const body=new URLSearchParams({enabled:$('enabled').checked?'1':'0',start:$('start').value,end:$('end').value,dayBrightness:$('dayBrightness').value,indicatorBrightness:$('indicatorBrightness').value,forecastMode:$('weatherMode').value==='forecast'?'1':'0',forecastHours:$('forecastHours').value});try{const response=await fetch('/api/settings',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body});const result=await response.json();if(!response.ok)throw Error(result.message||'保存に失敗しました');status.textContent='保存しました';status.className='status ok'}catch(error){status.textContent=error.message;status.className='status error'}finally{save.disabled=false}});
function downloadPreview(){$('panelPreview').toBlob(blob=>{if(!blob)return;const link=document.createElement('a');link.href=URL.createObjectURL(blob);link.download='weather-led-panel.png';link.click();setTimeout(()=>URL.revokeObjectURL(link.href),1000)},'image/png')}
$('downloadPreview').addEventListener('click',downloadPreview);load();loadDeviceInfo();updatePreview();updateRuntimeStatus();setInterval(updatePreview,500);setInterval(updateRuntimeStatus,2000);
</script></body></html>
)HTML";

bool isPrivateIpv4(const IPAddress& address) {
  return address[0] == 10 ||
         (address[0] == 172 && address[1] >= 16 && address[1] <= 31) ||
         (address[0] == 192 && address[1] == 168) ||
         (address[0] == 169 && address[1] == 254);
}
}

WeatherPanelControlCenter::WeatherPanelControlCenter(DisplaySettings& settings, P3RGB64x64MatrixPanel& panel)
  : _server(DeviceConfiguration::kControlCenterWebPort), _settings(settings), _panel(panel) {}

void WeatherPanelControlCenter::begin() {
  _server.on("/", HTTP_GET, [this]() { handleHome(); });
  _server.on("/api/device", HTTP_GET, [this]() { handleGetDeviceInfo(); });
  _server.on("/api/framebuffer", HTTP_GET, [this]() { handleGetFrameBuffer(); });
  _server.on("/api/status", HTTP_GET, [this]() { handleGetRuntimeStatus(); });
  _server.on("/api/settings", HTTP_GET, [this]() { handleGetSettings(); });
  _server.on("/api/settings", HTTP_POST, [this]() { handleSaveSettings(); });
  _server.onNotFound([this]() {
    if (!isLocalClient()) {
      replyForbidden();
      return;
    }
    _server.send(404, kTextContentType, "Not found");
  });
  _server.begin();

  Serial.print("Weather panel control center: http://");
  Serial.print(DeviceConfiguration::kControlCenterHostname);
  Serial.println(".local/");
}

void WeatherPanelControlCenter::update() {
  _server.handleClient();
}

void WeatherPanelControlCenter::setRuntimeStatus(const WeatherPanelRuntimeStatus& status) {
  _runtimeStatus = status;
}

void WeatherPanelControlCenter::handleHome() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }
  _server.sendHeader(kCacheControlHeader, kNoStoreValue);
  _server.send_P(200, kHtmlContentType, kPage);
}

void WeatherPanelControlCenter::handleGetDeviceInfo() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }

  const String macAddress = WiFi.macAddress();
  char response[40];
  snprintf(response, sizeof(response), "{\"mac\":\"%s\"}", macAddress.c_str());
  _server.sendHeader(kCacheControlHeader, kNoStoreValue);
  _server.send(200, kJsonContentType, response);
}

void WeatherPanelControlCenter::handleGetRuntimeStatus() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }

  const bool wifiConnected = WiFi.status() == WL_CONNECTED;
  const uint32_t elapsedMillis = _runtimeStatus.hasUpdated
      ? millis() - _runtimeStatus.lastUpdateMillis
      : 0;
  char response[320];
  snprintf(response, sizeof(response),
           "{\"timeSynchronized\":%s,\"nightModeActive\":%s,"
           "\"weatherAvailable\":%s,\"lastUpdateSucceeded\":%s,"
           "\"displayedForecast\":%s,\"displayedForecastHours\":%u,"
           "\"displayBrightnessPercent\":%u,\"indicatorBrightness\":%u,"
           "\"wifiConnected\":%s,\"wifiRssi\":%d,\"hasUpdated\":%s,"
           "\"secondsSinceUpdate\":%lu}",
           _runtimeStatus.timeSynchronized ? "true" : "false",
           _runtimeStatus.nightModeActive ? "true" : "false",
           _runtimeStatus.weatherAvailable ? "true" : "false",
           _runtimeStatus.lastUpdateSucceeded ? "true" : "false",
           _runtimeStatus.displayedForecast ? "true" : "false",
           static_cast<unsigned int>(_runtimeStatus.displayedForecastHours),
           static_cast<unsigned int>(_runtimeStatus.displayBrightnessPercent),
           static_cast<unsigned int>(_runtimeStatus.indicatorBrightness),
           wifiConnected ? "true" : "false", wifiConnected ? WiFi.RSSI() : -127,
           _runtimeStatus.hasUpdated ? "true" : "false",
           static_cast<unsigned long>(elapsedMillis / 1000UL));
  _server.sendHeader(kCacheControlHeader, kNoStoreValue);
  _server.send(200, kJsonContentType, response);
}

void WeatherPanelControlCenter::handleGetFrameBuffer() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }

  constexpr size_t kFramePixelCount = MatrixPanelConfiguration::kPixelCount;
  constexpr size_t kRgbBytesPerPixel = MatrixPanelConfiguration::kRgb888BytesPerPixel;
  constexpr size_t kRgbaBytesPerPixel = kRgbBytesPerPixel + 1;
  constexpr size_t kChunkPixelCount = MatrixPanelConfiguration::kPreviewChunkPixelCount;
  uint8_t rgbBuffer[kChunkPixelCount * kRgbBytesPerPixel];
  uint8_t rgbaBuffer[kChunkPixelCount * kRgbaBytesPerPixel];

  _server.sendHeader(kCacheControlHeader, kNoStoreValue);
  _server.setContentLength(kFramePixelCount * kRgbaBytesPerPixel);
  _server.send(200, kFrameBufferContentType, "");

  for (size_t firstPixel = 0; firstPixel < kFramePixelCount;
       firstPixel += kChunkPixelCount) {
    size_t pixelsInChunk = kFramePixelCount - firstPixel;
    if (pixelsInChunk > kChunkPixelCount) {
      pixelsInChunk = kChunkPixelCount;
    }
    _panel.copyRgb888Pixels(firstPixel, rgbBuffer, pixelsInChunk);
    for (size_t pixel = 0; pixel < pixelsInChunk; ++pixel) {
      const size_t rgbOffset = pixel * kRgbBytesPerPixel;
      const size_t rgbaOffset = pixel * kRgbaBytesPerPixel;
      rgbaBuffer[rgbaOffset] = rgbBuffer[rgbOffset];
      rgbaBuffer[rgbaOffset + 1] = rgbBuffer[rgbOffset + 1];
      rgbaBuffer[rgbaOffset + 2] = rgbBuffer[rgbOffset + 2];
      rgbaBuffer[rgbaOffset + 3] = kPreviewAlphaValue;
    }
    _server.sendContent(reinterpret_cast<const char*>(rgbaBuffer),
                        pixelsInChunk * kRgbaBytesPerPixel);
  }
}

void WeatherPanelControlCenter::handleGetSettings() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }

  const DisplaySettingsData configuration = _settings.get();
  char response[160];
  snprintf(response, sizeof(response),
           "{\"enabled\":%s,\"startHour\":%u,\"endHour\":%u,"
           "\"dayBrightnessPercent\":%u,\"indicatorBrightness\":%u,"
           "\"forecastMode\":%s,\"forecastHours\":%u}",
           configuration.nightModeEnabled ? "true" : "false",
           static_cast<unsigned int>(configuration.nightStartHour),
           static_cast<unsigned int>(configuration.nightEndHour),
           static_cast<unsigned int>(configuration.dayBrightnessPercent),
           static_cast<unsigned int>(configuration.indicatorBrightness),
           configuration.forecastModeEnabled ? "true" : "false",
           static_cast<unsigned int>(configuration.forecastHours));
  _server.sendHeader(kCacheControlHeader, kNoStoreValue);
  _server.send(200, kJsonContentType, response);
}

void WeatherPanelControlCenter::handleSaveSettings() {
  if (!isLocalClient()) {
    replyForbidden();
    return;
  }
  if (!_server.hasArg("enabled") || !_server.hasArg("start") || !_server.hasArg("end") ||
      !_server.hasArg("dayBrightness") || !_server.hasArg("indicatorBrightness") ||
      !_server.hasArg("forecastMode") || !_server.hasArg("forecastHours")) {
    _server.send(400, kJsonContentType, "{\"message\":\"設定値が不足しています\"}");
    return;
  }

  const int startHour = _server.arg("start").toInt();
  const int endHour = _server.arg("end").toInt();
  const int dayBrightness = _server.arg("dayBrightness").toInt();
  const int indicatorBrightness = _server.arg("indicatorBrightness").toInt();
  const int forecastHours = _server.arg("forecastHours").toInt();
  const String enabledValue = _server.arg("enabled");
  const String forecastModeValue = _server.arg("forecastMode");
  if ((enabledValue != "0" && enabledValue != "1") ||
      (forecastModeValue != "0" && forecastModeValue != "1") ||
      startHour < 0 || startHour >= DeviceConfiguration::kHoursPerDay ||
      endHour < 0 || endHour >= DeviceConfiguration::kHoursPerDay ||
      startHour == endHour ||
      dayBrightness < DeviceConfiguration::kMinimumDayBrightnessPercent ||
      dayBrightness > DeviceConfiguration::kMaximumDayBrightnessPercent ||
      indicatorBrightness < 0 ||
      indicatorBrightness > DeviceConfiguration::kMaximumIndicatorBrightness ||
      forecastHours < DeviceConfiguration::kForecastIntervalHours ||
      forecastHours > DeviceConfiguration::kMaximumForecastHours ||
      forecastHours % DeviceConfiguration::kForecastIntervalHours != 0) {
    _server.send(400, kJsonContentType, "{\"message\":\"設定値が正しくありません\"}");
    return;
  }

  const DisplaySettingsData configuration{
      enabledValue == "1", static_cast<uint8_t>(startHour), static_cast<uint8_t>(endHour),
      static_cast<uint8_t>(dayBrightness), static_cast<uint8_t>(indicatorBrightness),
      forecastModeValue == "1", static_cast<uint8_t>(forecastHours)};
  if (!_settings.set(configuration)) {
    _server.send(500, kJsonContentType, "{\"message\":\"本体への保存に失敗しました\"}");
    return;
  }
  _server.send(200, kJsonContentType, "{\"saved\":true}");
}

bool WeatherPanelControlCenter::isLocalClient() {
  const IPAddress remoteAddress = _server.client().remoteIP();
  const IPAddress localAddress = WiFi.localIP();
  if (!isPrivateIpv4(remoteAddress)) {
    return false;
  }

  const IPAddress subnetMask = WiFi.subnetMask();
  for (size_t octet = 0; octet < 4; ++octet) {
    if ((remoteAddress[octet] & subnetMask[octet]) !=
        (localAddress[octet] & subnetMask[octet])) {
      return false;
    }
  }
  return true;
}

void WeatherPanelControlCenter::replyForbidden() {
  _server.send(403, kTextContentType, "Local network only");
}
