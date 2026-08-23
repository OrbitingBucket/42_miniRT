const fs = require('fs');
const createMiniRT = require('./out/minirt.js');

createMiniRT().then((m) => {
	const ptr = m._rt_init();
	let frames = 0;
	while (frames < 2)
		frames += m._rt_tick();
	const buf = Buffer.from(m.HEAPU8.buffer, ptr, 800 * 600 * 4);
	fs.writeFileSync(process.argv[2], buf);
	console.log('wasm full frame written');
});
