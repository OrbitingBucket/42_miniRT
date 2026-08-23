createMiniRT().then(function (m) {
	var canvas = document.getElementById('rt');
	var ctx = canvas.getContext('2d');
	var ptr = m._rt_init();
	var img = new ImageData(800, 600);
	var keys = { w: 119, s: 115, a: 97, d: 100 };

	function blit() {
		img.data.set(m.HEAPU8.subarray(ptr, ptr + 800 * 600 * 4));
		ctx.putImageData(img, 0, 0);
	}

	function loop() {
		var t0 = performance.now();
		var ready = 0;
		while (performance.now() - t0 < 12 && !m._rt_idle())
			ready |= m._rt_tick();
		if (ready)
			blit();
		requestAnimationFrame(loop);
	}

	function pos(e) {
		var r = canvas.getBoundingClientRect();
		var cx = (e.touches ? e.touches[0].clientX : e.clientX) - r.left;
		var cy = (e.touches ? e.touches[0].clientY : e.clientY) - r.top;
		return [Math.round(cx * 800 / r.width), Math.round(cy * 600 / r.height)];
	}

	function press(e) { var p = pos(e); m._rt_press(p[0], p[1]); e.preventDefault(); }
	function move(e) { var p = pos(e); m._rt_move(p[0], p[1]); }
	function release() { m._rt_release(); }

	canvas.addEventListener('mousedown', press);
	window.addEventListener('mousemove', move);
	window.addEventListener('mouseup', release);
	canvas.addEventListener('touchstart', press, { passive: false });
	canvas.addEventListener('touchmove', function (e) { move(e); e.preventDefault(); }, { passive: false });
	window.addEventListener('touchend', release);
	canvas.addEventListener('wheel', function (e) {
		m._rt_wheel(e.deltaY > 0 ? 1 : -1);
		e.preventDefault();
	}, { passive: false });
	window.addEventListener('keydown', function (e) {
		if (keys[e.key])
			m._rt_key(keys[e.key]);
	});

	requestAnimationFrame(loop);
});
