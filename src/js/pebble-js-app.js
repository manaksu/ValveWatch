/*
 * ValveWatch — PebbleKit JS
 * AppMessage keys:
 *   0 = watermark : 0=off, 1=Steam Deck, 2=Valve logo, 3=Battery (white λ), 4=Battery distressed (orange λ)
 */
function loadCfg() {
  return {
    watermark: +(localStorage.getItem('vw_watermark') !== null
      ? localStorage.getItem('vw_watermark') : '1')
  };
}
function saveCfg(c) {
  localStorage.setItem('vw_watermark', c.watermark);
}
function sendMsg(c) {
  Pebble.sendAppMessage(
    { '0': c.watermark },
    function() { console.log('ValveWatch: sent watermark=' + c.watermark); },
    function(e) { console.log('ValveWatch: send failed', JSON.stringify(e)); }
  );
}
function buildConfig(c) {
  function radio(name, opts, sel) {
    return opts.map(function(l, i) {
      return '<label class="opt"><input type="radio" name="' + name +
        '" value="' + i + '"' + (i === sel ? ' checked' : '') +
        '><span>' + l + '</span></label>';
    }).join('');
  }
  var h = '<!DOCTYPE html><html><head>'
    + '<meta charset="utf-8">'
    + '<meta name="viewport" content="width=device-width,initial-scale=1">'
    + '<style>'
    + 'body{margin:0;font:15px/1.6 -apple-system,sans-serif;background:#0d0d0d;color:#ccc;padding:20px}'
    + 'h1{font-size:18px;color:#fff;margin:0 0 4px}'
    + 'p{font-size:12px;color:#555;margin:0 0 24px}'
    + 'h3{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:#555;margin:22px 0 8px}'
    + 'h3:first-of-type{margin-top:0}'
    + '.opt{display:flex;align-items:center;gap:12px;background:#1a1a1a;border-radius:8px;'
    +      'padding:13px;margin:5px 0;cursor:pointer}'
    + '.opt input{accent-color:#e03030;width:18px;height:18px;flex-shrink:0;margin:0}'
    + '.opt span{font-size:14px}'
    + '#s{display:block;width:100%;padding:14px;background:#e03030;color:#fff;border:none;'
    +    'border-radius:8px;font-size:15px;font-weight:bold;margin-top:24px;cursor:pointer;'
    +    'box-sizing:border-box}'
    + '</style></head><body>'
    + '<h1>ValveWatch</h1>'
    + '<p>A tribute to Valve &amp; Steam Deck</p>'
    + '<h3>Bottom Display</h3>'
    + radio('watermark', [
        'Off',
        'Steam Deck outline',
        'Valve logo',
        'Battery \u2014 Half-Life \u03bb',
        'Battery \u2014 Half-Life \u03bb distressed'
      ], c.watermark)
    + '<button id="s">Save</button>'
    + '<script>'
    + 'document.getElementById("s").onclick=function(){'
    + 'function g(n){var e=document.querySelector("input[name="+n+"]:checked");return e?+e.value:1;}'
    + 'location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify({'
    + 'watermark:g("watermark")}));'
    + '};<\/script></body></html>';
  return 'data:text/html,' + encodeURIComponent(h);
}

Pebble.addEventListener('ready', function() {
  console.log('ValveWatch: ready');
  sendMsg(loadCfg());
});
Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(buildConfig(loadCfg()));
});
Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response || e.response === '' || e.response === 'CANCELLED') return;
  var raw = e.response;
  if (raw.indexOf('#') !== -1) raw = raw.substring(raw.lastIndexOf('#') + 1);
  var c;
  try { c = JSON.parse(decodeURIComponent(raw)); } catch(err) { return; }
  saveCfg(c);
  sendMsg(c);
});
