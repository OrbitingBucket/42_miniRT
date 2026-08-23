const fs = require('fs');
const createMiniRT = require('./out/minirt.js');

const w = parseInt(process.argv[3], 10);
const h = parseInt(process.argv[4], 10);

createMiniRT().then((m) => {
	const ptr = m._rt_init(0);
	m._rt_render_band(0, h, 0);
	const buf = Buffer.from(m.HEAPU8.buffer, ptr, w * h * 4);
	fs.writeFileSync(process.argv[2], buf);
	console.log('wasm full frame written');
});
