importScripts('minirt.js');

var mod = null;
var ptr = 0;
var pending = null;

function handle(d) {
	mod._rt_set_cam(d.cam[0], d.cam[1], d.cam[2], d.cam[3], d.cam[4], d.cam[5]);
	mod._rt_render_band(d.y0, d.y1, d.q);
	var off = ptr + d.y0 * d.w * 4;
	var copy = mod.HEAPU8.slice(off, off + (d.y1 - d.y0) * d.w * 4);
	postMessage({ frame: d.frame, y0: d.y0, q: d.q, buf: copy.buffer }, [copy.buffer]);
}

createMiniRT().then(function (m) {
	mod = m;
	ptr = m._rt_init();
	postMessage({ ready: 1 });
	if (pending) {
		handle(pending);
		pending = null;
	}
});

onmessage = function (e) {
	if (!mod) {
		pending = e.data;
		return;
	}
	handle(e.data);
};
